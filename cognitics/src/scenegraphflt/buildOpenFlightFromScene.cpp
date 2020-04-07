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

//#pragma optimize( "", off )

#include "scenegraphflt/scenegraphflt.h"
#include <scenegraph/Face.h>
#include <scenegraph/LOD.h>
#include <flt/flt.h>
#include <vector>
#include <set>
#include <map>

namespace scenegraph
{
    struct OpenFlightVertex
    {
        float x, y, z, i, j, k, u, v;
        OpenFlightVertex(float x = 0.0f, float y = 0.0f, float z = 0.0f, float i = 0.0f, float j = 0.0f, float k = 0.0f, float u = 0.0f, float v = 0.0f)
            : x(x), y(y), z(z), i(i), j(j), k(k), u(u), v(v)
        {
        }
        bool operator==(const OpenFlightVertex &rhs) const
        {
            return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (i == rhs.i) && (j == rhs.j) && (k == rhs.k) && (u == rhs.u) && (v == rhs.v);
        }
        bool operator<(const OpenFlightVertex &rhs) const
        {
            if(x < rhs.x)
                return true;
            else if(x > rhs.x)
                return false;
            if(y < rhs.y)
                return true;
            else if(y > rhs.y)
                return false;
            if(z < rhs.z)
                return true;
            else if(z > rhs.z)
                return false;
            if(i < rhs.i)
                return true;
            else if(i > rhs.i)
                return false;
            if(j < rhs.j)
                return true;
            else if(j > rhs.j)
                return false;
            if(k < rhs.k)
                return true;
            else if(k > rhs.k)
                return false;
            if(u < rhs.u)
                return true;
            else if(u > rhs.u)
                return false;
            return (v < rhs.v);
        }
    };

    OpenFlightVertex buildOpenFlightVertexFromFace(Face &face, int i)
    {
        OpenFlightVertex v1;
        if(face.getNumVertices() > i)
        {
            sfa::Point pt = face.getVertN(i);
            v1.x = pt.X();
            v1.y = pt.Y();
            v1.z = pt.Z();
            sfa::Point n1 = face.getNormalN(i);
            v1.i = n1.X();
            v1.j = n1.Y();
            v1.k = n1.Z();
            if(face.textures.size() && face.textures[0].uvs.size())
            {
                sfa::Point uv1 = face.textures[0].uvs[i];
                v1.u = uv1.X();
                v1.v = uv1.Y();
            }
        }
        return v1;

    }

    struct SceneOpenFlightBuilder
    {
        std::string filename;
        Scene *scene;
        int revision;
        flt::OpenFlight *fltFile;
        flt::RecordList records;
        flt::Header *header;
        flt::ColorPalette *colorPalette;
        ColorList uniqueColors;
        std::map<Color, int> colorMap;
        MaterialList uniqueMaterials;
        std::map<Material, int> materialMap;
        std::vector<std::string> uniqueTextures;
        std::map<std::string, int> textureMap;
        flt::LightSourcePalette *lightSourcePalette;
        flt::VertexPalette *vertexPalette;
        std::set<OpenFlightVertex> vertexSet;
        std::vector<OpenFlightVertex> vertexVector;
        std::map<OpenFlightVertex, size_t> vertexMap;
        std::set<OpenFlightVertex> vertexSetM;
        std::vector<OpenFlightVertex> vertexVectorM;
        std::map<OpenFlightVertex, size_t> vertexMapM;
        size_t vertexSize;

        SceneOpenFlightBuilder(const std::string &filename, Scene *scene, int revision) : filename(filename), scene(scene), revision(revision), vertexSize(64)
        {
        }

        ~SceneOpenFlightBuilder(void)
        {
            for(size_t i = 0, c = records.size(); i < c; ++i)
                delete records[i];
        }

        ccl::uint32_t getUInt32FromColor(const Color &color)
        {
            return ((unsigned char)(color.a * 0xFF) << 24)
                | ((unsigned char)(color.b * 0xFF) << 16)
                | ((unsigned char)(color.g * 0xFF) << 8)
                | (unsigned char)(color.r * 0xFF);
        }

