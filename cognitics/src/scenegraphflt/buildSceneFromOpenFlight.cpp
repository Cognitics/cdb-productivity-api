/*************************************************************************
Copyright (c) 2019 Cognitics, Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
****************************************************************************/

#include "scenegraphflt/scenegraphflt.h"
#include <scenegraph/LOD.h>
#include <flt/flt.h>
#include <flt/Header.h>
#include <flt/ColorPalette.h>
#include <flt/TexturePalette.h>
#include <flt/MaterialPalette.h>
#include <flt/VertexPalette.h>
#include <flt/VertexWithColor.h>
#include <flt/VertexWithColorUV.h>
#include <flt/VertexWithColorNormalUV.h>
#include <flt/VertexWithColorNormal.h>
#include <flt/PushLevel.h>
#include <flt/PopLevel.h>
#include <flt/Group.h>
#include <flt/Object.h>
#include <flt/Face.h>
#include <flt/LongID.h>
#include <flt/LightPoint.h>
#include <flt/Comment.h>
#include <flt/VertexList.h>
#include <flt/Matrix.h>
#include <flt/ExternalReference.h>
#include <flt/LevelOfDetail.h>
#include <boost/tokenizer.hpp>
#include <iostream>
#include <algorithm>
#include <dom/dom.h>
#include <boost/algorithm/string.hpp>

namespace scenegraph
{
    struct OpenFlightSceneBuilder
    {
        std::string filename;
        bool setTexturePath;
        std::string fltFilePath;
        flt::OpenFlight *fltFile;
        Scene *rootScene;
        Scene *currentScene;
        Scene *nextGroup;
        int depth;
        std::map<int, Scene *> depthMap;
        std::vector<flt::ColorPalette *> fltColors;
        std::vector<flt::TexturePalette *> fltTextures;
        std::vector<flt::MaterialPalette *> fltMaterials;
        std::vector<flt::Record *> fltVertices;
        ccl::uint32_t vertexPalettePosition;
        Face *currentFace;
        ExternalReference *currentExternalReference;

        Color getColorFromInt32(ccl::int32_t color)
        {
            Color result;
            result.a = ((color >> 24) & 0xFF) / 0xFF;
            result.b = ((color >> 16) & 0xFF) / 0xFF;
            result.g = ((color >> 8) & 0xFF) / 0xFF;
            result.r = (color & 0xFF) / 0xFF;
            return result;
        }

        Color getColorFromUInt32(ccl::uint32_t color)
        {
            Color result;
            result.a = ((color >> 24) & 0xFF) / 0xFF;
            result.b = ((color >> 16) & 0xFF) / 0xFF;
            result.g = ((color >> 8) & 0xFF) / 0xFF;
            result.r = (color & 0xFF) / 0xFF;
            return result;
        }

        Color getColorFromPalette(ccl::int32_t index)
        {
            // 0-6 = intensity
            // 7-15 = color index
            ccl::int32_t realIndex = index >> 7;
            float intensity = (float)(index & 0x7F) / 127.0f;
            if((realIndex < 0) || (realIndex > 1023))
                return Color();
            Color result = getColorFromInt32(fltColors[0]->brightestRGB[realIndex]);
            result.r *= intensity;
            result.g *= intensity;
            result.b *= intensity;
            return result;
        }

        sfa::Matrix asSFAMatrix(flt::Matrix *matrix)
        {
            sfa::Matrix m;
            double *d = m.getData();
            for(size_t a = 0; a < 12; ++a)
            {
                *d = matrix->matrix[((a % 4) * 4) + int(a / 4)];
                ++d;
            }
            return m;
        }

        OpenFlightSceneBuilder(const std::string &filename, bool setTexturePath) : filename(filename), setTexturePath(setTexturePath), vertexPalettePosition(0), currentFace(NULL), currentExternalReference(NULL), rootScene(NULL), currentScene(NULL), nextGroup(NULL)
        {
            ccl::FileInfo fileinfo(filename);
            fltFilePath = fileinfo.getDirName();
        }

