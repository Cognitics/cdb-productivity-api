/****************************************************************************
Copyright 2017 Cognitics, Inc.
****************************************************************************/

//#pragma optimize( "", off )

#include "ip/attr.h"
#include <fstream>

namespace ip {

    attrFile::attrFile(void)
    {
        u_size            = 0;
        v_size            = 0;
        u_scale            = 0;
        v_scale            = 0;
        up_x            = 0;
        up_y            = 0;
        fileFormat        = FILE_FORMAT_NONE_USED;
        minFilterMode    = MIN_FILTER_NONE;
        magFilterMode    = MAG_FILTER_POINT;
        wrapMode        = WRAP_REPEAT;
        wrapMode_u        = WRAP_REPEAT;
        wrapMode_v        = WRAP_REPEAT;
        modifyFlag        = 0;
        x_pivot            = 0;
        y_pivot            = 0;
        texEnvMode        = TEXENV_MODULATE;
        intensityAsAlpha = 0;
        float_u_size    = 0;
        float_v_size    = 0;
        originCode        = 0;
        kernelVersion    = 0;
        internalFormat    = 0;
        externalFormat    = 0;
        useMipmap        = 0;
        for (int i = 0; i < 8; i++) mipmap[i] = 0.f;
        useLodScale        = 0;
        for (int i = 0; i < 7; i+=2) 
        {
            lodScale[i] = 0.f;
            lodScale[i+1] = 1.f;
        }
        clamp            = 0;
        magFilterAlpha    = 2;
        magFilterColor    = 2;
        lambertMeridian    = 0;
        lambertUpperLat    = 0;
        lambertLowerLat    = 0;
        useDetail        = 0;
        detail_j        = 0;
        detail_k        = 0;
        detail_m        = 0;
        detail_n        = 0;
        detail_s        = 0;
        useTile            = 0;
        txTile_ll_u        = 0;
        txTile_ll_v        = 0;
        txTile_ur_u        = 0;
        txTile_ur_v        = 0;
        projection        = PROJECTION_UNDEFINED;
        earthModel        = DATUM_WGS84;
        utmZone            = 0;
        imageOrigin        = 0;
        geoUnits        = 0;
        hemisphere        = 1;
        comments        = "";
        attrVersion        = 0;
        controlPoints    = 0;
        numSubTextures    = 0;
    }

    bool attrFile::Write(const std::string& filename)
    {
        std::string ext = filename.substr(filename.find_last_of(".") + 1);
        if (ext.compare("attr") != 0) return false;

        std::ofstream out;
        out.open(filename.c_str(), std::ios::out | std::ios::binary );

        if (!out.is_open()) return false;

        out << u_size;
        out << v_size;
        out << u_scale;
        out << v_scale;
        out << up_x;
        out << up_y;
        out << fileFormat;
        out << minFilterMode;
        out << magFilterMode;
        out << wrapMode;
        out << wrapMode_u;
        out << wrapMode_v;
        out << modifyFlag;
        out << x_pivot;
        out << y_pivot;
        out << texEnvMode;
        out << intensityAsAlpha;
        for (int i = 0; i < 32; i++) out << '\0';
        for (int i = 0; i < 4; i++) out << '\0';
        out << float_u_size;
        out << float_v_size;
        out << originCode;
        out << kernelVersion;
        out << internalFormat;
        out << externalFormat;
        out << useMipmap;
        for (int i = 0; i < 8; i++) out << mipmap[i];
        out << useLodScale;
        for (int i = 0; i < 16; i++) out << lodScale[i];
        out << clamp;
        out << magFilterAlpha;
        out << magFilterColor;
        for (int i = 0; i < 36; i++) out << '\0';
        out << lambertMeridian;
        out << lambertUpperLat;
        out << lambertLowerLat;
        for (int i = 0; i < 28; i++) out << '\0';
        out << useDetail;
        out << detail_j;
        out << detail_k;
        out << detail_m;
        out << detail_n;
        out << detail_s;
        out << useTile;
        out << txTile_ll_u;
        out << txTile_ll_v;
        out << txTile_ur_u;
        out << txTile_ur_v;
        out << projection;
        out << earthModel;
        for (int i = 0; i < 4; i++) out << '\0';
        out << utmZone;
        out << imageOrigin;
        out << geoUnits;
        for (int i = 0; i < 8; i++) out << '\0';
        out << hemisphere;
        for (int i = 0; i < 604; i++) out << '\0';

        if (comments.size() > 511)
        {
            for (unsigned int i = 0; i < 511; i++) out << comments[i];
            out << '\0';
        }
        else
        {
            out << comments;
            for (unsigned int i = 0; i < (512 - comments.size()); i++) out << '\0';
        }

        for (int i = 0; i < 56; i++) out << '\0';
        out << attrVersion;
        out << controlPoints;
        out << numSubTextures;

        out.close();

        return true;
    }