        flt::Matrix *asFLTMatrix(const sfa::Matrix &matrix)
        {
            flt::Matrix *matrixRecord = new flt::Matrix;
            const double *d = matrix.getData();
            matrixRecord->matrix.resize(16, 0.0f);
            for(size_t a = 0; a < 12; ++a)
                matrixRecord->matrix[((a % 4) * 4) + int(a / 4)] = d[a];
            matrixRecord->matrix[3] = 0.0f;
            matrixRecord->matrix[7] = 0.0f;
            matrixRecord->matrix[11] = 0.0f;
            matrixRecord->matrix[15] = 1.0f;
            return matrixRecord;
        }
        
        flt::GeneralMatrix *asFLTGeneralMatrix(const sfa::Matrix &matrix)
        {
            flt::GeneralMatrix *matrixRecord = new flt::GeneralMatrix;
            const double *d = matrix.getData();
            matrixRecord->matrix.resize(16, 0.0f);
            for(size_t a = 0; a < 12; ++a)
                matrixRecord->matrix[((a % 4) * 4) + int(a / 4)] = d[a];
            matrixRecord->matrix[3] = 0.0f;
            matrixRecord->matrix[7] = 0.0f;
            matrixRecord->matrix[11] = 0.0f;
            matrixRecord->matrix[15] = 1.0f;
            return matrixRecord;
        }
        