        ~OpenFlightSceneBuilder(void)
        {
            for(size_t i = 0, c = fltColors.size(); i < c; ++i)
                delete fltColors[i];
            for(size_t i = 0, c = fltTextures.size(); i < c; ++i)
                delete fltTextures[i];
            for(size_t i = 0, c = fltMaterials.size(); i < c; ++i)
                delete fltMaterials[i];
            for(size_t i = 0, c = fltVertices.size(); i < c; ++i)
                delete fltVertices[i];
        }

        void build_HEADER(flt::Header *header)
        {
            currentScene->attributes.setAttribute("origin_lat",header->originLatitude);
            currentScene->attributes.setAttribute("origin_lon",header->originLongitude);

            currentScene->attributes.setAttribute("sw_lat",header->southwestLatitude);
            currentScene->attributes.setAttribute("sw_lon",header->southwestLongitude);
            currentScene->attributes.setAttribute("ne_lat",header->northeastLatitude);
            currentScene->attributes.setAttribute("ne_lon",header->northeastLongitude);

            if(header->projectionType == 4)
                currentScene->attributes.setAttribute("utm_zone", header->utmZone);

            delete header;
        }

        void build_COLORPALETTE(flt::ColorPalette *colorPalette)
        {
            fltColors.push_back(colorPalette);
        }

        void build_TEXTUREPALETTE(flt::TexturePalette *texturePalette)
        {
            fltTextures.push_back(texturePalette);
        }

        void build_MATERIALPALETTE(flt::MaterialPalette *materialPalette)
        {
            fltMaterials.push_back(materialPalette);
        }

        void build_VERTEXPALETTE_VERTEX(flt::Record *record)
        {
            fltVertices.push_back(record);
        }

        void build_VERTEXPALETTE(flt::VertexPalette *vertexPalette)
        {
            vertexPalettePosition = vertexPalette->position;
            while((fltFile->getNextOpcode() == flt::Record::FLT_VERTEXWITHCOLOR)
                || (fltFile->getNextOpcode() == flt::Record::FLT_VERTEXWITHCOLORNORMAL)
                || (fltFile->getNextOpcode() == flt::Record::FLT_VERTEXWITHCOLORNORMALUV)
                || (fltFile->getNextOpcode() == flt::Record::FLT_VERTEXWITHCOLORUV))
            {
                build_VERTEXPALETTE_VERTEX(fltFile->getNextRecord());
            }
            delete vertexPalette;
        }

        void build_GROUP(flt::Group *group)
        {
            Scene *sceneGroup = new Scene;
            sceneGroup->parent = currentScene;
            sceneGroup->name = group->id;
            currentScene->children.push_back(sceneGroup);
            nextGroup = sceneGroup;
        }

        void build_OBJECT(flt::Object *object)
        {
            Scene *sceneGroup = new Scene;
            sceneGroup->parent = currentScene;
            sceneGroup->name = object->id;
            sceneGroup->fltobject = "Object";
            currentScene->children.push_back(sceneGroup);
            nextGroup = sceneGroup;
        }

        void build_LEVELOFDETAIL(flt::LevelOfDetail *lod)
        {
            LOD *sceneGroup = new LOD;
            sceneGroup->parent = currentScene;
            sceneGroup->name = lod->id;
            sceneGroup->center = sfa::Point(lod->centerX, lod->centerY, lod->centerZ);
            sceneGroup->ranges.push_back(LODRange(lod->switchOutDistance, lod->switchInDistance));
            currentScene->children.push_back(sceneGroup);
            nextGroup = sceneGroup;
        }