    bool attrFile::Read(const std::string& filename)
    {
        std::string ext = filename.substr(filename.find_last_of(".") + 1);
        if (ext.compare("attr") != 0) return false;

        std::ifstream in;
        in.open(filename.c_str(), std::ios::in | std::ios::binary );
        
        if (!in.is_open()) return false;

        in >> u_size;
        in >> v_size;
        in >> u_scale;
        in >> v_scale;
        in >> up_x;
        in >> up_y;
        in >> fileFormat;
        in >> minFilterMode;
        in >> magFilterMode;
        in >> wrapMode;
        in >> wrapMode_u;
        in >> wrapMode_v;
        in >> modifyFlag;
        in >> x_pivot;
        in >> y_pivot;
        in >> texEnvMode;
        in >> intensityAsAlpha;
        in.ignore(32);
        //Not sure what is supposed to be here, but this is what OSG does, and it is the only way the rest of the data makes sense.
        in.ignore(4);
        in >> float_u_size;
        in >> float_v_size;
        in >> originCode;
        in >> kernelVersion;
        in >> internalFormat;
        in >> externalFormat;
        in >> useMipmap;
        for (int i = 0; i < 8; i++) in >> mipmap[i];
        in >> useLodScale;
        for (int i = 0; i < 16; i++) in >> lodScale[i];
        in >> clamp;
        in >> magFilterAlpha;
        in >> magFilterColor;
        in.ignore(36);
        in >> lambertMeridian;
        in >> lambertUpperLat;
        in >> lambertLowerLat;
        in.ignore(28);
        in >> useDetail;
        in >> detail_j;
        in >> detail_k;
        in >> detail_m;
        in >> detail_n;
        in >> detail_s;
        in >> useTile;
        in >> txTile_ll_u;
        in >> txTile_ll_v;
        in >> txTile_ur_u;
        in >> txTile_ur_v;
        in >> projection;
        in >> earthModel;
        in.ignore(4);
        in >> utmZone;
        in >> imageOrigin;
        in >> geoUnits;
        in.ignore(8);
        in >> hemisphere;
        in.ignore(604);

        char buf[512];
        in.read(buf,512);
        buf[511] = '\0';
        comments = buf;

        in.ignore(56);
        in >> attrVersion;
        in >> controlPoints;
        in >> numSubTextures;

        in.close();

        if (float_u_size == 0 && u_scale == 0)
        {
            u_scale = 1;
            float_u_size = 1;
        }
        else if (float_u_size == 0)
            float_u_size = int(u_scale);
        else
            u_scale = int(float_u_size);

        if (float_v_size == 0 && v_scale == 0)
        {
            v_scale = 1;
            float_v_size = 1;
        }
        else if (float_v_size == 0)
            float_v_size = int(v_scale);
        else
            v_scale = int(float_v_size);

        return true;
    }

