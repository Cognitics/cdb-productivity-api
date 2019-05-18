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
#include "sfa/Label.h"
#include <sstream>

namespace sfa {

    Label::Label(int dimension)
    {
        loc[0] = UNKNOWN;
        loc[1] = UNKNOWN;
        dim = dimension;
    }

    Label::Label(Location a, Location b, int dimension)
    {
        loc[0] = a;
        loc[1] = b;
        dim = dimension;
    }

    Location& Label::operator [](int i)
    {
        return loc[i];
    }

    void Label::flip(void)
    {
        Location temp = loc[0];
        loc[0] = loc[1];
        loc[0] = temp;
    }

    void Label::copyLabel(const Label& l)
    {
        copyLabel(l,0);
        copyLabel(l,1);
    }

    void Label::copyLabel(const Label& l, int n)
    {
        //    Mod 2 rule
        if (l.loc[n] == BOUNDARY && loc[n] == BOUNDARY) loc[n] = INTERIOR;
        else if (l.loc[n] == BOUNDARY || loc[n] == BOUNDARY) loc[n] = BOUNDARY;
        loc[n] = l.loc[n] < loc[n] ? l.loc[n] : loc[n];
    }

    std::string Label::print(void) const
    {
        std::stringstream result;
        switch(loc[0])
        {
        case (0):
            result << "INTERIOR\t";
            break;
        case (1):
            result << "BOUNDARY\t";
            break;
        case (2):
            result << "EXTERIOR\t";
            break;
        default:
            result << "UNKONWN \t";
        }
        switch(loc[1])
        {
        case (0):
            result << "INTERIOR\t";
            break;
        case (1):
            result << "BOUNDARY\t";
            break;
        case (2):
            result << "EXTERIOR\t";
            break;
        default:
            result << "UNKONWN \t";
        }
        result << dim;
        return result.str();
    }

}