        void build_FACE(flt::Face *face)
        {
            currentScene->faces.push_back(Face());
            currentFace = &(currentScene->faces.at(currentScene->faces.size() - 1));
            currentFace->id = face->id;
            currentFace->smc = face->surfaceMaterialCode;
            currentFace->featureID = face->featureID;
            currentFace->drawBothSides = (face->drawType==1);
            if(face->flags[0])
                currentFace->attributes.setAttribute("terrain", true);
            if(face->flags[4])
                currentFace->attributes.setAttribute("footprint", true);
            if(face->flags[5])
                currentFace->attributes.setAttribute("hidden", true);
            if(face->flags[6])
                currentFace->attributes.setAttribute("roofline", true);

            if(face->flags[3])
            {
                currentFace->primaryColor = getColorFromUInt32(face->primaryPackedColor);
                currentFace->transparency = face->transparency/65535;
                currentFace->alternateColor = getColorFromUInt32(face->altPackedColor);
            }
            else
            {
                currentFace->primaryColor = getColorFromPalette(face->primaryColorIndex);
                currentFace->transparency = face->transparency/65535;
                currentFace->alternateColor = getColorFromPalette(face->altColorIndex);
            }

            MappedTexture tex;
            for(size_t i = 0, c = fltTextures.size(); i < c; ++i)
            {
                if(fltTextures[i]->textureIndex == face->texturePatternIndex)
                {
                    std::string texturefile = fltTextures[i]->fileName.c_str();
                    if(setTexturePath)
                    {
                        ccl::FileInfo textureFI(texturefile);
                        texturefile = fltFilePath + "/" + textureFI.getBaseName();
                        /*
                        if(texturefile.size() > 1 && texturefile[0] == '.')
                        {
                            if(texturefile[1] == '\\' || texturefile[1] == '/')
                            {
                                texturefile = fltFilePath + "/" + texturefile.substr(2, texturefile.size() - 2);
                            }
                        }
#ifdef WIN32
                        for(size_t j = 0, jlen = texturefile.size(); j < jlen; j++)
                        {
                            if(texturefile[j] == '/')
                                texturefile[j] = '\\';
                        }
#endif
                        */

                    }
                    tex.SetTextureName(texturefile);
                    currentFace->textures.push_back(tex);
                    break;
                }
            }

            Material mat;
            for(size_t i = 0, c = fltMaterials.size(); i < c; ++i)
            {
                if(fltMaterials[i]->index == face->materialIndex)
                {
                    mat.ambient = Color(fltMaterials[i]->ambientRed, fltMaterials[i]->ambientGreen, fltMaterials[i]->ambientBlue);
                    mat.diffuse = Color(fltMaterials[i]->diffuseRed, fltMaterials[i]->diffuseGreen, fltMaterials[i]->diffuseBlue);
                    mat.specular = Color(fltMaterials[i]->specularRed, fltMaterials[i]->specularGreen, fltMaterials[i]->specularBlue);
                    mat.emission = Color(fltMaterials[i]->emissiveRed, fltMaterials[i]->emissiveGreen, fltMaterials[i]->emissiveBlue);
                    mat.shine = fltMaterials[i]->shininess;
                    mat.transparency = fltMaterials[i]->alpha;
                    currentFace->materials.push_back(mat);
                }
            }

            delete face;
        }

        void build_PUSHLEVEL(flt::PushLevel *pushLevel)
        {
            if(nextGroup)
            {
                depthMap[depth] = currentScene;
                currentScene = nextGroup;
            }
            nextGroup = NULL;
            ++depth;
            delete pushLevel;
        }

        void build_POPLEVEL(flt::PopLevel *popLevel)
        {
            --depth;
            if(depthMap.find(depth) != depthMap.end())
            {
                currentScene = depthMap[depth];
                depthMap.erase(depth);
            }
            currentFace = NULL;
            currentExternalReference = NULL;
            nextGroup = NULL;
            delete popLevel;
        }

        void build_LONGID(flt::LongID *longID)
        {
            if(!currentFace)
            {
                if(nextGroup)
                    nextGroup->name = longID->id;
                else
                    currentScene->name = longID->id;
            }
            delete longID;
        }

