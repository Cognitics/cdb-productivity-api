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

#include "scenegraph/Color.h"

namespace scenegraph
{

    Color::Color(double R, double G, double B, double A)
        : initialized(true),r(R), g(G), b(B), a(A)
    {
    }
    
    Color::Color() : initialized(false),r(1),g(1),b(1),a(1)
    {
    }

    Color::~Color()
    {
    }

    bool Color::isInitialized() const
    {
        return initialized;
    }

    bool Color::operator==(const Color &rhs) const
    {
        return (r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a);
    }

    bool Color::operator<(const Color &rhs) const
    {
        if(r < rhs.r)
            return true;
        else if (r > rhs.r)
            return false;
        else if(g < rhs.g)
            return true;
        else if (g > rhs.b)
            return false;
        else if(b < rhs.b)
            return true;
        else if (b > rhs.b)
            return false;
        else return (a < rhs.a);
    }

    bool Color::operator>(const Color &rhs) const
    {
        return !(rhs < *this) && !(rhs == *this);
    }

} //scenegraph : namespace