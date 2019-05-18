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
#include "sfa/PatchUnion.h"

#include <exception>
#include <stdexcept>

namespace sfa {

    LineString* PatchUnion::apply(const PolygonList& list)
    {
        if (list.empty()) return NULL;
        LineString* currentResult = NULL;

    //    Initialize all merge flags to false
        bool* merged = new bool[list.size()];
        for (int i = 0; i < int(list.size()); i++)
            merged[i] = false;
        int leftToMerge = int(list.size());

    //    Choose the first currentResult
        merged[0] = true;
        currentResult = new LineString(list[0]->getExteriorRing());
        --leftToMerge;

    //    Itterate through until all the polygons have been merged.
        while (leftToMerge)
        {
            std::vector<int> touchingMergeVertex;
            std::vector<int> touchingCurrentVertex;
            
            LineString* nextMerge = NULL;

    /*
        Find the next polygon to merge. The next polygon to merge must have at least 2 touching points with the
        current polygon to merge with it. Also, the LineString segment which the current and merge canidate intersect
        along must be non disjoint. If there is breaks in the interseting edge, a union know will cause the current
        result to fold in on itself to create a hole and will cause the algorithm to blow up very quickly.
    */
            for (int i = 0; i < int(list.size()); i++)
            {
                touchingMergeVertex.clear();
                touchingCurrentVertex.clear();
                if (merged[i]) continue;

                for (int j = 0; j < currentResult->getNumPoints() - 1; j++)
                {
                    for (int h = 0; h < list[i]->getExteriorRing()->getNumPoints() - 1; h++)
                    {
                        if (currentResult->getPointN(j)->equals(list[i]->getExteriorRing()->getPointN(h)))
                        {
                            touchingMergeVertex.push_back(h);
                            touchingCurrentVertex.push_back(j);
                        }
                    }
                }

    /*
        Check that the intersetion boundary between the merge canidate and current result is a single linestring with no breaks.
        Note that on the first check, the number of points of the currentResult is added before modding, this is because some machines
        return -1 for the % operation of -1 mod 5...this is not valid for these checks. Since the first check is the only one where a -1
        is possible to obtain before the mod, then it is the only one in need of this precautionary addition.
    */
                if (touchingMergeVertex.size() >= 2)
                {
                    int breakA = 0;
                    int breakB = 0;
                    for (int k = 0; k < int(touchingMergeVertex.size()); k++)
                    {
                        if (touchingCurrentVertex[k] != (touchingCurrentVertex[(k+1)%touchingCurrentVertex.size()] - 1 + (currentResult->getNumPoints() - 1 ))
                            % (currentResult->getNumPoints() - 1 ))
                            breakA++;
                        if (touchingMergeVertex[k] != (touchingMergeVertex[(k+1)%touchingMergeVertex.size()] + 1)
                            % (list[i]->getExteriorRing()->getNumPoints() - 1))
                            breakB++;
                    }
                    if (breakA > 1 || breakB > 1) continue;

                    merged[i] = true;
                    nextMerge = list[i]->getExteriorRing();
                    break;
                }
            }

    /*
        If no merge canidate was found, then the PolygonList given is not defined properly. Either there boundary verticies of one or all
        of the polygons are not defined in the correct order, or there is a polygon that is not connected to the patch network by at least
        one edge.
    */
            if (!nextMerge)
            {
                delete [] merged;
                return NULL; //throw std::runtime_error("PatchUnion::apply() Invalid PolygonList given");
            }
            
            LineString* tempLine = new LineString;

    /*--------------------------------------------------------------------------------------------------------
        Special Case !
        This special case is when the the merge and current linestrings are equal. This happens if and 
        only if the patches form a closed surface, in which case a null linestring is returned.
    */
            if (touchingMergeVertex.size() == (nextMerge->getNumPoints() - 1)
                &&
                touchingCurrentVertex.size() == (currentResult->getNumPoints() - 1) 
                )
            {
                delete [] merged;
                return NULL;
            }

    /*
        Special Case !
        This special case occurs when the Merge polygon is completely eclipsed by the current result.
        Unfortunately, if this occurs, we cannot use the merge polygon to determine the start and end points,
        so we must use the current linestring to find the start and endpoints and then add only the current
        result component.
    */
            if (touchingMergeVertex.size() == (nextMerge->getNumPoints() - 1))
            {
                int startIndex = 0;
                int stopIndex = int(touchingCurrentVertex.size()) - 1;
                for (int i = 0; i < (int(touchingCurrentVertex.size()) - 1); i++)
                {
                    if (touchingCurrentVertex[i] != (touchingCurrentVertex[(i+1)%touchingCurrentVertex.size()] - 1 + (currentResult->getNumPoints() - 1 ))
                            % (currentResult->getNumPoints() - 1 ))
                    {
                        stopIndex = i;
                        startIndex = i+1;
                        break;
                    }
                }

    //    Add Current outlining ring
                Point* stop = currentResult->getPointN(touchingCurrentVertex[startIndex]);
                int currentPointIndex = touchingCurrentVertex[stopIndex];
                Point* nextPoint = currentResult->getPointN(currentPointIndex);

                do
                {
                    tempLine->addPoint(new Point(nextPoint));
                    currentPointIndex = (currentPointIndex + 1)%(currentResult->getNumPoints()-1);
                    nextPoint = currentResult->getPointN(currentPointIndex);
                } 
                while (!nextPoint->equals(stop));

                tempLine->addPoint(new Point(nextPoint));

    //    Close ring
                tempLine->addPoint(new Point(tempLine->getStartPoint()));
                if (currentResult) delete currentResult;
                currentResult = tempLine;

                leftToMerge--;
                continue;
            }
    //    End special cases -------------------------------------------------------------------------------------

    //    Find the start and stop points of the linestring which is the intersection
            int startIndex = 0;
            int stopIndex = int(touchingMergeVertex.size()) - 1;
            for (int i = 0; i < (int(touchingMergeVertex.size()) - 1); i++)
            {
                if (touchingMergeVertex[i] != (touchingMergeVertex[i+1] + 1)%(nextMerge->getNumPoints() - 1))
                {
                    stopIndex = i;
                    startIndex = i+1;
                    break;
                }
            }
            
    //    Add all the points not on the intersection from merge to the temporary result
            Point* stop = nextMerge->getPointN(touchingMergeVertex[stopIndex]);
            int currentPointIndex = touchingMergeVertex[startIndex];
            Point* nextPoint = nextMerge->getPointN(currentPointIndex);
            do
            {
                tempLine->addPoint(new Point(nextPoint));
                currentPointIndex = (currentPointIndex + 1)%(nextMerge->getNumPoints()-1);
                nextPoint = nextMerge->getPointN(currentPointIndex);
            } 
            while (!nextPoint->equals(stop));
            
    /*--------------------------------------------------------------------------------------------------------
        Special Case !
        The special case handled by the following conditional is when the current ring build is completely
        surrounded by the merge polygon. This is handled simply be not adding and edges of the current 
        ring to the result.
    */
            if (touchingCurrentVertex.size() == (currentResult->getNumPoints() - 1))
            {
                tempLine->addPoint(new Point(nextPoint));
                tempLine->addPoint(new Point(tempLine->getEndPoint()));
                if (currentResult) delete currentResult;
                currentResult = tempLine;

                leftToMerge--;
                continue;
            }

    //    End special case -------------------------------------------------------------------------------------

    //    Add Current vertex points that aren't on the intersection to the result
            stop = currentResult->getPointN(touchingCurrentVertex[startIndex]);
            currentPointIndex = touchingCurrentVertex[stopIndex];
            nextPoint = currentResult->getPointN(currentPointIndex);

            do
            {
                tempLine->addPoint(new Point(nextPoint));
                currentPointIndex = (currentPointIndex + 1)%(currentResult->getNumPoints()-1);
                nextPoint = currentResult->getPointN(currentPointIndex);
            }
            while (!nextPoint->equals(stop));

    //    Close ring
            tempLine->addPoint(new Point(nextMerge->getPointN(touchingMergeVertex[startIndex])));
            if (currentResult) delete currentResult;
            currentResult = tempLine;

            leftToMerge--;
        }
        
        return currentResult;
    };

}