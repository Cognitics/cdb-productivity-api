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
#pragma once

#include <vector>
#include <string>
#include <iterator>
#include <iostream>


namespace cognitics
{
    class OpenFlightParser : public std::istream_iterator<int>
    {
    public:
        OpenFlightParser(std::istream& input) : is(input) { };
        OpenFlightParser(const OpenFlightParser&) = default;
        OpenFlightParser& operator=(const OpenFlightParser&) = default;

        struct Record
        {
            size_t Position { 0 };
            uint16_t OpCode { 0 };
            uint16_t Length { 0 };
            bool operator==(Record other) const { return (OpCode == other.OpCode) && (Position == other.Position) && (Length == other.Length); }
            bool operator!=(Record other) const { return !(*this == other); }
            Record() { }
            Record(std::istream& is)
            {
                if(!is.good())
                    return;
                Position = is.tellg();
                is.read((char*)&OpCode, 2);
                is.read((char*)&Length, 2);
                // TODO: local byte order detection and use a swap implementation
                OpCode = ((OpCode & 0x00FF) << 8) | ((OpCode & 0xFF00) >> 8);
                Length = ((Length & 0x00FF) << 8) | ((Length & 0xFF00) >> 8);
                if(is.good())
                    return;
                *this = Record();
            }
        };

        class iterator : public std::iterator<std::input_iterator_tag, Record, size_t, const Record*, Record>
        {
        public:
            explicit iterator(std::istream& is) : is(is), record(is) { }
            explicit iterator(std::istream& is, const Record& record) : is(is), record(record) { }
            iterator& operator++()
            {
                is.seekg(record.Position + record.Length, std::ios::beg);
                record = Record(is);
                return *this;
            }
            iterator operator++(int) { iterator ret = *this; ++(*this); return ret; }
            bool operator==(iterator other) const { return record == other.record; }
            bool operator!=(iterator other) const { return !(*this == other); }
            reference operator*() const { return record; }

        private:
            std::istream& is;
            Record record;
        };

        iterator begin() { return iterator(is); }
        iterator end() { return iterator(is, Record()); }

