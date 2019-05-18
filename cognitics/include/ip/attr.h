/****************************************************************************
Copyright 2017 Cognitics, Inc.
****************************************************************************/
#pragma once

#include <ccl/ccl.h>

namespace ip {

    class attrFile
    {
    public:
        
        typedef ccl::BigEndian<ccl::int32_t>    int32;
        typedef ccl::BigEndian<float>            float32;
        typedef ccl::BigEndian<double>            float64;

        attrFile(void);

        enum FileFormat    
        {
            FILE_FORMAT_NONE_USED            = -1,
            FILE_FORMAT_ATT_I8_PATTERN        = 0,
            FILE_FORMAT_ATT_I8_TEMPLATE        = 1,
            FILE_FORMAT_SGI_MODULATION        = 2,
            FILE_FORMAT_SGI_WITH_ALPHA        = 3,
            FILE_FORMAT_SGI_RGB                = 4,
            FILE_FORMAT_SGI_RGBA            = 5
        };

        enum MinFilterMode 
        {
            MIN_FILTER_POINT                = 0,
            MIN_FILTER_BILINEAR                = 1,
            MIN_FILTER_MIPMAP                = 2,
            MIN_FILTER_MIPMAP_POINT            = 3,
            MIN_FILTER_MIPMAP_LINEAR        = 4,
            MIN_FILTER_MIPMAP_BILINEAR        = 5,
            MIN_FILTER_MIPMAP_TRILINEAR        = 6,
            MIN_FILTER_NONE                    = 7,
            MIN_FILTER_BICUBIC                = 8,
            MIN_FILTER_BILINEAR_GEQUAL        = 9,
            MIN_FILTER_BILINEAR_LEQUAL        = 10,
            MIN_FILTER_BICUBIC_GEQUAL        = 11,
            MIN_FILTER_BICUBIC_LEQUAL        = 12
        };

        enum MagFilterMode 
        {
            MAG_FILTER_POINT                = 0,
            MAG_FILTER_BILINEAR                = 1,
            MAG_FILTER_NONE                    = 2,
            MAG_FILTER_BICUBIC                = 3,
            MAG_FILTER_SHARPEN                = 4,
            MAG_FILTER_ADD_DETAIL            = 5,
            MAG_FILTER_MODULATE_DETAIL        = 6,
            MAG_FILTER_BILINEAR_GEQUAL        = 7,
            MAG_FILTER_BILINEAR_LEQUAL        = 8,
            MAG_FILTER_BICUBIC_GEQUAL        = 9,
            MAG_FILTER_BICUBIC_LEQUAL        = 10
        };

        enum WrapMode
        {
            WRAP_REPEAT                        = 0,
            WRAP_CLAMP                        = 1,
            WRAP_NONE                        = 3,
            WRAP_MIRRORED_REPEAT            = 4
        };

        enum TexEnvMode 
        {
            TEXENV_MODULATE                    = 0,
            TEXENV_BLEND                    = 1,
            TEXENV_DECAL                    = 2,
            TEXENV_COLOR                    = 3,
            TEXENV_ADD                        = 4
        };

        enum Projection
        {
            PROJECTION_FLAT                    = 0,
            PROJECTION_LAMBERT_CONIC        = 3,
            PROJECTION_UTM                    = 4,
            PROJECTION_UNDEFINED            = 7
        };

        enum Datum 
        {
            DATUM_WGS84                        = 0,
            DATUM_WGS72                        = 1,
            DATUM_BESSEL                    = 2,
            DATUM_CLARK_1866                = 3,
            DATUM_NAD27                        = 4
       };

        enum InternalFormat 
        {
            INTERNAL_FORMAT_DEFAULT            = 0,
            INTERNAL_FORMAT_TX_I_12A_4        = 1,
            INTERNAL_FORMAT_TX_IA_8            = 2,
            INTERNAL_FORMAT_TX_RGB_5        = 3,
            INTERNAL_FORMAT_TX_RGBA_4        = 4,
            INTERNAL_FORMAT_TX_IA_12        = 5,
            INTERNAL_FORMAT_TX_RGBA_8        = 6,
            INTERNAL_FORMAT_TX_RGBA_12        = 7,
            INTERNAL_FORMAT_TX_I_16            = 8,
            INTERNAL_FORMAT_TX_RGB_12        = 9
        };

        enum ExternalFormat 
        {
            EXTERNAL_FORMAT_DEFAULT            = 0,
            EXTERNAL_FORMAT_TX_PACK_8        = 1,
            EXTERNAL_FORMAT_TX_PACK_16        = 2
        };