        void buildScene(Scene *scene)
        {
            if(!scene)
                return;

            bool pop = false;

            flt::Record *container = NULL;
            if(scene->type() == "LOD")
            {
                LOD *lod = dynamic_cast<LOD *>(scene);
                flt::LevelOfDetail *lodRecord = new flt::LevelOfDetail;
                lodRecord->id = lod->name;
                if(lod->ranges.size() > 0)
                {
                    lodRecord->switchInDistance = lod->ranges[0].max;
                    lodRecord->switchOutDistance = lod->ranges[0].min;
                    lodRecord->transitionRange = (lodRecord->switchInDistance - lodRecord->switchOutDistance) / 10.0f;
                }
                container = lodRecord;
                header->nextLODNodeID = header->nextLODNodeID + 1;
            }

            if(!container && (scene->fltobject == "Object"))
            {
                flt::Object *groupRecord = new flt::Object;
                groupRecord->id = scene->name;
                container = groupRecord;
                header->nextObjectNodeID = header->nextObjectNodeID + 1;
            }

            if(!container)
            {
                flt::Group *groupRecord = new flt::Group;
                groupRecord->id = scene->name;
                container = groupRecord;
                header->nextGroupNodeID = header->nextGroupNodeID + 1;
            }

            if(container)
                records.push_back(container);

            if(!scene->children.empty() && (scene->name.size() > 7))
            {
                flt::LongID *longID = new flt::LongID;
                longID->id = scene->name;
                records.push_back(longID);
            }

            if(!(scene->matrix == sfa::Matrix()))
            {
                if(scene->children.empty())
                {
                    records.push_back(new flt::Group);
                    pop = true;
                }
                records.push_back(asFLTMatrix(scene->matrix));
                records.push_back(asFLTGeneralMatrix(scene->matrix));
                if(pop)
                    records.push_back(new flt::PushLevel);
            }

            if(scene->attributes.getKeys().size() > 0)
            {
                flt::Comment *commentRecord = new flt::Comment;
                std::vector<std::string> keys = scene->attributes.getKeys();
                for(size_t j = 0, jc = keys.size(); j < jc; ++j)
                {
                    if(scene->attributes.getAttributeAsBool(keys[j]))
                    {
                        if(commentRecord->textDescription.size() > 0)
                            commentRecord->textDescription += ";";
                        commentRecord->textDescription += keys[j];
                    }
                }
                records.push_back(commentRecord);
            }

            if(container)
                records.push_back(new flt::PushLevel);

            for(size_t i = 0; i < scene->children.size(); i++)
                buildScene(scene->children[i]);

            for(size_t i = 0, c = scene->lightPoints.size(); i < c; ++i)
            {
                LightPoint &lightPoint = scene->lightPoints[i];
                flt::LightPoint *lightPointRecord = new flt::LightPoint;

                header->nextLightPointNodeID = header->nextLightPointNodeID + 1;

                flt::VertexList *vertexListRecord = new flt::VertexList;

                OpenFlightVertex ofv;
                ofv.x = lightPoint.point.X();
                ofv.y = lightPoint.point.Y();
                ofv.z = lightPoint.point.Z();
                size_t index = vertexMapM[ofv];
                vertexListRecord->offsets.push_back(int(8 + (vertexVector.size() * vertexSize) + (index * (vertexSize - 8))));

                records.push_back(lightPointRecord);

                if(lightPoint.attributes.getVariantMap()->size() > 0)
                {
                    flt::Comment *commentRecord = new flt::Comment;
                    std::vector<std::string> keys = lightPoint.attributes.getKeys();
                    for(size_t j = 0, jc = keys.size(); j < jc; ++j)
                    {
                        if(lightPoint.attributes.getAttributeAsBool(keys[j]))
                        {
                            if(commentRecord->textDescription.size() > 0)
                                commentRecord->textDescription += ";";
                            commentRecord->textDescription += keys[j];
                        }
                    }
                    records.push_back(commentRecord);
                }

                records.push_back(new flt::PushLevel);
                records.push_back(vertexListRecord);
                records.push_back(new flt::PopLevel);
            }

            for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
            {
                Face &face = scene->faces[i];
                flt::Face *faceRecord = new flt::Face;

                if(face.id.size()==0)
                {
                    std::stringstream ss;
                    ss << "p" << i;
                    faceRecord->id = ss.str();
                }
                else
                {
                    faceRecord->id = face.id;
                }

                header->nextFaceNodeID = header->nextFaceNodeID + 1;
                if(face.attributes.getVariantMap()->size() > 0)
                {
                    bool terrain = false;
                    bool footprint = false;
                    bool hidden = false;
                    bool roofline = false;
                    int priority = 0;
                    face.attributes.getAttribute("terrain", terrain);
                    face.attributes.getAttribute("footprint", footprint);
                    face.attributes.getAttribute("hidden", hidden);
                    face.attributes.getAttribute("roofline", roofline);
                    face.attributes.getAttribute("priority", priority);
                    faceRecord->flags[0] = terrain;
                    faceRecord->flags[4] = footprint;
                    faceRecord->flags[5] = hidden;
                    faceRecord->flags[6] = roofline;
                    faceRecord->priority = priority;
                }
                faceRecord->surfaceMaterialCode = face.smc;
                faceRecord->featureID = face.featureID;
                faceRecord->primaryPackedColor = getUInt32FromColor(face.primaryColor);
                faceRecord->altPackedColor = getUInt32FromColor(face.alternateColor);
                faceRecord->altPackedColor = faceRecord->primaryPackedColor;        // TODO: we're generating a black alternate color, so this is overriding that
                faceRecord->primaryColorIndex = colorMap[face.primaryColor];
                faceRecord->altColorIndex = colorMap[face.alternateColor];
                faceRecord->transparency = (face.transparency>1.0)?65535:face.transparency*65535;
                faceRecord->lightMode = 3;

                if(face.materials.size() > 0)
                    faceRecord->materialIndex = materialMap[face.materials[0]];

                if(face.textures.size() > 0)
                    faceRecord->texturePatternIndex = textureMap[face.textures[0].GetTextureName()];

                if(faceRecord->texturePatternIndex == -1)
                {
                    faceRecord->flags[3] = true;    // use packed colors                    
                }
                if(face.drawBothSides)
                        faceRecord->drawType = 1;
                    else
                        faceRecord->drawType = 0;

                flt::VertexList *vertexListRecord = new flt::VertexList;
                for(size_t j = 0, jc = face.getNumVertices(); j < jc; ++j)
                {
                    if(face.textures.empty())
                    {
                        size_t index = vertexMapM[buildOpenFlightVertexFromFace(face, int(j))];
                        vertexListRecord->offsets.push_back(int(8 + (vertexVector.size() * vertexSize) + (index * (vertexSize - 8))));
                    }
                    else
                    {
                        size_t index = vertexMap[buildOpenFlightVertexFromFace(face, int(j))];
                        vertexListRecord->offsets.push_back(int(8 + (index * vertexSize)));
                    }
                }

                records.push_back(faceRecord);

                if(face.attributes.getVariantMap()->size() > 0)
                {
                    flt::Comment *commentRecord = new flt::Comment;
                    std::vector<std::string> keys = face.attributes.getKeys();
                    for(size_t j = 0, jc = keys.size(); j < jc; ++j)
                    {
                        if(face.attributes.getAttributeAsBool(keys[j]))
                        {
                            if(commentRecord->textDescription.size() > 0)
                                commentRecord->textDescription += ";";
                            commentRecord->textDescription += keys[j];
                        }
                    }
                    records.push_back(commentRecord);
                }

                records.push_back(new flt::PushLevel);
                records.push_back(vertexListRecord);
                records.push_back(new flt::PopLevel);
            }

            if(!scene->externalReferences.empty())
            {
                std::stringstream ss;
                ss << "g" << scene;
                flt::Group *extgroup = new flt::Group;
                extgroup->id = ss.str();
                records.push_back(extgroup);
                header->nextGroupNodeID = header->nextGroupNodeID + 1;
                records.push_back(new flt::PushLevel);
                for(size_t i = 0, c = scene->externalReferences.size(); i < c; ++i)
                {
                    ExternalReference &externalReference = scene->externalReferences[i];
                    flt::ExternalReference *externalReferenceRecord = new flt::ExternalReference;
                    externalReferenceRecord->path = externalReference.filename;
                    externalReferenceRecord->flags[0] = true;
                    externalReferenceRecord->flags[1] = true;
                    externalReferenceRecord->flags[2] = true;
                    externalReferenceRecord->flags[3] = true;
                    externalReferenceRecord->flags[4] = true;
                    externalReferenceRecord->flags[5] = true;
                    //externalReferenceRecord->flags[6] = true;
                    //externalReferenceRecord->flags[7] = true;
                    records.push_back(externalReferenceRecord);
                    sfa::Matrix m;
                    m.PushScale(externalReference.scale);
                    m.PushRotate(externalReference.attitude);
                    m.PushTranslate(externalReference.position);
                    if(!(m == sfa::Matrix()))
                    {
                        records.push_back(asFLTMatrix(m));
                        records.push_back(asFLTGeneralMatrix(m));
                    }
                }
                records.push_back(new flt::PopLevel);
            }

            if(container)
                records.push_back(new flt::PopLevel);
            if(pop)
                records.push_back(new flt::PopLevel);
        }

