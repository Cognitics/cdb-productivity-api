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
/*! \brief Provides sfa::Label
\author Joshua Anghel <janghel@cognitics.net>
\date 13 October 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once
#include <string>

namespace sfa {

    enum Location
    {
        INTERIOR = 0,
        BOUNDARY = 1,
        EXTERIOR = 2,
        UNKNOWN = 3
    };

/*!\class sfa::Label Label.h Label.h
\brief Label

Provides information on a partition of space created by one or more Geometries including the dimension of the space
and the location of that space with respect to up to two Geometries.
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
    class Label
    {
    public:
        Location loc[2];
        int dim;

        Label(int dimension = -1);
        Label(Location a, Location b, int dimension = -1);
        ~Label(void) {}

        Location& operator[](int i);

//!    Switches the values of the Locations for the two Geometries in the Label
        void flip(void);

/*!\brief Copies the information of another Label into this Label using copyLabel(Label l, int n).
\param l Label to copy information from.
*/
        void copyLabel(const Label& l);

/*!\brief Copies the information from another Label for one Geometry only (using the Mod-2 boundary rule).

The Mod-2 boundary rule cause the Location in question to be changed to INTERIOR if both the current Location and
copy Location are BOUNDARY. If this is not the case, then the Location chosen between the current and copy Location will
be the Location with lower enumberated type.
\param l Label to copy information from
\param n Geometry index to copy information for
*/
        void copyLabel(const Label& l, int n);

//!    Returns the information contained in this Label as a std::string.
        std::string print(void) const;
    };

}