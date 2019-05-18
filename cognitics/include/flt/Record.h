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
/*! \file flt/Record.h
\headerfile flt/Record.h
\brief Provides flt::Record
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include <ccl/ccl.h>
#include <ccl/BigEndian.h>
#include <ccl/LittleEndian.h>

namespace flt
{
    class Record;
    typedef std::vector<Record *> RecordList;

    class Record
    {
    public:
        static const int FLT_INVALID                            = 0;
        static const int FLT_HEADER                                = 1;
        static const int FLT_GROUP                                = 2;
        static const int FLT_LEVELOFDETAIL_OBSOLETE3            = 3;        // OBSOLETE (73)
        static const int FLT_OBJECT                                = 4;
        static const int FLT_FACE                                = 5;
        static const int FLT_VERTEXWITHID_OBSOLETE6                = 6;        // OBSOLETE (68-71)
        static const int FLT_SHORTVERTEXWITHOUTID_OBSOLETE7        = 7;        // OBSOLETE (68-71)
        static const int FLT_VERTEXWITHCOLOR_OBSOLETE8            = 8;        // OBSOLETE (68-71)
        static const int FLT_VERTEXWITHCOLORNORMAL_OBSOLETE9    = 9;        // OBSOLETE (68-71)
        static const int FLT_PUSHLEVEL                            = 10;
        static const int FLT_POPLEVEL                            = 11;
        static const int FLT_TRANSLATE_OBSOLETE12                = 12;        // OBSOLETE (78)
        static const int FLT_DEGREEOFFREEDOM_OBSOLETE13            = 13;        // OBSOLETE (14)
        static const int FLT_DEGREEOFFREEDOM                    = 14;
        static const int FLT_INSTANCEREFERENCE_OBSOLETE16        = 16;        // OBSOLETE (61)
        static const int FLT_INSTANCEDEFINITION_OBSOLETE17        = 17;        // OBSOLETE (62)
        static const int FLT_PUSHSUBFACE                        = 19;
        static const int FLT_POPSUBFACE                            = 20;
        static const int FLT_PUSHEXTENSION                        = 21;
        static const int FLT_POPEXTENSION                        = 22;
        static const int FLT_CONTINUATION                        = 23;
        static const int FLT_COMMENT                            = 31;
        static const int FLT_COLORPALETTE                        = 32;
        static const int FLT_LONGID                                = 33;
        static const int FLT_TRANSLATE_OBSOLETE40                = 40;        // OBSOLETE (78)
        static const int FLT_ROTATEABOUTPOINT_OBSOLETE41        = 41;        // OBSOLETE (80)
        static const int FLT_ROTATEABOUTEDGE_OBSOLETE42            = 42;        // OBSOLETE (76)
        static const int FLT_SCALE_OBSOLETE43                    = 43;        // OBSOLETE (79)
        static const int FLT_TRANSLATE_OBSOLETE44                = 44;        // OBSOLETE (78)
        static const int FLT_SCALENONUNIFORM_OBSOLETE45            = 45;        // OBSOLETE (79)
        static const int FLT_ROTATEABOUTPOINT_OBSOLETE46        = 46;        // OBSOLETE (80)
        static const int FLT_ROTATEABOUTSCALEPOINT_OBSOLETE47    = 47;        // OBSOLETE (81)
        static const int FLT_PUT_OBSOLETE48                        = 48;        // OBSOLETE (82)
        static const int FLT_MATRIX                                = 49;
        static const int FLT_VECTOR                                = 50;
        static const int FLT_BOUNDINGBOX_OBSOLETE51                = 51;        // OBSOLETE (74)
        static const int FLT_MULTITEXTURE                        = 52;
        static const int FLT_UVLIST                                = 53;
        static const int FLT_BINARYSEPARATINGPLANE                = 55;
        static const int FLT_REPLICATE                            = 60;
        static const int FLT_INSTANCEREFERENCE                    = 61;
        static const int FLT_INSTANCEDEFINITION                    = 62;
        static const int FLT_EXTERNALREFERENCE                    = 63;
        static const int FLT_TEXTUREPALETTE                        = 64;
        static const int FLT_EYEPOINTPALETTE_OBSOLETE65            = 65;        // OBSOLETE (83)
        static const int FLT_MATERIALPALETTE_OBSOLETE66            = 66;        // OBSOLETE (80)
        static const int FLT_VERTEXPALETTE                        = 67;
        static const int FLT_VERTEXWITHCOLOR                    = 68;
        static const int FLT_VERTEXWITHCOLORNORMAL                = 69;
        static const int FLT_VERTEXWITHCOLORNORMALUV            = 70;
        static const int FLT_VERTEXWITHCOLORUV                    = 71;
        static const int FLT_VERTEXLIST                            = 72;
        static const int FLT_LEVELOFDETAIL                        = 73;
        static const int FLT_BOUNDINGBOX                        = 74;
        static const int FLT_ROTATEABOUTEDGE                    = 76;
        static const int FLT_SCALE_OBSOLETE77                    = 77;        // OBSOLETE (79)
        static const int FLT_TRANSLATE                            = 78;
        static const int FLT_SCALE                                = 79;
        static const int FLT_ROTATEABOUTPOINT                    = 80;
        static const int FLT_ROTATESCALETOPOINT                    = 81;
        static const int FLT_PUT                                = 82;
        static const int FLT_EYEPOINTTRACKPLANEPALETTE            = 83;
        static const int FLT_MESH                                = 84;
        static const int FLT_LOCALVERTEXPOOL                    = 85;
        static const int FLT_MESHPRIMITIVE                        = 86;
        static const int FLT_ROADSEGMENT                        = 87;
        static const int FLT_ROADZONE                            = 88;
        static const int FLT_MORPHVERTEXLIST                    = 89;
        static const int FLT_LINKAGEPALETTE                        = 90;
        static const int FLT_SOUND                                = 91;
        static const int FLT_ROADPATH                            = 92;
        static const int FLT_SOUNDPALETTE                        = 93;
        static const int FLT_GENERALMATRIX                        = 94;
        static const int FLT_TEXT                                = 95;
        static const int FLT_SWITCH                                = 96;
        static const int FLT_LINESTYLEPALETTE                    = 97;
        static const int FLT_CLIPREGION                            = 98;
        static const int FLT_EXTENSION                            = 100;
        static const int FLT_LIGHTSOURCE                        = 101;
        static const int FLT_LIGHTSOURCEPALETTE                    = 102;
        static const int FLT_BOUNDINGSPHERE                        = 105;
        static const int FLT_BOUNDINGCYLINDER                    = 106;
        static const int FLT_BOUNDINGCONVEXHULL                    = 107;
        static const int FLT_BOUNDINGVOLUMECENTER                = 108;
        static const int FLT_BOUNDINGVOLUMEORIENTATION            = 109;
        static const int FLT_LIGHTPOINT                            = 111;
        static const int FLT_TEXTUREMAPPINGPALETTE                = 112;
        static const int FLT_MATERIALPALETTE                    = 113;
        static const int FLT_NAMETABLE                            = 114;
        static const int FLT_CAT                                = 115;
        static const int FLT_CATDATA                            = 116;
        static const int FLT_BOUNDINGHISTOGRAM                    = 119;
        static const int FLT_PUSHATTRIBUTE                        = 122;
        static const int FLT_POPATTRIBUTE                        = 123;
        static const int FLT_CURVE                                = 126;
        static const int FLT_ROADCONSTRUCTION                    = 127;
        static const int FLT_LIGHTPOINTAPPEARANCEPALETTE        = 128;
        static const int FLT_LIGHTPOINTANIMATIONPALETTE            = 129;
        static const int FLT_INDEXEDLIGHTPOINT                    = 130;
        static const int FLT_LIGHTPOINTSYSTEM                    = 131;        // 15.8
        static const int FLT_INDEXEDSTRING                        = 132;
        static const int FLT_SHADERPALETTE                        = 133;
        static const int FLT_EXTENDEDMATERIALHEADER                = 135;        // 16.3
        static const int FLT_EXTENDEDMATERIALAMBIENT            = 136;        // 16.3
        static const int FLT_EXTENDEDMATERIALDIFFUSE            = 137;        // 16.3
        static const int FLT_EXTENDEDMATERIALSPECULAR            = 138;        // 16.3
        static const int FLT_EXTENDEDMATERIALEMISSIVE            = 139;        // 16.3
        static const int FLT_EXTENDEDMATERIALALPHA                = 140;        // 16.3
        static const int FLT_EXTENDEDMATERIALLIGHTMAP            = 141;        // 16.3
        static const int FLT_EXTENDEDMATERIALNORMALMAP            = 142;        // 16.3
        static const int FLT_EXTENDEDMATERIALBUMPMAP            = 143;        // 16.3
        static const int FLT_EXTENDEDMATERIALSHADOWMAP            = 145;        // 16.3
        static const int FLT_EXTENDEDMATERIALREFLECTIONMAP        = 147;        // 16.3
        static const int FLT_EXTENSIONGUIDPALETTE                = 148;        // 16.4
        static const int FLT_EXTENSIONFIELDBOOLEAN                = 149;        // 16.4
        static const int FLT_EXTENSIONFIELDINTEGER                = 150;        // 16.4
        static const int FLT_EXTENSIONFIELDFLOAT                = 151;        // 16.4
        static const int FLT_EXTENSIONFIELDDOUBLE                = 152;        // 16.4
        static const int FLT_EXTENSIONFIELDSTRING                = 153;        // 16.4
        static const int FLT_EXTENSIONFIELDXML                    = 154;        // 16.4

        static const int CHILDTYPE_LEVEL                        = 0;
        static const int CHILDTYPE_SUBFACE                        = 1;
        static const int CHILDTYPE_EXTENSION                    = 2;
        static const int CHILDTYPE_ATTRIBUTE                    = 3;

        Record *parent;
        RecordList children;
        int childType;
        ccl::uint32_t position;

        virtual ~Record(void);
        Record(void);

        virtual int getRecordType(void) = 0;
        virtual std::string getRecordName(void) = 0;

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0) = 0;

    };

}

