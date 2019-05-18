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

#include "scenegraph/scenegraph.h"
#include <sfa/sfa.h>

namespace scenegraph
{
    bool drapeFeature(sfa::Feature *feature, Scene *scene, double zOffset, bool discardZ, bool onlyEndPoints)
    {
        if(!scene || !feature || !feature->geometry)
            return false;

        // TODO: we need to support more than just linestrings
        if(feature->geometry->getWKBGeometryType() != sfa::wkbLineString)
            return false;

        sfa::LineString *featureLine = dynamic_cast<sfa::LineString *>(feature->geometry);
        if(onlyEndPoints)
        {
            sfa::Point *startp = featureLine->getPointN(0);
            sfa::Point *endp = featureLine->getPointN(featureLine->getNumPoints()-1);
            double originalStartZ = discardZ ? 0.0f : startp->Z();
            double originalEndZ = discardZ ? 0.0f : endp->Z();
            bool foundStartFace = false;
            bool foundEndFace = false;
            for(size_t k = 0, kc = scene->faces.size(); k < kc; ++k)
            {
                // identify point in 2D face and set Z value
                Face &face = scene->faces.at(k);
                if(!foundStartFace && face.interpolatePointInFace(*startp))
                {
                    foundStartFace = true;
                    startp->setZ(startp->Z() + originalStartZ + zOffset);
                }
                if(!foundEndFace && face.interpolatePointInFace(*endp))
                {
                    foundEndFace = true;
                    endp->setZ(endp->Z() + originalStartZ + zOffset);
                }
                if(foundEndFace && foundStartFace)
                    break;
            }
            // Interpolate all positions between start and end 
            double deltaZ = endp->Z() - startp->Z();
            double linelength = featureLine->getLength2D();
            sfa::Point *lastPoint = featureLine->getPointN(0);
            double pos = 0;
            for(size_t i = 1, c = featureLine->getNumPoints()-1; i < c; ++i)
            {
                sfa::Point *point = featureLine->getPointN(int(i));
                double deltax = lastPoint->X()-point->X();
                double deltay = lastPoint->Y()-point->Y();
                double seglen = sqrt(deltax*deltax + deltay*deltay);
                pos += seglen;
                point->setZ(startp->Z() + (deltaZ*(pos/linelength)));
                lastPoint = point;
            }
            return true;
        }

        std::vector<std::map<double, sfa::Point *> > newPointMaps;
        for(size_t i = 0, c = featureLine->getNumPoints(); i < c; ++i)
        {
            sfa::Point *point = featureLine->getPointN(int(i));
            double originalZ = discardZ ? 0.0f : point->Z();

            std::vector<sfa::Point *> newPoints;
            for(size_t k = 0, kc = scene->faces.size(); k < kc; ++k)
            {
                // identify point in 2D face and set Z value
                Face &face = scene->faces.at(k);
                if(face.interpolatePointInFace(*point))
                    point->setZ(point->Z() + originalZ);

                if(i + 1 >= c)        // we're at the last point, so there are no more segments
                    continue;

                // test for 2D edge intersection of the next segment
                sfa::Point *nextPoint = featureLine->getPointN(int(i + 1));
                double xa = point->X();
                double ya = point->Y();
                double xb = nextPoint->X();
                double yb = nextPoint->Y();
                double A2 = yb - ya;
                double B2 = xa - xb;
                int numVerts = face.getNumVertices();
                if(numVerts)
                {
                    sfa::Point prevPt = face.getVertN(0);
                    for(int l = 1; l < numVerts; ++l)
                    {
                        sfa::Point facept = face.getVertN(l);

                        double x1 = prevPt.X();
                        double y1 = prevPt.Y();
                        double z1 = prevPt.Z();
                        double x2 = facept.X();
                        double y2 = facept.Y();
                        double z2 = facept.Z();
                        prevPt = facept;

                        double A1 = y2 - y1;
                        double B1 = x1 - x2;
                        double C1 = (A1 * x1) + (B1 * y1);
                        double C2 = (A2 * xa) + (B2 * ya);
                        double det = (A1 * B2) - (A2 * B1);
                        if(det == 0)
                            continue;

                        double x = ((B2 * C1) - (B1 * C2)) / det;
                        double y = ((A1 * C2) - (A2 * C1)) / det;

                        double minX1 = (x1 < x2) ? x1 : x2;
                        double minX2 = (xa < xb) ? xa : xb;
                        double minX = (minX1 > minX2) ? minX1 : minX2;
                        double minY1 = (y1 < y2) ? y1 : y2;
                        double minY2 = (ya < yb) ? ya : yb;
                        double minY = (minY1 > minY2) ? minY1 : minY2;
                        double maxX1 = (x1 > x2) ? x1 : x2;
                        double maxX2 = (xa > xb) ? xa : xb;
                        double maxX = (maxX1 < maxX2) ? maxX1 : maxX2;
                        double maxY1 = (y1 > y2) ? y1 : y2;
                        double maxY2 = (ya > yb) ? ya : yb;
                        double maxY = (maxY1 < maxY2) ? maxY1 : maxY2;

                        if((x >= minX) && (x <= maxX) && (y >= minY) && (y <= maxY))
                        {
                            sfa::Point *pt = new sfa::Point(x, y);
                            sfa::LineString ls;
                            ls.addPoint(new sfa::Point(x1, y1, z1));
                            ls.addPoint(new sfa::Point(x2, y2, z2));
                            ls.interpolateZ(pt);
                            pt->setZ(pt->Z() + originalZ);
                            newPoints.push_back(pt);
                        }
                    }
                }
            }            

            // sort the newPoints based on distance from current point
            std::map<double, sfa::Point *> newPointMap;
            for(size_t k = 0, kc = newPoints.size(); k < kc; ++k)
            {
                sfa::Point *newPoint = newPoints.at(k);
                double dist = ((newPoint->X() - point->X()) * (newPoint->X() - point->X())) + ((newPoint->Y() - point->Y()) * (newPoint->Y() - point->Y()));
                if(newPointMap.find(dist)!=newPointMap.end())
                    delete newPointMap[dist];
                newPointMap[dist] = newPoint;
            }
            newPointMaps.push_back(newPointMap);
        }
        
        
        // add the new points to the feature line
        for(size_t i = 0, j = 0, c = newPointMaps.size(); i < c; ++i, ++j)
        {
            for(std::map<double, sfa::Point *>::iterator it = newPointMaps[i].begin(), end = newPointMaps[i].end(); it != end; ++it, ++j)
                featureLine->insertPoint(int(j + 1), it->second);
        }

        // add the z offset
        for(size_t i = 0, c = featureLine->getNumPoints(); i < c; ++i)
            featureLine->getPointN(int(i))->setZ(featureLine->getPointN(int(i))->Z() + zOffset);

        return true;
    }

}