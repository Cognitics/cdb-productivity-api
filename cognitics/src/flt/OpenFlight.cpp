/****************************************************************************
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

#include "flt/OpenFlight.h"
#include "flt/Unknown.h"
#include "flt/Header.h"
#include "flt/Group.h"
#include "flt/Object.h"
#include "flt/Face.h"
#include "flt/PushLevel.h"
#include "flt/PopLevel.h"
#include "flt/DegreeOfFreedom.h"
#include "flt/PushSubface.h"
#include "flt/PopSubface.h"
#include "flt/PushExtension.h"
#include "flt/PopExtension.h"
#include "flt/Comment.h"
#include "flt/ColorPalette.h"
#include "flt/LongID.h"
#include "flt/Matrix.h"
#include "flt/Vector.h"
#include "flt/MultiTexture.h"
#include "flt/UVList.h"
#include "flt/BinarySeparatingPlane.h"
#include "flt/Replicate.h"
#include "flt/InstanceReference.h"
#include "flt/InstanceDefinition.h"
#include "flt/ExternalReference.h"
#include "flt/TexturePalette.h"
#include "flt/VertexPalette.h"
#include "flt/VertexWithColor.h"
#include "flt/VertexWithColorNormal.h"
#include "flt/VertexWithColorNormalUV.h"
#include "flt/VertexWithColorUV.h"
#include "flt/VertexList.h"
#include "flt/LevelOfDetail.h"
#include "flt/BoundingBox.h"
#include "flt/RotateAboutEdge.h"
#include "flt/Translate.h"
#include "flt/Scale.h"
#include "flt/RotateAboutPoint.h"
#include "flt/RotateScaleToPoint.h"
#include "flt/Put.h"
#include "flt/EyepointTrackplanePalette.h"
#include "flt/Mesh.h"
#include "flt/LocalVertexPool.h"
#include "flt/MeshPrimitive.h"
#include "flt/RoadSegment.h"
#include "flt/RoadZone.h"
#include "flt/MorphVertexList.h"
#include "flt/LinkagePalette.h"
#include "flt/Sound.h"
#include "flt/RoadPath.h"
#include "flt/SoundPalette.h"
#include "flt/GeneralMatrix.h"
#include "flt/Text.h"
#include "flt/Switch.h"
#include "flt/LineStylePalette.h"
#include "flt/ClipRegion.h"
#include "flt/Extension.h"
#include "flt/LightSource.h"
#include "flt/LightSourcePalette.h"
#include "flt/BoundingSphere.h"
#include "flt/BoundingCylinder.h"
#include "flt/BoundingConvexHull.h"
#include "flt/BoundingVolumeCenter.h"
#include "flt/BoundingVolumeOrientation.h"
#include "flt/LightPoint.h"
#include "flt/TextureMappingPalette.h"
#include "flt/MaterialPalette.h"
#include "flt/NameTable.h"
#include "flt/CAT.h"
#include "flt/CATData.h"
#include "flt/BoundingHistogram.h"
#include "flt/PushAttribute.h"
#include "flt/PopAttribute.h"
#include "flt/Curve.h"
#include "flt/RoadConstruction.h"
#include "flt/LightPointAppearancePalette.h"
#include "flt/LightPointAnimationPalette.h"
#include "flt/IndexedLightPoint.h"
#include "flt/LightPointSystem.h"
#include "flt/IndexedString.h"
#include "flt/ShaderPalette.h"
#include "flt/ExtendedMaterialHeader.h"
#include "flt/ExtendedMaterialAmbient.h"
#include "flt/ExtendedMaterialDiffuse.h"
#include "flt/ExtendedMaterialSpecular.h"
#include "flt/ExtendedMaterialEmissive.h"
#include "flt/ExtendedMaterialAlpha.h"
#include "flt/ExtendedMaterialLightMap.h"
#include "flt/ExtendedMaterialNormalMap.h"
#include "flt/ExtendedMaterialBumpMap.h"
#include "flt/ExtendedMaterialShadowMap.h"
#include "flt/ExtendedMaterialReflectionMap.h"
#include "flt/ExtensionGUIDPalette.h"
#include "flt/ExtensionFieldBoolean.h"
#include "flt/ExtensionFieldInteger.h"
#include "flt/ExtensionFieldFloat.h"
#include "flt/ExtensionFieldDouble.h"
#include "flt/ExtensionFieldString.h"
#include "flt/ExtensionFieldXML.h"

#include <ccl/binary.h>

namespace flt
{
    Record *createRecordForOpcode(int opcode)
    {
        switch(opcode)
        {
            case Record::FLT_HEADER:                            return new Header;
            case Record::FLT_GROUP:                                return new Group;
            case Record::FLT_OBJECT:                            return new Object;
            case Record::FLT_FACE:                                return new Face;
            case Record::FLT_PUSHLEVEL:                            return new PushLevel;
            case Record::FLT_POPLEVEL:                            return new PopLevel;
            case Record::FLT_DEGREEOFFREEDOM:                    return new DegreeOfFreedom;
            case Record::FLT_PUSHSUBFACE:                        return new PushSubface;
            case Record::FLT_POPSUBFACE:                        return new PopSubface;
            case Record::FLT_PUSHEXTENSION:                        return new PushExtension;
            case Record::FLT_POPEXTENSION:                        return new PopExtension;
            case Record::FLT_COMMENT:                            return new Comment;
            case Record::FLT_COLORPALETTE:                        return new ColorPalette;
            case Record::FLT_LONGID:                            return new LongID;
            case Record::FLT_MATRIX:                            return new Matrix;
            case Record::FLT_VECTOR:                            return new Vector;
            case Record::FLT_MULTITEXTURE:                        return new MultiTexture;
            case Record::FLT_UVLIST:                            return new UVList;
            case Record::FLT_BINARYSEPARATINGPLANE:                return new BinarySeparatingPlane;
            case Record::FLT_REPLICATE:                            return new Replicate;
            case Record::FLT_INSTANCEREFERENCE:                    return new InstanceReference;
            case Record::FLT_INSTANCEDEFINITION:                return new InstanceDefinition;
            case Record::FLT_EXTERNALREFERENCE:                    return new ExternalReference;
            case Record::FLT_TEXTUREPALETTE:                    return new TexturePalette;
            case Record::FLT_VERTEXPALETTE:                        return new VertexPalette;
            case Record::FLT_VERTEXWITHCOLOR:                    return new VertexWithColor;
            case Record::FLT_VERTEXWITHCOLORNORMAL:                return new VertexWithColorNormal;
            case Record::FLT_VERTEXWITHCOLORNORMALUV:            return new VertexWithColorNormalUV;
            case Record::FLT_VERTEXWITHCOLORUV:                    return new VertexWithColorUV;
            case Record::FLT_VERTEXLIST:                        return new VertexList;
            case Record::FLT_LEVELOFDETAIL:                        return new LevelOfDetail;
            case Record::FLT_BOUNDINGBOX:                        return new BoundingBox;
            case Record::FLT_ROTATEABOUTEDGE:                    return new RotateAboutEdge;
            case Record::FLT_TRANSLATE:                            return new Translate;
            case Record::FLT_SCALE:                                return new Scale;
            case Record::FLT_ROTATEABOUTPOINT:                    return new RotateAboutPoint;
            case Record::FLT_ROTATESCALETOPOINT:                return new RotateScaleToPoint;
            case Record::FLT_PUT:                                return new Put;
            case Record::FLT_EYEPOINTTRACKPLANEPALETTE:            return new EyepointTrackplanePalette;
            case Record::FLT_MESH:                                return new Mesh;
            case Record::FLT_LOCALVERTEXPOOL:                    return new LocalVertexPool;
            case Record::FLT_MESHPRIMITIVE:                        return new MeshPrimitive;
            case Record::FLT_ROADSEGMENT:                        return new RoadSegment;
            case Record::FLT_ROADZONE:                            return new RoadZone;
            case Record::FLT_MORPHVERTEXLIST:                    return new MorphVertexList;
            case Record::FLT_LINKAGEPALETTE:                    return new LinkagePalette;
            case Record::FLT_SOUND:                                return new Sound;
            case Record::FLT_ROADPATH:                            return new RoadPath;
            case Record::FLT_SOUNDPALETTE:                        return new SoundPalette;
            case Record::FLT_GENERALMATRIX:                        return new GeneralMatrix;
            case Record::FLT_TEXT:                                return new Text;
            case Record::FLT_SWITCH:                            return new Switch;
            case Record::FLT_LINESTYLEPALETTE:                    return new LineStylePalette;
            case Record::FLT_CLIPREGION:                        return new ClipRegion;
            case Record::FLT_EXTENSION:                            return new Extension;
            case Record::FLT_LIGHTSOURCE:                        return new LightSource;
            case Record::FLT_LIGHTSOURCEPALETTE:                return new LightSourcePalette;
            case Record::FLT_BOUNDINGSPHERE:                    return new BoundingSphere;
            case Record::FLT_BOUNDINGCYLINDER:                    return new BoundingCylinder;
            case Record::FLT_BOUNDINGCONVEXHULL:                return new BoundingConvexHull;
            case Record::FLT_BOUNDINGVOLUMECENTER:                return new BoundingVolumeCenter;
            case Record::FLT_BOUNDINGVOLUMEORIENTATION:            return new BoundingVolumeOrientation;
            case Record::FLT_LIGHTPOINT:                        return new LightPoint;
            case Record::FLT_TEXTUREMAPPINGPALETTE:                return new TextureMappingPalette;
            case Record::FLT_MATERIALPALETTE:                    return new MaterialPalette;
            case Record::FLT_NAMETABLE:                            return new NameTable;
            case Record::FLT_CAT:                                return new CAT;
            case Record::FLT_CATDATA:                            return new CATData;
            case Record::FLT_BOUNDINGHISTOGRAM:                    return new BoundingHistogram;
            case Record::FLT_PUSHATTRIBUTE:                        return new PushAttribute;
            case Record::FLT_POPATTRIBUTE:                        return new PopAttribute;
            case Record::FLT_CURVE:                                return new Curve;
            case Record::FLT_ROADCONSTRUCTION:                    return new RoadConstruction;
            case Record::FLT_LIGHTPOINTAPPEARANCEPALETTE:        return new LightPointAppearancePalette;
            case Record::FLT_LIGHTPOINTANIMATIONPALETTE:        return new LightPointAnimationPalette;
            case Record::FLT_INDEXEDLIGHTPOINT:                    return new IndexedLightPoint;
            case Record::FLT_LIGHTPOINTSYSTEM:                    return new LightPointSystem;
            case Record::FLT_INDEXEDSTRING:                        return new IndexedString;
            case Record::FLT_SHADERPALETTE:                        return new ShaderPalette;
            case Record::FLT_EXTENDEDMATERIALHEADER:            return new ExtendedMaterialHeader;
            case Record::FLT_EXTENDEDMATERIALAMBIENT:            return new ExtendedMaterialAmbient;
            case Record::FLT_EXTENDEDMATERIALDIFFUSE:            return new ExtendedMaterialDiffuse;
            case Record::FLT_EXTENDEDMATERIALSPECULAR:            return new ExtendedMaterialSpecular;
            case Record::FLT_EXTENDEDMATERIALEMISSIVE:            return new ExtendedMaterialEmissive;
            case Record::FLT_EXTENDEDMATERIALALPHA:                return new ExtendedMaterialAlpha;
            case Record::FLT_EXTENDEDMATERIALLIGHTMAP:            return new ExtendedMaterialLightMap;
            case Record::FLT_EXTENDEDMATERIALNORMALMAP:            return new ExtendedMaterialNormalMap;
            case Record::FLT_EXTENDEDMATERIALBUMPMAP:            return new ExtendedMaterialBumpMap;
            case Record::FLT_EXTENDEDMATERIALSHADOWMAP:            return new ExtendedMaterialShadowMap;
            case Record::FLT_EXTENDEDMATERIALREFLECTIONMAP:        return new ExtendedMaterialReflectionMap;
            case Record::FLT_EXTENSIONGUIDPALETTE:                return new ExtensionGUIDPalette;
            case Record::FLT_EXTENSIONFIELDBOOLEAN:                return new ExtensionFieldBoolean;
            case Record::FLT_EXTENSIONFIELDINTEGER:                return new ExtensionFieldInteger;
            case Record::FLT_EXTENSIONFIELDFLOAT:                return new ExtensionFieldFloat;
            case Record::FLT_EXTENSIONFIELDDOUBLE:                return new ExtensionFieldDouble;
            case Record::FLT_EXTENSIONFIELDSTRING:                return new ExtensionFieldString;
            case Record::FLT_EXTENSIONFIELDXML:                    return new ExtensionFieldXML;
        }
        return new Unknown(opcode);
    }

    OpenFlight::OpenFlight(void) : revision(0), nextOpcode(0)
    {
        log.init("OpenFlight", this);
    }

    OpenFlight *OpenFlight::create(const std::string &filename, int revision)
    {
        if(revision == 0)
            revision = getSupportedRevisions().back();
        OpenFlight *flt = new OpenFlight;
        if(!supportsRevision(revision))
            flt->log << ccl::LWARNING << "create(" << filename << ", " << revision << "): revision not supported, attempting to continue" << flt->log.endl; 
        flt->revision = revision;
        flt->outFile.open(filename.c_str(), std::ofstream::binary | std::ofstream::out | std::ofstream::trunc);
        if(!flt->outFile.good())
        {
            flt->log << ccl::LERR << "create(" << filename << ", " << revision << "): error creating file" << flt->log.endl; 
            delete flt;
            return NULL;
        }
        flt->bindStream.setStream(flt->outFile);
        return flt;
    }

    OpenFlight *OpenFlight::open(const std::string &filename)
    {
        OpenFlight *flt = new OpenFlight;
        flt->inFile.open(filename.c_str(), std::ifstream::binary | std::ifstream::in);
        if(!flt->inFile.good())
        {
            flt->log << ccl::LERR << "open(" << filename << "): error opening file" << flt->log.endl; 
            delete flt;
            return NULL;
        }
        flt->bindStream.setStream(flt->inFile);

        // preread the header for validation
        flt->bindStream.bind(flt->nextOpcode);
        if(flt->nextOpcode != Record::FLT_HEADER)
        {
            flt->log << ccl::LERR << "open(" << filename << "): invalid OpenFlight file (header not found)" << flt->log.endl; 
            delete flt;
            return NULL;
        }
        Header *header = dynamic_cast<Header *>(flt->getNextRecord());
        if(!header)
        {
            flt->log << ccl::LERR << "open(" << filename << "): invalid OpenFlight file (header not found)" << flt->log.endl; 
            delete flt;
            return NULL;
        }
        flt->revision = header->formatRevisionLevel;
        if(!supportsRevision(flt->revision))
            flt->log << ccl::LWARNING << "open(" << filename << "): revision " << flt->revision << " not supported, attempting to continue" << flt->log.endl; 

        // start over
        flt->inFile.seekg(0);
        flt->bindStream.bind(flt->nextOpcode);

        return flt;
    }

    void OpenFlight::destroy(OpenFlight *ptr)
    {
        delete ptr;
    }

    std::vector<int> OpenFlight::getSupportedRevisions(void)
    {
        std::vector<int> revisions;
        revisions.push_back(1570);
        revisions.push_back(1580);
        revisions.push_back(1590);
        revisions.push_back(1600);
        revisions.push_back(1610);
        revisions.push_back(1620);
        revisions.push_back(1630);
        revisions.push_back(1640);
        return revisions;
    }

    bool OpenFlight::supportsRevision(int revision)
    {
        std::vector<int> revisions = getSupportedRevisions();
        for(int i = 0, c = int(revisions.size()); i < c; ++i)
        {
            if(revision == revisions.at(i))
                return true;
        }
        return false;
    }

    std::vector<std::string> OpenFlight::getTexturePaletteFilenames(const std::string &filename)
    {
        std::vector<std::string> result;
        OpenFlight *file = open(filename);
        if(!file)
            return result;
        while(Record *record = file->getNextRecord())
        {
            if(record->getRecordType() == Record::FLT_TEXTUREPALETTE)
            {
                TexturePalette *tp = dynamic_cast<TexturePalette *>(record);
                if(!tp->fileName.empty())
                    result.push_back(tp->fileName);
            }
            delete record;
        }
        delete file;
        return result;
    }

    bool OpenFlight::getBoundingBox(const std::string &filename, double &xmin, double &xmax ,double &ymin, double &ymax, double &zmin, double &zmax)
    {
        flt::OpenFlight *file = flt::OpenFlight::open(filename);
        if(!file)
        {
            //log << ccl::LERR << "Failed to load " + filename + ".";
            return false;
        }

        while(Record *record = file->getNextRecord())
        {
            double x = 0;
            double y = 0;
            double z = 0;
            if(record->getRecordType() == Record::FLT_VERTEXWITHCOLOR)
            {
                VertexWithColor *vertex = dynamic_cast<VertexWithColor *>(record);
                if(vertex)
                {
                    x = vertex->x;
                    y = vertex->y;
                    z = vertex->z;                    
                }
            }
            else if(record->getRecordType() == Record::FLT_VERTEXWITHCOLORNORMAL)
            {
                VertexWithColorNormal *vertex = dynamic_cast<VertexWithColorNormal *>(record);
                if(vertex)
                {
                    x = vertex->x;
                    y = vertex->y;
                    z = vertex->z;                    
                }
            }
            else if(record->getRecordType() == Record::FLT_VERTEXWITHCOLORNORMALUV)
            {
                VertexWithColorNormalUV *vertex = dynamic_cast<VertexWithColorNormalUV *>(record);
                if(vertex)
                {
                    x = vertex->x;
                    y = vertex->y;
                    z = vertex->z;                    
                }
            }
            else if(record->getRecordType() == Record::FLT_VERTEXWITHCOLORUV)
            {
                VertexWithColorUV *vertex = dynamic_cast<VertexWithColorUV *>(record);
                if(vertex)
                {
                    x = vertex->x;
                    y = vertex->y;
                    z = vertex->z;                    
                }
            }
            xmin = std::min<double>(x,xmin);
            ymin = std::min<double>(y,ymin);
            zmin = std::min<double>(z,zmin);

            xmax = std::max<double>(x,xmax);
            ymax = std::max<double>(y,ymax);
            zmax = std::max<double>(z,zmax);
        
            delete record;
        }
        return true;
    }

    OpenFlight::~OpenFlight(void)
    {
    }

    ccl::uint16_t OpenFlight::getNextOpcode(void)
    {
        return nextOpcode;
    }

    Record *OpenFlight::getNextRecord(void)
    {
        if(!inFile.is_open())
            throw std::runtime_error("getNextRecord() called with invalid input file");
        if(inFile.eof())
            return NULL;
        int position = bindStream.pos() - 2;
        if(!inFile.good())
        {
            log << ccl::LERR << "getNextRecord(): file error at file position " << position << log.endl;
            return NULL;
        }
        ccl::uint16_t opcode = nextOpcode;
        ccl::BigEndian<ccl::uint16_t> length;
        bindStream.bind(length);
        ccl::binary data;
        bindStream.bind(data, length - 4);
        if(!inFile.good())
        {
            log << ccl::LERR << "getNextRecord(): file error at file position " << position << log.endl;
            return NULL;
        }
        bindStream.bind(nextOpcode);
        while(nextOpcode == Record::FLT_CONTINUATION)
        {
            ccl::BigEndian<ccl::uint16_t> partLength;
            bindStream.bind(partLength);
            ccl::binary partData;
            bindStream.bind(partData, partLength - 4);
            if(!inFile.good())
            {
                log << ccl::LERR << "getNextRecord(): file error at file position " << position << log.endl;
                return NULL;
            }
            data.append(partData);
            bindStream.bind(nextOpcode);
        }
        Record *record = createRecordForOpcode(opcode);
        if(!record)
        {
            log << ccl::LERR << "getNextRecord(): error creating record (opcode " << ccl::uint16_t(opcode) << ") at file position " << position << log.endl;
            return NULL;
        }
        record->position = position;
        ccl::binarystringstream bss(data, ccl::binarystringstream::binary | ccl::binarystringstream::in);
        ccl::BindStream bs((std::istream &)bss);
        record->bind(bs, length - 4, revision);
        return record;
    }

    RecordList OpenFlight::getRecords(void)
    {
        if(!inFile.is_open())
            throw std::runtime_error("getRecords() called with invalid input file");
        RecordList result;
        Record *prev = NULL;
        std::vector<Record *> stack;
        while(Record *record = getNextRecord())
        {
            record->parent = stack.empty() ? NULL : stack.back();
            if(record->parent)
                record->parent->children.push_back(record);
            else
                result.push_back(record);
            switch(record->getRecordType())
            {
                case Record::FLT_PUSHLEVEL:
                case Record::FLT_PUSHSUBFACE:
                case Record::FLT_PUSHEXTENSION:
                case Record::FLT_PUSHATTRIBUTE:
                    stack.push_back(record);
                    break;
                case Record::FLT_POPLEVEL:
                case Record::FLT_POPSUBFACE:
                case Record::FLT_POPEXTENSION:
                case Record::FLT_POPATTRIBUTE:
                    stack.pop_back();
                    break;
            }
            prev = record;
        }
        return result;
    }

    bool OpenFlight::addRecord(Record *record)
    {
        if(!outFile.is_open())
            throw std::runtime_error("addRecord() called with invalid output file");
        if(!record)
            throw std::runtime_error("addRecord() called with a NULL record");
        if((nextOpcode == 0) && (record->getRecordType() != Record::FLT_HEADER))    // first record added must be a header
            throw std::runtime_error("addRecord() called without a header record added");
        int position = bindStream.pos();
        if(record->getRecordType() == Record::FLT_HEADER)
        {
            Header *header = dynamic_cast<Header *>(record);
            header->formatRevisionLevel = revision;
        }
        ccl::binarystringstream bss(ccl::binarystringstream::binary | ccl::binarystringstream::out | ccl::binarystringstream::trunc);
        ccl::BindStream bs((std::ostream &)bss);
        record->bind(bs, 0, revision);
        ccl::binary data(bss.str());
        for(int i = 0;; ++i)
        {
            ccl::BigEndian<ccl::uint16_t> opcode = (i == 0) ? record->getRecordType() : Record::FLT_CONTINUATION;
            ccl::BigEndian<ccl::uint16_t> length = 65532;
            if(data.size() < 65532 - 4)
            {
                size_t a = data.size() % 4;
                if(a > 0)
                    data.append(4 - a, 0);
                length = ccl::uint16_t(data.size()) + 4;
                bindStream.bind(opcode);
                bindStream.bind(length);
                bindStream.bind(data, ccl::uint16_t(data.size()));
                if(!outFile.good())
                {
                    log << ccl::LERR << "addRecord(): error adding record (opcode " << ccl::uint16_t(opcode) << ") part " << (i+1) << " at file position " << position << log.endl;
                    return false;
                }
                data.clear();
                break;
            }

            bindStream.bind(opcode);
            bindStream.bind(length);
            bindStream.bind(data, 65532 - 4);
            data = data.substr(65532 - 4);
            if(!outFile.good())
            {
                log << ccl::LERR << "addRecord(): error adding record (opcode " << ccl::uint16_t(opcode) << ") part " << (i+1) << " at file position " << position << log.endl;
                return false;
            }
        }
        nextOpcode = record->getRecordType();
        return true;
    }

    bool OpenFlight::addRecords(const RecordList &records)
    {
        if(!outFile.is_open())
            throw std::runtime_error("addRecord() called with invalid output file");
        for(int i = 0, c = int(records.size()); i < c; ++i)
        {
            Record *record = records.at(i);
            if(!addRecord(record) || !addRecords(record->children))
                return false;
        }
        return true;
    }

}
