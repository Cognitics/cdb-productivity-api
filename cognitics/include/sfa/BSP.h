/****************************************************************************
Copyright (c) 2015 Cognitics, Inc.

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

#pragma once
#include <float.h>
#include "Geometry.h"
#include "LineString.h"
#include <list>
#include "Polygon.h"
#include "PointLocator.h"


namespace sfa
{
    typedef struct    {
        sfa::Geometry *line;
        double density;
    } BSP_NODE_AREA;

    class BSP
    {
        std::map<Geometry *, LineString> envelopeMap;
    public:
        size_t depth;
        size_t maxDepth;
        size_t geometryCount;
        size_t targetCount;
        bool vertical;
        double split;
        BSP *minChild;
        BSP *maxChild;
        std::list<Geometry *> geometries;
        
        ~BSP(void);
        BSP(size_t depth = 0, bool vertical = true);
        void addGeometry(Geometry *geometry);
        bool removeGeometry(Geometry *geometry);
        bool removeBoundingPolygon(Polygon *bounds);
        void generate(std::map<Geometry *, LineString *> &envelopes);
        //Keep a cache of envelopes to save time in BSP searching
        bool getGeometryEnvelope(Geometry *geom, LineString &envelope);
    };
    /*
    This is the base class for all of the BSP visitors. This class has not search logic and thus will not actually perform a search.
    */
    class BSPVisitor
    {
    public:
        virtual ~BSPVisitor(void);
        BSPVisitor(void);

        void visit(BSP *bsp);
        void traverse(BSP *bsp);

        virtual void visiting(BSP *bsp);
    };
    /*_______________________________________________________________________________________________
    This class searches for geometries that touch a search window that is defined as a rectangle. 
    The defined rectangle is one for which the sides are parallel to the x and y axes.
     ________________________________________________________________________________________________*/
    class BSPCollectGeometriesVisitor : public BSPVisitor
    {
    public:
        std::list<Geometry *> results;
        double minX, maxX, minY, maxY;
        void setBounds(double minX, double minY, double maxX, double maxY);

        virtual ~BSPCollectGeometriesVisitor(void);
        BSPCollectGeometriesVisitor(void);
        virtual void visiting(BSP *bsp);
    };
    /*_______________________________________________________________________________________________
    This class searches for geometries that touch a search window that is defined as a polygon.
    ________________________________________________________________________________________________*/
    class BSPCollectGeometriesInPolygonVisitor : public BSPVisitor
    {
    public:
        std::list<Geometry *> results;
        double minX, maxX, minY, maxY;
        sfa::Polygon *window;
        void setBoundingPolygon(sfa::Polygon &proposedWindow);

        virtual ~BSPCollectGeometriesInPolygonVisitor(void);
        BSPCollectGeometriesInPolygonVisitor(void);
        virtual void visiting(BSP *bsp);
    };


    /*_______________________________________________________________________________________________
    This class visits all of the leaf nodes in the BSP and does some reporting
    on the geometries that are stored there. The expectation for this class is
    that all of the geometries will be points.
    ________________________________________________________________________________________________*/
    class BSPCollectLeafEnvelopeVisitor : public BSPVisitor
    {
    public:
        std::list<BSP_NODE_AREA *> results;
        double threshold;
        virtual ~BSPCollectLeafEnvelopeVisitor(void);
        BSPCollectLeafEnvelopeVisitor(void);
        virtual void visiting(BSP *bsp);
    };
    /*_______________________________________________________
    This class searches the leaves for the target geometry
    that is either to be added or removed.
    __________________________________________________________*/
    const int INSERT = 0;
    const int REMOVE = 1;
    const int POLY_REMOVE = 2;
    const int NO_MODE = -1;

    class BSPEditVisitor : public BSPVisitor
    {
    public:
        Geometry *elementToEdit;
        int visitMode;
        double minX, maxX, minY, maxY;

        virtual ~BSPEditVisitor(void);
        BSPEditVisitor(void);
        virtual void visiting(BSP *bsp);
        void setVisitMode(int visitMode);
        void setEditElement(Geometry *target);
    };

}
