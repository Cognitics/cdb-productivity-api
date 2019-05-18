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

#include "flt/EyepointTrackplanePalette.h"

namespace flt
{
    EyepointTrackplanePalette::~EyepointTrackplanePalette(void)
    {
    }

    EyepointTrackplanePalette::EyepointTrackplanePalette(void)
    {
    }

    int EyepointTrackplanePalette::getRecordType(void)
    { 
        return FLT_EYEPOINTTRACKPLANEPALETTE;
    }

    std::string EyepointTrackplanePalette::getRecordName(void)
    {
        return "EyepointTrackplanePalette";
    }

    void EyepointTrackplanePalette::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(RESERVED4);

        eyepoints.resize(10);
        for(int i = 0; i < 10; ++i)
        {
            bs.bind(eyepoints[i].rotationCenterX);
            bs.bind(eyepoints[i].rotationCenterY);
            bs.bind(eyepoints[i].rotationCenterZ);
            bs.bind(eyepoints[i].yaw);
            bs.bind(eyepoints[i].pitch);
            bs.bind(eyepoints[i].roll);
            eyepoints[i].rotationMatrix.resize(4);
            for(int j = 0; j < 4; ++j)
            {
                eyepoints[i].rotationMatrix[j].resize(4);
                for(int k = 0; k < 4; ++k)
                    bs.bind(eyepoints[i].rotationMatrix[j][k]);
            }
            bs.bind(eyepoints[i].fieldOfView);
            bs.bind(eyepoints[i].scale);
            bs.bind(eyepoints[i].nearClippingPlane);
            bs.bind(eyepoints[i].farClippingPlane);
            eyepoints[i].flyThroughMatrix.resize(4);
            for(int j = 0; j < 4; ++j)
            {
                eyepoints[i].flyThroughMatrix[j].resize(4);
                for(int k = 0; k < 4; ++k)
                    bs.bind(eyepoints[i].flyThroughMatrix[j][k]);
            }
            bs.bind(eyepoints[i].positionX);
            bs.bind(eyepoints[i].positionY);
            bs.bind(eyepoints[i].positionZ);
            bs.bind(eyepoints[i].flyThroughYaw);
            bs.bind(eyepoints[i].flyThroughPitch);
            bs.bind(eyepoints[i].directionVectorX);
            bs.bind(eyepoints[i].directionVectorY);
            bs.bind(eyepoints[i].directionVectorZ);
            bs.bind(eyepoints[i].noFlyThrough);
            bs.bind(eyepoints[i].orthoView);
            bs.bind(eyepoints[i].valid);
            bs.bind(eyepoints[i].imageOffsetX);
            bs.bind(eyepoints[i].imageOffsetY);
            bs.bind(eyepoints[i].imageZoom);
            bs.bind(eyepoints[i].RESERVED244);
            bs.bind(eyepoints[i].RESERVED248);
            bs.bind(eyepoints[i].RESERVED252);
            bs.bind(eyepoints[i].RESERVED256);
            bs.bind(eyepoints[i].RESERVED260);
            bs.bind(eyepoints[i].RESERVED264);
            bs.bind(eyepoints[i].RESERVED268);
            bs.bind(eyepoints[i].RESERVED272);
            bs.bind(eyepoints[i].RESERVED276);
        }

        trackplanes.resize(10);
        for(int i = 0; i < 10; ++i)
        {
            bs.bind(trackplanes[i].valid);
            bs.bind(trackplanes[i].RESERVED2732);
            bs.bind(trackplanes[i].originX);
            bs.bind(trackplanes[i].originY);
            bs.bind(trackplanes[i].originZ);
            bs.bind(trackplanes[i].alignmentX);
            bs.bind(trackplanes[i].alignmentY);
            bs.bind(trackplanes[i].alignmentZ);
            bs.bind(trackplanes[i].planeX);
            bs.bind(trackplanes[i].planeY);
            bs.bind(trackplanes[i].planeZ);
            bs.bind(trackplanes[i].gridVisible);
            bs.bind(trackplanes[i].gridType);
            bs.bind(trackplanes[i].gridUnder);
            bs.bind(trackplanes[i].RESERVED2811);
            bs.bind(trackplanes[i].gridAngle);
            bs.bind(trackplanes[i].gridSpacingX);
            bs.bind(trackplanes[i].gridSpacingY);
            bs.bind(trackplanes[i].radialGridSpacingControl);
            bs.bind(trackplanes[i].rectangularGridSpacingControl);
            bs.bind(trackplanes[i].snapToGrid);
            bs.bind(trackplanes[i].RESERVED2835);
            bs.bind(trackplanes[i].RESERVED2836);
            bs.bind(trackplanes[i].gridSize);
            bs.bind(trackplanes[i].visibilityMask);
            bs.bind(trackplanes[i].RESERVED2852);
        }
    }


}