        int32            u_size;                    //    Number of texels in the u direction
        int32            v_size;                    //    Number of texels in the v direction
        int32            u_scale;                //    Real world size in u direction
        int32            v_scale;                //    Real world size in v direction
        int32            up_x;                    //    x component of the up vector
        int32            up_y;                    //    y component of the up vector
        int32            fileFormat;                //    (See FileFormat)
        int32            minFilterMode;            //    (See MinFilterMode)
        int32            magFilterMode;            //    (See MagFilterMode)
        int32            wrapMode;                //    (See WrapMode)
        int32            wrapMode_u;                //    Repetition type in u direction (See WrapMode)
        int32            wrapMode_v;                //    Repetition type in v direction (See WrapMode)
        int32            modifyFlag;                //    Modify flag (for internal use)
        int32            x_pivot;                //    x pivot point for rotating texture
        int32            y_pivot;                //    y pivot point for rotating texture
        int32            texEnvMode;                //    See TexEnvMode
        int32            intensityAsAlpha;        //    True if intensity pattern to be loaded in alpha with white in color
        //int32            reserved_0[8];            //    spare
        float64            float_u_size;            //    Real world size u for floating point databases
        float64            float_v_size;            //    Real world size u for floating point databases
        int32            originCode;                //    Code for origin of imported texture
        int32            kernelVersion;            //    Kernel version number
        int32            internalFormat;            //    (See InternalFormat)
        int32            externalFormat;            //    (See ExternalFormat)
        int32            useMipmap;                //    TRUE if using following 8 floats for MIPMAP kernel
        float32            mipmap[8];                //    8 floats for kernel of seperable symmetric filter
        int32            useLodScale;            //    if TRUE, use the following floats
        float32            lodScale[16];            //    LOD and Scale values
        float32            clamp;                    //    Clamp
        int32            magFilterAlpha;            //    (See MagFilterMode)
        int32            magFilterColor;            //    (See MagFilterMode)
        //float32            reserved_1[9];            //    spare
        float64            lambertMeridian;        //    Lambert conic projection central meridian
        float64            lambertUpperLat;        //    Lambert conic projection upper lattitude
        float64            lambertLowerLat;        //    Lambert conic projection lower lattitude
        //float64            reserved_2;                //    spare
        //float32            reserved_3[5];            //    spare
        int32            useDetail;                //    TRUE if using next 5 integers for detail texture
        int32            detail_j;                //    J argument for TX_DETAIL
        int32            detail_k;                //    K argument for TX_DETAIL
        int32            detail_m;                //    M argument for TX_DETAIL
        int32            detail_n;                //    N argument for TX_DETAIL
        int32            detail_s;                //    Scrambe argument for TX_DETAIL
        int32            useTile;                //    TRUE if using the next four floats for TX_TILE
        float32            txTile_ll_u;            //    Lower-left u value for TX_TILE
        float32            txTile_ll_v;            //    Lower-left v value for TX_TILE
        float32            txTile_ur_u;            //    Upper-right u value for TX_TILE
        float32            txTile_ur_v;            //    Upper-rigt v value for TX_TILE
        int32            projection;                //    (See Projection)
        int32            earthModel;                //    (See Datum)
        //int32            reserved_4;                //    spare
        int32            utmZone;                //    UTM zone
        int32            imageOrigin;            //    Image origin
                                                //        0 = Lower-left
                                                //        1 = Upper-left
        int32            geoUnits;                //    Geospecific points units
                                                //        0 = Degrees
                                                //        1 = Meters
                                                //        2 = Pixels
        //int32            reserved_5[2];            //    spare
        int32            hemisphere;                //    Hemisphere for geospecific points units
                                                //        0 = Southern
                                                //        1 = Northern
        //int32            reserved_6[151];        //    spare
        std::string        comments;                //    Comments char[512]
        //int32            reserved_7[14];            //    spare
        int32            attrVersion;            //    Attribute file version number
        int32            controlPoints;            //    Number of geospecific control points
        int32            numSubTextures;            //    # of subtextures

/*        
        for each geospecific control point:
        {
            float64            texel_u;                //    Texel u of geospecific control point
            float64            textel_v;                //    Texel v of geospecific control point
            float64            geoPoint[2];            //    Real earth coordinate of geospecific control point
                                                    //    (this value depends on the projection, earth model, and geospecific points units)
        }        
*/

        bool Read(const std::string& filename);

        bool Write(const std::string& filename);

        void Dump(std::ostream &ostream);
    };

}