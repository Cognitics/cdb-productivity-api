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
/*! \brief Provides sfa::RelateOperation and sfa::getRelateComputer
\author Joshua Anghel <janghel@cognitics.net>
\date 20 October 2010
*/
#pragma once
#include "de9im.h"
#include "Geometry.h"

namespace sfa {

/*! \class sfa::RelateOperation
\brief RelateOperation

Represents a relational operation structure. Used as an abstract class interface for a class used to compute the de9im
of two geometries.

Usage:
\code
    sfa::GeometrySP a, b;
    sfa::de9imSP result;
    sfa::RelateOperationSP computer;

//    For 2D analysis use
    computer = sfa::getRelateComputer(a,b);

//    For a 3D analysis use
    computer = sfa::getRelateComputer(a,b,true);

//    Compute matrix
    result = comuter->computeIM();
\endcode

It is advised to use the sfa::Relate class instead of interacting with the RelateOperations directly as the sfa::Relate class
is capable of determining and using efficient shortcuts to determine various relations instead of using a matrix each time.
The sfa::Relate class also maintains the intersection matrix so that it is only computed if and when it is needed and ensuring
it is stored so that it is only computed once.

Depending on the last option given to the sfa::getRelateCompute call a RelateOperation can be either performed only in the xy
plane or in full 3D space.
*/
    class RelateOperation
    {
    protected:

        de9im intersectionMatrix;

        RelateOperation(void){}

    public:

        virtual ~RelateOperation(void){}

/*! \brief Creates a RelateComputer for the two Geometries.

If is3D is set to true, then the RelateCompute3D class will be used to create an intersection matrix. Note that currently 
only standard Geometries and MultiPoints are supported for full 3D de9im computation.
\param a First Geometry
\param b Second Geometry
\param is3D Use true 3D in computing the de9im
\return A RelateOperation capable of computing the de9im matrix between the two given geometries.
*/
        static RelateOperation* getRelateComputer(const Geometry* a, const Geometry* b, bool is3D = false);

/*! \brief Computes the de9im intersection matrix of the two Geometry objects stored in the derived RelateOperation class.
\return The de9im intersection matrix representing the relation between two Geometries.
*/
        virtual de9im computeIM(void) = 0;
    };

}