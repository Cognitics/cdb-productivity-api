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
/*! \brief Provides sfa::de9im
\author Josh Anghel <joshanghel@cognitics.net>
\date July 15 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once
#include "Label.h"

//#if (_MSC_VER >= 1700)
#include <memory>
//#else
//#include <boost/tr1/memory.hpp>
//#endif
#include <string>

namespace sfa 
{

    class de9im;
    typedef std::shared_ptr<de9im> de9imSP;

/*! \class sfa::de9im de9im.h de9im.h
\brief de9im

Dimensionally Extended Nine-Intersection Matrix

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0) 6.1.2
*/
    class de9im
    {
    protected:
        int matrix[9];

    public:

//!    Constructs a matrix of default values of {-1,-1,-1,-1,-1,-1,-1,-1,-1}.
        de9im(void);

//!    Construct a matrix with default values of {a,b,c,d,e,f,g,h,i}.
        de9im(int a, int b, int c, int d, int e, int f, int g, int h, int i);

/*! \brief Construct a matrix from a char array

The char array should contain 9 characters, which will translate into the values listed below:

T/t    ->    0
F/f    ->    -1
0    ->    0
1    ->    1
2    ->    2

Notice that T/t transform to 0, that is because their actualy value doesn't matter in this case,
but instead it is only important that the value at that point is not -1. Also, since the matrix is
continually updated using the setAtLeast functions, it is a good practice to set T to the lowest
possible value to avoid any incorrect assumptions.
*/
        de9im(char m[]);

//!    Copy methods
        de9im(const de9im& other);
        de9im(const de9im* other);

        de9im& operator=(const de9im& other);
        bool operator==(const de9im& other) const;
        bool operator!=(const de9im& other) const;

        ~de9im(void) { }

/*! \fn de9im::set(int a, int b, int c, int d, int e, int f, int g, int h, int i)
\brief Set the values of the entire matrix based on the given values.
\param a INTERIOR INTERIOR index
\param b INTERIOR BOUNDARY index
\param c INTERIOR EXTERIOR index
\param d BOUNDARY INTERIOR index
\param e BOUNDARY BOUNDARY index
\param f BOUNDARY EXTERIOR index
\param g EXTERIOR INTERIOR index
\param h EXTERIOR BOUNDARY index
\param i EXTERIOR EXTERIOR index*/
        void set(int a, int b, int c, int d, int e, int f, int g, int h, int i);

/*! \fn de9im::set(Location a, Location b, int value)
\brief Set a specific value of the matrix based on the given Regions.
\param a Region of geometry A to set
\param b Region of geometry B to set
\param value The dimension of those regions intersection*/
        void set(Location a, Location b, int value);

/*! \fn de9im::set(int row, int col, int value)
\brief Set a specific value of the matrix based on the given row and column.
\param row The row of the matrix index to set
\param col The column of the matrix index to set
\param value The dimension of that row and column intersection*/
        void set(int row, int col, int value);

/*!    \brief Set the minimum value of all the entries of a matrix.

If the value of a dimension at a Location is already higher than the one specified, it will remain at its current value.
\param a INTERIOR INTERIOR index
\param b INTERIOR BOUNDARY index
\param c INTERIOR EXTERIOR index
\param d BOUNDARY INTERIOR index
\param e BOUNDARY BOUNDARY index
\param f BOUNDARY EXTERIOR index
\param g EXTERIOR INTERIOR index
\param h EXTERIOR BOUNDARY index
\param i EXTERIOR EXTERIOR index*/
        void setAtLeast(int a, int b, int c, int d, int e, int f, int g, int h, int i);

/*! \fn de9im::setAtLeast(de9imSP another)
\brief Set the minimum value of all the entries of a matrix using another matrix.

If the value of a dimension at a Location is already higher than the one specified, it will remain at its current value.
\param another Matrix with values to set the current matrix at least to.*/
        void setAtLeast(const de9im& another);
        void setAtLeast(const de9im* another);

/*! \fn de9im::setAtLeast(Location a, Location b, int value)
\brief Set the minimum value of a single entry in the matrix using the Location codes.

If the value of a dimension at a Location is already higher than the one specified, it will remain at its current value.
\param a The Location of geometry A to set
\param b The Location of geometry B to set
\param value The dimension that the matrix at that index needs to be greater than or equal to*/
        void setAtLeast(Location a, Location b, int value);

/*! \fn de9im::setAtLeast(int row, int col, int value)
\brief Set the minimum value of a single entry in the matrix using the row and column index values.

If the value of a dimension at a Location is already higher than the one specified, it will remain at its current value.
\param row The row index of the matrix to set
\param col The col index of the matrix to set
\param value The dimension that the matrix at that index needs to be greater than or equal to*/
        void setAtLeast(int row, int col, int value);

/*! \brief Set the minimum value based on a Label

Uses the locations of the label and the dimension of the label to set a point of the matrix to a minimum value.
\param l Label to read information from
*/
        void setAtLeast(Label l);

/*! \fn de9im::getValue(int row, int col)
\brief Retrieve the value of the matrix at that row and column
\param row The row of the index to retrieve
\param col The column of the index to retrieve
\return The dimension of the matrix at that index*/
        int getValue(int row, int col) const;

/*! \fn de9im::getValue(Location a, Location b)
\brief Retrieve the value of the matrix at the specified Location
\param a The Location of A to retrieve
\param b The Location of B to retrieve.
\return The dimension of the matrix at those Locations*/
        int getValue(Location a, Location b) const;
    
/*! \fn de9im::transpose
\brief Transposes the matrix

Transposes the matrix about the center diagonal. Useful to invert the order of the Geometries.
The de9im of A x B is equal to the transpose of the de9im of B x A.
\return Returns a shared pointer to this matrix after tranposition*/
        de9im& transpose(void);

/*! \fn de9im::toString
\brief Reports the de9im as a string.
\return Returns the value of the matrix as a string. F being used instead of -1*/
        std::string toString(void) const;

/*!\brief Returns the de9im as a string with \n delimeters to break the output up into a true matrix.
\return Returns a string representing the de9im.
*/
        std::string toMatrixString(void) const;

/*! \brief Returns a string representing a single row of the de9im.
\param n Row to return.
\return Row n as a string.
*/
        std::string getRowAsString(int n) const;

/*! \fn de9im::compare
\brief Compares a matrix to a de9im matrix string.

The comparison string must represent a de9im matrix with exactly 9 characters. These characters can be -1,0,1,2, t, T, f, or F.
t/T meaning the dimension at that point can be anything except -1 and the f/F being -1.

\param another Matrix to compare to
\return True if this matrix is compatable with the given string*/
        bool compare(const std::string &another) const;
    };
}