        void buildVertexSetFromScene(Scene *scene)
        {
            for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
            {
                for(size_t j = 0, jc = scene->faces[i].getNumVertices(); j < jc; ++j)
                {
                    if(scene->faces[i].textures.empty())
                        vertexSetM.insert(buildOpenFlightVertexFromFace(scene->faces[i], int(j)));
                    else
                        vertexSet.insert(buildOpenFlightVertexFromFace(scene->faces[i], int(j)));
                }
            }
            for(size_t i = 0, c = scene->children.size(); i < c; ++i)
                buildVertexSetFromScene(scene->children.at(i));
        }

        bool build(void)
        {
            if(!scene)
                return false;
            fltFile = flt::OpenFlight::create(filename, revision);
            if(!fltFile)
                return false;

            scene->setVertexNormals();

            header = new flt::Header;

            // header->projectionType
            // 0 = Flat Earth 
            // 1 = Trapezoidal 
            // 2 = Round Earth 
            // 3 = Lambert 
            // 4 = UTM 
            // 5 = Geodetic 
            // 6 = Geocentric

            if(scene->attributes.hasAttribute("origin_lat"))
            {
                header->originLatitude = scene->attributes.getAttributeAsDouble("origin_lat");
                header->originLongitude = scene->attributes.getAttributeAsDouble("origin_lon");

                header->southwestLatitude = scene->attributes.getAttributeAsDouble("sw_lat");
                header->southwestLongitude = scene->attributes.getAttributeAsDouble("sw_lon");

                header->northeastLatitude = scene->attributes.getAttributeAsDouble("ne_lat");
                header->northeastLongitude = scene->attributes.getAttributeAsDouble("ne_lon");
            }

            // the UTM origin is at the longitudinal centerpoint on the equator
            if(scene->attributes.hasAttribute("utm_zone"))
            {
                header->projectionType = 4;
                header->utmZone = scene->attributes.getAttributeAsInt("utm_zone", true, 0);
                header->originLatitude = 0;
                header->originLongitude = -180 + (abs(header->utmZone) * 6) - 3;
            }


            header->flags[0] = true;    // ??
            //header->flags[3] = true;    // ??
            //header->flags[6] = true;    // ??
            records.push_back(header);

            colorMap.clear();
            std::set<Color> tempColors;
            scene->getUniqueColors(tempColors);
            uniqueColors.clear();
            uniqueColors.insert(uniqueColors.end(), tempColors.begin(), tempColors.end());

            flt::ColorPalette *colorPalette = new flt::ColorPalette;
            for(size_t i = 0, c = uniqueColors.size(); i < c; ++i)
            {
                colorPalette->brightestRGB[i] = (ccl::int32_t)(getUInt32FromColor(uniqueColors[i]));
                colorMap[uniqueColors[i]] = (int(i) << 7) + 127;
            }
            records.push_back(colorPalette);

            std::set<Material> tempMaterials;
            scene->getUniqueMaterials(tempMaterials);
            materialMap.clear();
            uniqueMaterials.clear();
            uniqueMaterials.insert(uniqueMaterials.end(), tempMaterials.begin(), tempMaterials.end());
            for(size_t i = 0, c = uniqueMaterials.size(); i < c; ++i)
            {
                materialMap[uniqueMaterials[i]] = int(i);
                flt::MaterialPalette *materialPalette = new flt::MaterialPalette;
                materialPalette->index = int(i);
                materialPalette->ambientRed = uniqueMaterials[i].ambient.r;
                materialPalette->ambientGreen = uniqueMaterials[i].ambient.g;
                materialPalette->ambientBlue = uniqueMaterials[i].ambient.b;
                materialPalette->diffuseRed = uniqueMaterials[i].diffuse.r;
                materialPalette->diffuseGreen = uniqueMaterials[i].diffuse.g;
                materialPalette->diffuseBlue = uniqueMaterials[i].diffuse.b;
                materialPalette->specularRed = uniqueMaterials[i].specular.r;
                materialPalette->specularGreen = uniqueMaterials[i].specular.g;
                materialPalette->specularBlue = uniqueMaterials[i].specular.b;
                materialPalette->emissiveRed = uniqueMaterials[i].emission.r;
                materialPalette->emissiveGreen = uniqueMaterials[i].emission.g;
                materialPalette->emissiveBlue = uniqueMaterials[i].emission.b;
                materialPalette->alpha = uniqueMaterials[i].diffuse.a;
                materialPalette->shininess = uniqueMaterials[i].shine;
                records.push_back(materialPalette);
            }

            std::set<std::string> tempTextures;
            scene->getUniqueTextures(tempTextures);
            textureMap.clear();
            uniqueTextures.clear();
            uniqueTextures.insert(uniqueTextures.end(), tempTextures.begin(), tempTextures.end());
            for(size_t i = 0, c = uniqueTextures.size(); i < c; ++i)
            {
                textureMap[uniqueTextures[i]] = int(i);
                ccl::FileInfo fltFI(filename);
                std::string fltfilepath = fltFI.getDirName();
                ccl::FileInfo textureFI(uniqueTextures[i]);
                //std::string textureName = fltfilepath + "/" + textureFI.getBaseName();
                std::string textureName = textureFI.getBaseName();

                flt::TexturePalette *texturePalette = new flt::TexturePalette;
                texturePalette->textureIndex = int(i);
                texturePalette->fileName = textureName;
                records.push_back(texturePalette);
            }

            lightSourcePalette = new flt::LightSourcePalette;
            lightSourcePalette->index = 1;
            lightSourcePalette->ambientAlpha = 1.0f;
            lightSourcePalette->diffuseRed = 1.0f;
            lightSourcePalette->diffuseGreen = 1.0f;
            lightSourcePalette->diffuseBlue = 1.0f;
            lightSourcePalette->diffuseAlpha = 1.0f;
            lightSourcePalette->specularRed = 1.0f;
            lightSourcePalette->specularGreen = 1.0f;
            lightSourcePalette->specularBlue = 1.0f;
            lightSourcePalette->specularAlpha = 1.0f;
            records.push_back(lightSourcePalette);


            buildVertexSetFromScene(scene);
            flt::RecordList vertexList;
            vertexVector.insert(vertexVector.end(), vertexSet.begin(), vertexSet.end());
            for(size_t i = 0, c = vertexVector.size(); i < c; ++i)
            {
                vertexMap[vertexVector[i]] = i;
                flt::VertexWithColorNormalUV *vertexRecord = new flt::VertexWithColorNormalUV;
                vertexRecord->x = vertexVector[i].x;
                vertexRecord->y = vertexVector[i].y;
                vertexRecord->z = vertexVector[i].z;
                vertexRecord->i = vertexVector[i].i;
                vertexRecord->j = vertexVector[i].j;
                vertexRecord->k = vertexVector[i].k;
                vertexRecord->u = vertexVector[i].u;
                vertexRecord->v = vertexVector[i].v;
                vertexRecord->flags[2] = true;    // no color
                vertexRecord->flags[3] = true;    // packed color
                vertexRecord->packedColor = getUInt32FromColor(Color(1.0f, 1.0f, 1.0f));    // if flags[2] is false
                vertexRecord->colorIndex = -1;
                vertexList.push_back(vertexRecord);
            }
            vertexVectorM.insert(vertexVectorM.end(), vertexSetM.begin(), vertexSetM.end());
            for(size_t i = 0, c = vertexVectorM.size(); i < c; ++i)
            {
                vertexMapM[vertexVectorM[i]] = i;
                flt::VertexWithColorNormal *vertexRecord = new flt::VertexWithColorNormal;
                vertexRecord->x = vertexVectorM[i].x;
                vertexRecord->y = vertexVectorM[i].y;
                vertexRecord->z = vertexVectorM[i].z;
                vertexRecord->i = vertexVectorM[i].i;
                vertexRecord->j = vertexVectorM[i].j;
                vertexRecord->k = vertexVectorM[i].k;
                vertexRecord->flags[2] = true;    // no color
                vertexRecord->flags[3] = true;    // packed color
                vertexRecord->packedColor = getUInt32FromColor(Color(1.0f, 1.0f, 1.0f));    // if flags[2] is false
                vertexRecord->colorIndex = -1;
                vertexList.push_back(vertexRecord);
            }

            vertexPalette = new flt::VertexPalette;
            vertexPalette->vertexPaletteLength = int(8 + (vertexVector.size() * vertexSize) + (vertexVectorM.size() * (vertexSize - 8)));
            records.push_back(vertexPalette);
            records.insert(records.end(), vertexList.begin(), vertexList.end());

            records.push_back(new flt::PushLevel);
            buildScene(scene);
            records.push_back(new flt::PopLevel);

            if(!fltFile->addRecords(records))
                return false;
            flt::OpenFlight::destroy(fltFile);
            return true;
        }

    };

    bool buildOpenFlightFromScene(const std::string &filename, Scene *scene, int revision)
    {
        SceneOpenFlightBuilder fltBuilder(filename, scene, revision);
        return fltBuilder.build();
    }



}