    void attrFile::Dump(std::ostream &out)
    {
        out << "u_size: " << ccl::int32_t(u_size) << std::endl;
        out << "v_size: " << ccl::int32_t(v_size) << std::endl;
        out << "u_scale: " << ccl::int32_t(u_scale) << std::endl;
        out << "v_scale: " << ccl::int32_t(v_scale) << std::endl;
        out << "up_x: " << ccl::int32_t(up_x) << std::endl;
        out << "up_y: " << ccl::int32_t(up_y) << std::endl;
        out << "fileFormat: " << ccl::int32_t(fileFormat) << std::endl;
        out << "minFilterMode: " << ccl::int32_t(minFilterMode) << std::endl;
        out << "magFilterMode: " << ccl::int32_t(magFilterMode) << std::endl;
        out << "wrapMode: " << ccl::int32_t(wrapMode) << std::endl;
        out << "wrapMode_u: " << ccl::int32_t(wrapMode_u) << std::endl;
        out << "wrapMode_v: " << ccl::int32_t(wrapMode_v) << std::endl;
        out << "modifyFlag: " << ccl::int32_t(modifyFlag) << std::endl;
        out << "x_pivot: " << ccl::int32_t(x_pivot) << std::endl;
        out << "y_pivot: " << ccl::int32_t(y_pivot) << std::endl;
        out << "texEnvMode: " << ccl::int32_t(texEnvMode) << std::endl;
        out << "intensityAsAlpha: " << ccl::int32_t(intensityAsAlpha) << std::endl;
        //int32            reserved_0[8];            //    spare
        out << "float_u_size: " << double(float_u_size) << std::endl;
        out << "float_v_size: " << double(float_v_size) << std::endl;
        out << "originCode: " << ccl::int32_t(originCode) << std::endl;
        out << "kernelVersion: " << ccl::int32_t(kernelVersion) << std::endl;
        out << "internalFormat: " << ccl::int32_t(internalFormat) << std::endl;
        out << "externalFormat: " << ccl::int32_t(externalFormat) << std::endl;
        out << "useMipMap: " << ccl::int32_t(useMipmap) << std::endl;
        for (int i = 0; i < 8; i++)
            out << "mipmap[" << i << "]: " << float(mipmap[i]) << std::endl;
        out << "useLodScale: " << ccl::int32_t(useLodScale) << std::endl;
        for (int i = 0; i < 16; i++)
            out << "lodScale[" << i << "]: " << float(lodScale[i]) << std::endl;
        out << "clamp: " << float(clamp) << std::endl;
        out << "magFilterAlpha: " << ccl::int32_t(magFilterAlpha) << std::endl;
        out << "magFilterColor: " << ccl::int32_t(magFilterColor) << std::endl;
        //float32            reserved_1[9];            //    spare
        out << "lambertMeridian: " << double(lambertMeridian) << std::endl;
        out << "lambertUpperLat: " << double(lambertUpperLat) << std::endl;
        out << "lambertLowerLat: " << double(lambertLowerLat) << std::endl;
        //float64            reserved_2;                //    spare
        //float32            reserved_3[5];            //    spare
        out << "useDetail: " << ccl::int32_t(useDetail) << std::endl;
        out << "detail_j: " << ccl::int32_t(detail_j) << std::endl;
        out << "detail_k: " << ccl::int32_t(detail_k) << std::endl;
        out << "detail_m: " << ccl::int32_t(detail_m) << std::endl;
        out << "detail_n: " << ccl::int32_t(detail_n) << std::endl;
        out << "detail_s: " << ccl::int32_t(detail_s) << std::endl;
        out << "useTile: " << ccl::int32_t(useTile) << std::endl;
        out << "txTile_ll_u: " << float(txTile_ll_u) << std::endl;
        out << "txTile_ll_v: " << float(txTile_ll_v) << std::endl;
        out << "txTile_ur_u: " << float(txTile_ur_u) << std::endl;
        out << "txTile_ur_v: " << float(txTile_ur_v) << std::endl;
        out << "projection: " << ccl::int32_t(projection) << std::endl;
        out << "earthModel: " << ccl::int32_t(earthModel) << std::endl;
        //int32            reserved_4;                //    spare
        out << "utmZone: " << ccl::int32_t(utmZone) << std::endl;
        out << "imageOrigin: " << ccl::int32_t(imageOrigin) << std::endl;
        out << "geoUnits: " << ccl::int32_t(geoUnits) << std::endl;
        //int32            reserved_5[2];            //    spare
        out << "hemisphere: " << ccl::int32_t(hemisphere) << std::endl;
        //int32            reserved_6[151];        //    spare
        out << "comments: " << comments << std::endl;
        //int32            reserved_7[14];            //    spare
        out << "attrVersion: " << ccl::int32_t(attrVersion) << std::endl;
        out << "controlPoints: " << ccl::int32_t(controlPoints) << std::endl;
        out << "numSubTextures: " << ccl::int32_t(numSubTextures) << std::endl;

    }


}