        void build_COMMENT(flt::Comment *comment)
        {
            std::string commentstr = comment->textDescription;
            // Get rid of leading and trailing spaces, and all cr/lf
            boost::trim(commentstr);
            commentstr.erase(std::remove(commentstr.begin(), commentstr.end(), '\n'), commentstr.end());
            commentstr.erase(std::remove(commentstr.begin(), commentstr.end(), '\r'), commentstr.end());

            if (currentFace)
                currentFace->legacyComment = commentstr;
            // Test that the comment is long enough to possibly have a XML tag (at minimum needs the outer tag and the #xml
            if (commentstr.length() > 14)//5 bytes for </edm>, 4 bytes for <edm>, 4 bytes for #xml, and some content
            {
                std::string attrlisttag("<AttributeList>");
                std::string xmltag("#xml");
                if (commentstr.compare(0, xmltag.size(), xmltag) == 0)
                {
                    // Make it real xml
                    std::string xmlstr = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
                    xmlstr += commentstr.substr(4,commentstr.size()-4);
                    // If the comment starts with #xml, parse out the next line forward worth of XML in the edm tag
                    commentstr = "";
                    dom::DOMImplementationSP domImplementation(new dom::DOMImplementation);
                    dom::LSReader datastr(xmlstr);
                    dom::LSInputSP input(new dom::LSInput);
                    input->characterStream = &datastr;
                    dom::LSParserSP parser(domImplementation->createLSParser());
                    dom::DocumentSP doc = parser->parse(input);
                    if (doc)
                    {
                        
                        dom::ElementSP element = doc->getDocumentElement();
                        if (element->getNodeName() == "edm")
                        {
                            
                            dom::ElementList childElements = element->getChildElements();
                            for (dom::ElementList::iterator it = childElements.begin(), end = childElements.end(); it != end; ++it)
                            {
                                dom::ElementSP child = *it;
                                ccl::Variant namevar = child->getAttribute("name");
                                if (namevar.as_string() == "building_component_type")
                                {
                                    ccl::Variant var = child->getAttribute("value");
                                    std::string varstr = var.as_string();
                                    commentstr += varstr;
                                    commentstr += ";";
                                }
                                //"transportation_use" value="road"
                                if (namevar.as_string() == "transportation_use")
                                {
                                    ccl::Variant var = child->getAttribute("value");
                                    std::string varstr = var.as_string();
                                    if(varstr=="road")
                                    {
                                        commentstr += "trafficable;";
                                    }
                                }                                
                            }
                        }
                    }
                }
                else if (commentstr.size() > 30 && (commentstr.compare(0, attrlisttag.size(), attrlisttag) == 0))
                {
                    //<AttributeList><FACC VAL="GB005" SUBVAL="000"/><MATERIAL NAME="BM_ASPHALT"/></AttributeList>
                    // Make it real xml
                    std::string xmlstr = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
                    xmlstr += commentstr;
                    dom::DOMImplementationSP domImplementation(new dom::DOMImplementation);
                    dom::LSReader datastr(xmlstr);
                    dom::LSInputSP input(new dom::LSInput);
                    input->characterStream = &datastr;
                    dom::LSParserSP parser(domImplementation->createLSParser());
                    dom::DocumentSP doc = parser->parse(input);
                    if (doc)
                    {
                        dom::ElementSP element = doc->getDocumentElement();
                        if (element->getNodeName() == "AttributeList")
                        {
                            dom::ElementList childElements = element->getChildElements();
                            for (dom::ElementList::iterator it = childElements.begin(), end = childElements.end(); it != end; ++it)
                            {
                                dom::ElementSP child = *it;
                                std::string key = child->getNodeName();
                                std::transform(key.begin(), key.end(), key.begin(), tolower);
                                if (key == "facc")
                                {
                                    dom::Variant val = child->getAttribute("VAL");
                                    //add key=FACC, value = VAL attribute
                                    //add key=FACC_SUB value = SUBVAL attribute
                                    if (currentFace)
                                        currentFace->attributes.setAttribute("facc", val.as_string());

                                    val = child->getAttribute("SUBVAL");
                                    if (currentFace)
                                        currentFace->attributes.setAttribute("facc_sub", val.as_string());


                                }
                                else if (key == "material")
                                {
                                    //add key = MATERIAL, value = NAME attribute
                                    dom::Variant val = child->getAttribute("NAME");
                                    if (currentFace)
                                        currentFace->attributes.setAttribute("material", val.as_string());
                                }
                                else
                                {
                                    //TODO: handle any attribute?
                                    //The problem is that these XML tags are unstructured, so passing them as
                                    //attributes doesn't work very well unless we can map them here (see the above special
                                    //case keys)
                                }
                            }
                        }
                    }
                }
                else
                {
                    // Only look at the flags if they are alpha numeric characters
                    for(size_t i=0,ic=commentstr.size();i<ic;i++)
                    {
                        if(!isalnum(commentstr[i]) && commentstr[i]!=';')
                        {
                            commentstr = "";
                            break;
                        }
                    }
                }
                /*
                <edm>
                <"building_component_type" value = "tunnelInterior" / >
                <"emitted_diffuse_light_scaled_intensity" value = "0.73481" / >
                < / edm>
                */
            }
            

            if(currentFace)
            {
                boost::char_separator<char> sep(";");
                boost::tokenizer<boost::char_separator<char> > tokens(commentstr, sep);
                for(boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin(), end = tokens.end(); it != end; ++it)
                    currentFace->attributes.setAttribute(*it, true);
            }
            else
            {
                boost::char_separator<char> sep(";");
                boost::tokenizer<boost::char_separator<char> > tokens(commentstr, sep);
                for(boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin(), end = tokens.end(); it != end; ++it)
                {
                    if(*it != "")
                    {
                        if(nextGroup)
                            nextGroup->attributes.setAttribute(*it, true);
                        else
                            currentScene->attributes.setAttribute(*it, true);
                    }
                }
            }
            delete comment;
        }