        constexpr static int HEADER = 1;
        constexpr static int GROUP = 2;
        constexpr static int LEVELOFDETAIL_OBSOLETE3 = 3;        // OBSOLETE (73)
        constexpr static int OBJECT = 4;
        constexpr static int FACE = 5;
        constexpr static int VERTEXWITHID_OBSOLETE6 = 6;        // OBSOLETE (68-71)
        constexpr static int SHORTVERTEXWITHOUTID_OBSOLETE7 = 7;        // OBSOLETE (68-71)
        constexpr static int VERTEXWITHCOLOR_OBSOLETE8 = 8;        // OBSOLETE (68-71)
        constexpr static int VERTEXWITHCOLORNORMAL_OBSOLETE9 = 9;        // OBSOLETE (68-71)
        constexpr static int PUSHLEVEL = 10;
        constexpr static int POPLEVEL = 11;
        constexpr static int TRANSLATE_OBSOLETE12 = 12;        // OBSOLETE (78)
        constexpr static int DEGREEOFFREEDOM_OBSOLETE13 = 13;        // OBSOLETE (14)
        constexpr static int DEGREEOFFREEDOM = 14;
        constexpr static int INSTANCEREFERENCE_OBSOLETE16 = 16;        // OBSOLETE (61)
        constexpr static int INSTANCEDEFINITION_OBSOLETE17 = 17;        // OBSOLETE (62)
        constexpr static int PUSHSUBFACE = 19;
        constexpr static int POPSUBFACE = 20;
        constexpr static int PUSHEXTENSION = 21;
        constexpr static int POPEXTENSION = 22;
        constexpr static int CONTINUATION = 23;
        constexpr static int COMMENT = 31;
        constexpr static int COLORPALETTE = 32;
        constexpr static int LONGID = 33;
        constexpr static int TRANSLATE_OBSOLETE40 = 40;        // OBSOLETE (78)
        constexpr static int ROTATEABOUTPOINT_OBSOLETE41 = 41;        // OBSOLETE (80)
        constexpr static int ROTATEABOUTEDGE_OBSOLETE42 = 42;        // OBSOLETE (76)
        constexpr static int SCALE_OBSOLETE43 = 43;        // OBSOLETE (79)
        constexpr static int TRANSLATE_OBSOLETE44 = 44;        // OBSOLETE (78)
        constexpr static int SCALENONUNIFORM_OBSOLETE45 = 45;        // OBSOLETE (79)
        constexpr static int ROTATEABOUTPOINT_OBSOLETE46 = 46;        // OBSOLETE (80)
        constexpr static int ROTATEABOUTSCALEPOINT_OBSOLETE47 = 47;        // OBSOLETE (81)
        constexpr static int PUT_OBSOLETE48 = 48;        // OBSOLETE (82)
        constexpr static int MATRIX = 49;
        constexpr static int VECTOR = 50;
        constexpr static int BOUNDINGBOX_OBSOLETE51 = 51;        // OBSOLETE (74)
        constexpr static int MULTITEXTURE = 52;
        constexpr static int UVLIST = 53;
        constexpr static int BINARYSEPARATINGPLANE = 55;
        constexpr static int REPLICATE = 60;
        constexpr static int INSTANCEREFERENCE = 61;
        constexpr static int INSTANCEDEFINITION = 62;
        constexpr static int EXTERNALREFERENCE = 63;
        constexpr static int TEXTUREPALETTE = 64;
        constexpr static int EYEPOINTPALETTE_OBSOLETE65 = 65;        // OBSOLETE (83)
        constexpr static int MATERIALPALETTE_OBSOLETE66 = 66;        // OBSOLETE (80)
        constexpr static int VERTEXPALETTE = 67;
        constexpr static int VERTEXWITHCOLOR = 68;
        constexpr static int VERTEXWITHCOLORNORMAL = 69;
        constexpr static int VERTEXWITHCOLORNORMALUV = 70;
        constexpr static int VERTEXWITHCOLORUV = 71;
        constexpr static int VERTEXLIST = 72;
        constexpr static int LEVELOFDETAIL = 73;
        constexpr static int BOUNDINGBOX = 74;
        constexpr static int ROTATEABOUTEDGE = 76;
        constexpr static int SCALE_OBSOLETE77 = 77;        // OBSOLETE (79)
        constexpr static int TRANSLATE = 78;
        constexpr static int SCALE = 79;
        constexpr static int ROTATEABOUTPOINT = 80;
        constexpr static int ROTATESCALETOPOINT = 81;
        constexpr static int PUT = 82;
        constexpr static int EYEPOINTTRACKPLANEPALETTE = 83;
        constexpr static int MESH = 84;
        constexpr static int LOCALVERTEXPOOL = 85;
        constexpr static int MESHPRIMITIVE = 86;
        constexpr static int ROADSEGMENT = 87;
        constexpr static int ROADZONE = 88;
        constexpr static int MORPHVERTEXLIST = 89;
        constexpr static int LINKAGEPALETTE = 90;
        constexpr static int SOUND = 91;
        constexpr static int ROADPATH = 92;
        constexpr static int SOUNDPALETTE = 93;
        constexpr static int GENERALMATRIX = 94;
        constexpr static int TEXT = 95;
        constexpr static int SWITCH = 96;
        constexpr static int LINESTYLEPALETTE = 97;
        constexpr static int CLIPREGION = 98;
        constexpr static int EXTENSION = 100;
        constexpr static int LIGHTSOURCE = 101;
        constexpr static int LIGHTSOURCEPALETTE = 102;
        constexpr static int BOUNDINGSPHERE = 105;
        constexpr static int BOUNDINGCYLINDER = 106;
        constexpr static int BOUNDINGCONVEXHULL = 107;
        constexpr static int BOUNDINGVOLUMECENTER = 108;
        constexpr static int BOUNDINGVOLUMEORIENTATION = 109;
        constexpr static int LIGHTPOINT = 111;
        constexpr static int TEXTUREMAPPINGPALETTE = 112;
        constexpr static int MATERIALPALETTE = 113;
        constexpr static int NAMETABLE = 114;
        constexpr static int CAT = 115;
        constexpr static int CATDATA = 116;
        constexpr static int BOUNDINGHISTOGRAM = 119;
        constexpr static int PUSHATTRIBUTE = 122;
        constexpr static int POPATTRIBUTE = 123;
        constexpr static int CURVE = 126;
        constexpr static int ROADCONSTRUCTION = 127;
        constexpr static int LIGHTPOINTAPPEARANCEPALETTE = 128;
        constexpr static int LIGHTPOINTANIMATIONPALETTE = 129;
        constexpr static int INDEXEDLIGHTPOINT = 130;
        constexpr static int LIGHTPOINTSYSTEM = 131;        // 15.8
        constexpr static int INDEXEDSTRING = 132;
        constexpr static int SHADERPALETTE = 133;
        constexpr static int EXTENDEDMATERIALHEADER = 135;        // 16.3
        constexpr static int EXTENDEDMATERIALAMBIENT = 136;        // 16.3
        constexpr static int EXTENDEDMATERIALDIFFUSE = 137;        // 16.3
        constexpr static int EXTENDEDMATERIALSPECULAR = 138;        // 16.3
        constexpr static int EXTENDEDMATERIALEMISSIVE = 139;        // 16.3
        constexpr static int EXTENDEDMATERIALALPHA = 140;        // 16.3
        constexpr static int EXTENDEDMATERIALLIGHTMAP = 141;        // 16.3
        constexpr static int EXTENDEDMATERIALNORMALMAP = 142;        // 16.3
        constexpr static int EXTENDEDMATERIALBUMPMAP = 143;        // 16.3
        constexpr static int EXTENDEDMATERIALSHADOWMAP = 145;        // 16.3
        constexpr static int EXTENDEDMATERIALREFLECTIONMAP = 147;        // 16.3
        constexpr static int EXTENSIONGUIDPALETTE = 148;        // 16.4
        constexpr static int EXTENSIONFIELDBOOLEAN = 149;        // 16.4
        constexpr static int EXTENSIONFIELDINTEGER = 150;        // 16.4
        constexpr static int EXTENSIONFIELDFLOAT = 151;        // 16.4
        constexpr static int EXTENSIONFIELDDOUBLE = 152;        // 16.4
        constexpr static int EXTENSIONFIELDSTRING = 153;        // 16.4
        constexpr static int EXTENSIONFIELDXML = 154;        // 16.4


    private:
        std::istream& is;
    };




}