        void build_VERTEXLIST(flt::VertexList *vertexList)
        {
            if(!currentFace)
                return;
            for(size_t i = 0, c = vertexList->offsets.size(); i < c; ++i)
            {
                ccl::uint32_t vertexPosition = vertexPalettePosition + vertexList->offsets[i];
                for(flt::RecordList::iterator it = fltVertices.begin(), end = fltVertices.end(); it != end; ++it)
                {
                    flt::Record *vertexRecord = *it;
                    if(vertexRecord->position != vertexPosition)
                        continue;

                    sfa::Point point;
                    sfa::Point normal;
                    sfa::Point uv;
                    bool hasNormal = false;
                    bool hasUV = false;
                    if(vertexRecord->getRecordType() == flt::Record::FLT_VERTEXWITHCOLOR)
                    {
                        flt::VertexWithColor *rec = dynamic_cast<flt::VertexWithColor *>(vertexRecord);
                        point = sfa::Point(rec->x, rec->y, rec->z);
                    }
                    if(vertexRecord->getRecordType() == flt::Record::FLT_VERTEXWITHCOLORNORMAL)
                    {
                        flt::VertexWithColorNormal *rec = dynamic_cast<flt::VertexWithColorNormal *>(vertexRecord);
                        point = sfa::Point(rec->x, rec->y, rec->z);
                        normal = sfa::Point(rec->i, rec->j, rec->k);
                        hasNormal = true;
                    }
                    if(vertexRecord->getRecordType() == flt::Record::FLT_VERTEXWITHCOLORNORMALUV)
                    {
                        flt::VertexWithColorNormalUV *rec = dynamic_cast<flt::VertexWithColorNormalUV *>(vertexRecord);
                        point = sfa::Point(rec->x, rec->y, rec->z);
                        normal = sfa::Point(rec->i, rec->j, rec->k);
                        uv = sfa::Point(rec->u, rec->v);
                        hasNormal = true;
                        hasUV = true;
                    }
                    if(vertexRecord->getRecordType() == flt::Record::FLT_VERTEXWITHCOLORUV)
                    {
                        flt::VertexWithColorUV *rec = dynamic_cast<flt::VertexWithColorUV *>(vertexRecord);
                        point = sfa::Point(rec->x, rec->y, rec->z);
                        uv = sfa::Point(rec->u, rec->v);
                        hasUV = true;
                    }

                    int ptid = currentFace->addVert(point);
                    //ptid should always equal i
                    assert(ptid==i);
                    if(hasNormal)
                        currentFace->setNormalN(int(i), normal);
                    if(hasUV && (currentFace->textures.size() > 0))
                        currentFace->textures[0].uvs.push_back(uv);
                }
            }

            delete vertexList;
        }

        void build_MATRIX(flt::Matrix *matrix)
        {
            if(currentExternalReference)
            {
                sfa::Matrix extMatrix = asSFAMatrix(matrix);
                currentExternalReference->scale = extMatrix.getScale();
                currentExternalReference->position = extMatrix.getTranslation();
                currentExternalReference->attitude = extMatrix.getRotation();
            }
            else
            {
                if(nextGroup)
                    nextGroup->matrix = asSFAMatrix(matrix);
                else
                    currentScene->matrix = asSFAMatrix(matrix);
            }
            delete matrix;
        }

        void build_EXTERNALREFERENCE(flt::ExternalReference *externalReference)
        {
            currentScene->externalReferences.push_back(ExternalReference());
            currentExternalReference = &(currentScene->externalReferences.at(currentScene->externalReferences.size() - 1));
            currentExternalReference->filename = externalReference->path;
            delete externalReference;
        }

        void build_LIGHTPOINT(flt::LightPoint *lightPoint)
        {
            scenegraph::LightPoint light;
            //
            currentScene->lightPoints.push_back(light);

        }

        bool build(void)
        {
            fltFile = flt::OpenFlight::open(filename);
            if(!fltFile)
                return false;

            rootScene = new Scene;
            rootScene->name = "root";
            currentScene = rootScene;
            nextGroup = currentScene;    // root PushLevel doesn't include a group, so we predefine the next group for push level assignment
            depth = 0;

            while(flt::Record *record = fltFile->getNextRecord())
            {
                ccl::uint16_t nextOpcode = fltFile->getNextOpcode();

                if(record->getRecordType() == flt::Record::FLT_HEADER)
                {
                    build_HEADER(dynamic_cast<flt::Header *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_COLORPALETTE)
                {
                    build_COLORPALETTE(dynamic_cast<flt::ColorPalette *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_TEXTUREPALETTE)
                {
                    build_TEXTUREPALETTE(dynamic_cast<flt::TexturePalette *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_MATERIALPALETTE)
                {
                    build_MATERIALPALETTE(dynamic_cast<flt::MaterialPalette *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_VERTEXPALETTE)
                {
                    build_VERTEXPALETTE(dynamic_cast<flt::VertexPalette *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_PUSHLEVEL)
                {
                    build_PUSHLEVEL(dynamic_cast<flt::PushLevel *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_POPLEVEL)
                {
                    build_POPLEVEL(dynamic_cast<flt::PopLevel *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_GROUP)
                {
                    build_GROUP(dynamic_cast<flt::Group *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_OBJECT)
                {
                    build_OBJECT(dynamic_cast<flt::Object *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_LEVELOFDETAIL)
                {
                    build_LEVELOFDETAIL(dynamic_cast<flt::LevelOfDetail *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_FACE)
                {
                    build_FACE(dynamic_cast<flt::Face *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_LONGID)
                {
                    build_LONGID(dynamic_cast<flt::LongID *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_COMMENT)
                {
                    build_COMMENT(dynamic_cast<flt::Comment *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_VERTEXLIST)
                {
                    build_VERTEXLIST(dynamic_cast<flt::VertexList *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_MATRIX)
                {
                    build_MATRIX(dynamic_cast<flt::Matrix *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_EXTERNALREFERENCE)
                {
                    build_EXTERNALREFERENCE(dynamic_cast<flt::ExternalReference *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_LIGHTPOINT)
                {
                    build_LIGHTPOINT(dynamic_cast<flt::LightPoint *>(record));
                    continue;
                }

                if(record->getRecordType() == flt::Record::FLT_INDEXEDLIGHTPOINT)
                {

                    continue;
                }
                if(record->getRecordType() == flt::Record::FLT_LIGHTPOINTAPPEARANCEPALETTE)
                {
                    //build_LIGHTPOINT(dynamic_cast<flt::LightPoint *>(record));
                    continue;
                }
                if(record->getRecordType() == flt::Record::FLT_LIGHTSOURCEPALETTE)
                {
                    //build_LIGHTPOINT(dynamic_cast<flt::LightPoint *>(record));
                    continue;
                }

                delete record;
            }

            flt::OpenFlight::destroy(fltFile);

            return true;
        }


    };


    Scene *buildSceneFromOpenFlight(const std::string &filename, bool setTexturePath)
    {
        OpenFlightSceneBuilder builder(filename, setTexturePath);
        return builder.build() ? builder.rootScene : NULL;
    }

}