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
//#pragma optimize( "", off )
#include "sfa/BSP.h"




namespace sfa
{
    BSP::~BSP(void)
    {
        delete minChild;
        delete maxChild;
    }

    BSP::BSP(size_t depth, bool vertical) : depth(depth), maxDepth(10), geometryCount(0), vertical(vertical), split(0.0f), minChild(NULL), maxChild(NULL), targetCount(10)
    {

    }

    /*************************************************************************************************
    This method will add a geometry to the BSP. The idea here is that ALL of the geometries that
    the BSP is to manage must be added to the BSP BEFORE any searches can be done. The method expects
    one argument:

    geometry    This is the geometry that is to be added to the BSP.

    The method does not have a meaningful return value.
    ****************************************************************************************************/
    void BSP::addGeometry(Geometry *geometry)
    {
        geometryCount += 1;
        geometries.push_back(geometry);
    }
    /*****************************************************************************************
    This method removes a geometry from the node. The method expects one argument:

    target        This is a pointer to the Geometry that is to be
                removed from the list.

    The method will search through the geometries list until it finds a match for 
    the target pointer that was provided as the argument. Once a match is found,
    that element is removed from the list.
    *******************************************************************************************/
    bool BSP::removeGeometry(Geometry *target)
    {
        bool hit = false;
        for (std::list<Geometry *>::iterator it = geometries.begin(), end = geometries.end(); it != end; )
        {
            Geometry *listElement = *it;
            if (listElement == target)
            {
                it = geometries.erase(it);
                geometryCount -= 1;
                end = geometries.end();
                hit = true;
                break;
            }
            else
            {
                ++it;
            }
        }
        return(hit);
    }

    bool BSP::removeBoundingPolygon(Polygon *bounds)
    {
        bool hit = false;
        for (std::list<Geometry *>::iterator it = geometries.begin(), end = geometries.end(); it != end;)
        {
            Geometry *listElement = *it;
            Point *point = dynamic_cast<Point *>(listElement);
            bool inside = (point != NULL)? point->intersects(bounds) : false;
            if (inside)
            {
                hit = true;
                delete (*it);
                it = geometries.erase(it);
                geometryCount -= 1;
                end = geometries.end();
                continue;
            }
            else
            {
                ++it;
            }
        }
        return hit;
    }

    /***************************************************************************************************
    This method takes all of the geometries that have been added via the addGeometry() method and
    expands those out into a BSP tree. The method expects one argument

    envelopes    This is a std::map that contains the bounding boxes for all of the geometries
                that this tree will manage. The map is a collection of key/value pairs where the
                key is the target geometry and the value is is associated bounding box. The bounding
                box is in the form of a 2 point lineString. For the first call of this method (ie. happens
                when the target application wants to fill the BSP), the caller does NOT specify the
                envelopes - or else provides an empty map. The class default will take care of providing
                an empty map if the caller does not provide one. The idea here is that for the first call
                to the method, an empty map is provided and that map will get filled by the method. For
                calls that happen as part of the recursion, the filled envelopes structure will get
                passed in.

    The method does not have a meaningful return value.
    ******************************************************************************************************/
    void BSP::generate(std::map<Geometry *, LineString *> &envelopes)
    {
        /*Once this function runs, the children will be created and so this ensures that the function cannot be run again*/
        if (minChild)
            return;
        if ((depth > maxDepth) || (geometries.size() < targetCount))
            return;
                                            /*__________________________________________________
                                            This will mark the condition that this method call
                                            is the top level call. That means that this call was
                                            not made somewhere inside the recursion, but rather
                                            at the very top of the recursion.
                                            _____________________________________________________*/
        bool isTop = envelopes.empty(); 
                                            /*_____________________________________________________
                                            This section obtains that bounding boxes for all of the
                                            geometries that are to be expanded into the tree. This is
                                            only done at the very top level generate call so that
                                            redundant envelope generation is suppressed.
                                            _______________________________________________________*/
        if (isTop)
        {
            for (std::list<Geometry *>::iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
            {
                Geometry *geometry = *it;
                LineString *envelope = dynamic_cast<LineString *>(geometry->getEnvelope());
                envelopes[geometry] = envelope;
            }
        }


        double minX = DBL_MAX;
        double maxX = -DBL_MAX;
        double minY = DBL_MAX;
        double maxY = -DBL_MAX;
                                            /*_____________________________________________________
                                            This section loops through all of the geometries in
                                            order to find the bounding box for current set of
                                            geometries. If this is the top level generate, this
                                            will be the bounding box for the entire set.
                                            _____________________________________________________*/
        for (std::list<Geometry *>::iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
        {
            Geometry *geometry = *it;
            LineString *envelope = envelopes[geometry];
            if (envelope && (envelope->getNumPoints() == 2))
            {
                Point *minPoint = envelope->getPointN(0);
                Point *maxPoint = envelope->getPointN(1);
                minX = std::min<double>(minX, minPoint->X());
                maxX = std::max<double>(maxX, maxPoint->X());
                minY = std::min<double>(minY, minPoint->Y());
                maxY = std::max<double>(maxY, maxPoint->Y());
            }
        }
                                            /*_____________________________________________________________
                                            This section uses the bounding box for the current node to
                                            calculate the split coordinate. This will be the the coordinate
                                            that bisects the data in the currently defined dimension.
                                            ______________________________________________________________*/
        if (vertical)
        {
            split = minX + ((maxX - minX) / 2);
        }
        else
        {
            split = minY + ((maxY - minY) / 2);
        }
        // create children
        minChild = new BSP(depth + 1, !vertical);
        minChild->maxDepth = maxDepth;
        maxChild = new BSP(depth + 1, !vertical);
        maxChild->maxDepth = maxDepth;
                                            /*_________________________________________________________
                                            This section makes a recursive call to the generate method
                                            according to the split coordinate that was calculated above
                                            __________________________________________________________*/
        std::list<Geometry *> saved_geometries;
        for (std::list<Geometry *>::iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
        {
            Geometry *geometry = *it;
            LineString *envelope = envelopes[geometry];
                                            /*___________________________________________________________
                                            If the geometry is legitimate, it will have a valid entry
                                            in the envelopes map. If it does not, ignore it. If it does,
                                            figure out which child should manage it. 
                                            ___________________________________________________________*/
            if (envelope && (envelope->getNumPoints() == 2))
            {
                Point *minPoint = envelope->getPointN(0);
                Point *maxPoint = envelope->getPointN(1);
                                            /*___________________________________________________________
                                            The nodes have two different orientations, horizontal and
                                            vertical. The logic for both is similar. In both cases, the
                                            split member specifies the coordinate that decides which child
                                            will manage the geometry. Three cases arise:

                                            1) min >= split  - in this case the maxChild will manage the 
                                                               geometry.
                                            2) max < split   - in this case the minChild will manage the
                                                               geometry.
                                            3) If neither of these is true, the bounding box spans the split.
                                               In that event, the current node will keep the geometry in its
                                               list of geometries (ie. saved_geometries).
                                            ________________________________________________________________*/
                if (vertical)
                {
                    if (minPoint->X() >= split)
                    {
                        maxChild->addGeometry(geometry);
                    }
                    else if (maxPoint->X() < split)
                    {
                        minChild->addGeometry(geometry);
                    }
                    else
                    {
                        saved_geometries.push_back(geometry);
                    }
                }
                else
                {
                    if (minPoint->Y() >= split)
                    {
                        maxChild->addGeometry(geometry);
                    }
                    else if (maxPoint->Y() < split)
                    {
                        minChild->addGeometry(geometry);
                    }
                    else
                    {
                        saved_geometries.push_back(geometry);
                    }
                }

            }
        }
        // update sources and generate children
        geometries = saved_geometries;
        geometryCount = geometries.size();
        minChild->generate(envelopes);
        maxChild->generate(envelopes);
                                            /*___________________________________________________________________________
                                            The idea here is to remove all of the envelopes after the entire generation
                                            process has completed. This will be the case when the top level generate call
                                            ends. Thus, if this is the top level generate - this is the place to do the
                                            envelope removal.
                                            _____________________________________________________________________________*/
        if (isTop)
        {
            for (std::map<Geometry *, LineString *>::iterator it = envelopes.begin(), end = envelopes.end(); it != end; ++it)
            {
                delete it->second;
            }
        }
    }

    bool BSP::getGeometryEnvelope(Geometry *geom, LineString &envelope)
    {
        std::map<Geometry *, LineString>::iterator iter = envelopeMap.find(geom);
        if (iter != envelopeMap.end())
        {
            envelope = iter->second;
            return true;
        }
        LineString *newenv = dynamic_cast<LineString *>(geom->getEnvelope());
        if (newenv)
        {
            envelope = *newenv;
            envelopeMap[geom] = *newenv;
            delete newenv;
            return true;
        }
        return false;
    }
    /*This is the class denstuctor*/
    BSPVisitor::~BSPVisitor(void)
    {
    }
    /*This is the class constructor*/
    BSPVisitor::BSPVisitor(void)
    {
    }

    void BSPVisitor::visit(BSP *bsp)
    {
        visiting(bsp);
    }

    void BSPVisitor::traverse(BSP *bsp)
    {
        if (!bsp)
            return;
        if (bsp->minChild)
            visit(bsp->minChild);

        if (bsp->maxChild)
            visit(bsp->maxChild);
    }

    void BSPVisitor::visiting(BSP *bsp)
    {
        traverse(bsp);
    }

    /******************************************************************************
    This function is used to define the bounding box that will be used in order to 
    collect the geometries that are in the BSP.
    *******************************************************************************/
    void BSPCollectGeometriesVisitor::setBounds(double minX, double minY, double maxX, double maxY)
    {
        this->minY = minY;
        this->minX = minX;
        this->maxX = maxX;
        this->maxY = maxY;
    }

    BSPCollectGeometriesVisitor::~BSPCollectGeometriesVisitor(void)
    {
    }

    BSPCollectGeometriesVisitor::BSPCollectGeometriesVisitor(void) : minX(-DBL_MAX), maxX(DBL_MAX), minY(-DBL_MAX), maxY(DBL_MAX)
    {
    }
    /***************************************************************************************************
    This method extracts all of the geometries that touch the above specified bounding box and places
    them into the results list. The method expects one argument:

    bsp        This is the node that the method is to operate on. The method is called recursively.

    The method does not have a return value.
    ****************************************************************************************************/
    void BSPCollectGeometriesVisitor::visiting(BSP *bsp)
    {
                                                    /*______________________________________________________
                                                    This section loops through all of the geometries
                                                    that are attached to this node. All of the geometries
                                                    that touch the bounding box are appended to the results
                                                    list.
                                                    _________________________________________________________*/
        for (std::list<Geometry *>::iterator it = bsp->geometries.begin(), end = bsp->geometries.end(); it != end; ++it)
        {
            Geometry *geometry = *it;
            LineString envelope;// = dynamic_cast<LineString *>(geometry->getEnvelope());
            if (bsp->getGeometryEnvelope(geometry, envelope))
            {
                if (envelope.getNumPoints() == 2)
                {
                    Point *minPoint = envelope.getPointN(0);
                    Point *maxPoint = envelope.getPointN(1);

                    if ((minPoint->X() <= maxX) &&
                        (maxPoint->X() >= minX) &&
                        (minPoint->Y() <= maxY) &&
                        (maxPoint->Y() >= minY))
                    {
                        results.push_back(geometry);
                    }
                }

            }
        }
                                                    /*________________________________________________
                                                    Return to the caller if this node has no children.
                                                    __________________________________________________*/
        if (! bsp->minChild) 
            return;
                                                    /*____________________________________________________
                                                    This section extends the search to those child nodes
                                                    that touch the bounding box. This check is done on the
                                                    X or Y portions of the bounding box according to the
                                                    direction of the split. 2 cases are addressed:

                                                    1) min < split
                                                    2) max > split

                                                    A third, and implied case is the one for which both are
                                                    true.
                                                    ________________________________________________________*/
        if (bsp->vertical)
        {
            if (minX <= bsp->split)
                visit(bsp->minChild);
            if (maxX >= bsp->split)
                visit(bsp->maxChild);
        }
        else
        {
            if (minY <= bsp->split)
                visit(bsp->minChild);
            if (maxY >= bsp->split)
                visit(bsp->maxChild);
        }
    }

    BSPCollectGeometriesInPolygonVisitor::BSPCollectGeometriesInPolygonVisitor(void) : minX(-DBL_MAX), maxX(DBL_MAX), minY(-DBL_MAX), maxY(DBL_MAX), window(NULL)
    {
    }

    BSPCollectGeometriesInPolygonVisitor::~BSPCollectGeometriesInPolygonVisitor(void) 
    {
    }
    void BSPCollectGeometriesInPolygonVisitor::setBoundingPolygon(sfa::Polygon &proposedWindow)
    {
        LineString *envelope = dynamic_cast<LineString *>(proposedWindow.getEnvelope());
        if (envelope && (envelope->getNumPoints() == 2))
        {
            Point *minPoint = envelope->getPointN(0);
            Point *maxPoint = envelope->getPointN(1);
            minX = minPoint->X();
            minY = minPoint->Y();
            maxX = maxPoint->X();
            maxY = maxPoint->Y();
            if (window != NULL)
            {
                window->clearRings();
                delete (window);
            }
            window = new Polygon(proposedWindow);
        }
        else
        {
            minX = 0;
            minY = 0;
            maxX = 0;
            maxY = 0;
            //throw std::runtime_error("unable to get bounds for search polygon");
        }
        delete envelope;
        results.clear();
    }
    void BSPCollectGeometriesInPolygonVisitor::visiting(BSP *bsp)
    {    
                                            /*______________________________________________________
                                            This section loops through all of the geometries
                                            that are attached to this node. All of the geometries
                                            that touch the bounding box are appended to the results
                                            list.
                                            _________________________________________________________*/
        for (std::list<Geometry *>::iterator it = bsp->geometries.begin(), end = bsp->geometries.end(); it != end; ++it)
        {
            Geometry *geometry = *it;
            Point *innerPoint;
            LineString *innerLineString;
            if (innerLineString = dynamic_cast<LineString *>(geometry))
            {
                
            }
            else if (innerPoint = dynamic_cast<Point *>(geometry))
            {
                bool inside = innerPoint->intersects(window);
                if(inside)
                    results.push_back(geometry);
            }
        }
                                        /*________________________________________________
                                        Return to the caller if this node has no children.
                                        __________________________________________________*/
        if (bsp->minChild == NULL)
            return;
                                        /*____________________________________________________
                                        This section extends the search to those child nodes
                                        that touch the bounding box. This check is done on the
                                        X or Y portions of the bounding box according to the
                                        direction of the split. 2 cases are addressed:

                                        1) min < split
                                        2) max > split

                                        A third, and implied case is the one for which both are
                                        true.
                                        ________________________________________________________*/
        if (bsp->vertical)
        {
            if (minX <= bsp->split)
                visit(bsp->minChild);
            if (maxX >= bsp->split)
                visit(bsp->maxChild);
        }
        else
        {
            if (minY <= bsp->split)
                visit(bsp->minChild);
            if (maxY >= bsp->split)
                visit(bsp->maxChild);
        }
    }

    BSPCollectLeafEnvelopeVisitor::BSPCollectLeafEnvelopeVisitor(void) : threshold(0.0)
    {
    }
    /*________________________________________________________________________
    This is the class destructor. When the visiting method is called, it will
    fill in the result structure with some nested pointers - meaning that
    nested heap allocations occurred. This method will first free up the
    LineStrings and then the enclosing structure.
    ___________________________________________________________________________*/
    BSPCollectLeafEnvelopeVisitor::~BSPCollectLeafEnvelopeVisitor(void)
    {
        for (std::list<BSP_NODE_AREA *>::iterator it = results.begin(), end = results.end(); it != end; ++it)
        {
            BSP_NODE_AREA *sabot = *it;
            Geometry *geometry = sabot->line;
            LineString *box = dynamic_cast<LineString *>(geometry);
            box->clearPoints();
            delete(sabot);
        }
    }
    void BSPCollectLeafEnvelopeVisitor::visiting(BSP *bsp)
    {
        double x0 = DBL_MAX, y0 = DBL_MAX, x1 = -DBL_MAX, y1 = -DBL_MAX,x,y;
        int pointCount = 0;
                                    /*______________________________________________________
                                    This section loops through all of the geometries
                                    that are attached to this node.
                                    _________________________________________________________*/
        for (std::list<Geometry *>::iterator it = bsp->geometries.begin(), end = bsp->geometries.end(); it != end; ++it)
        {
            Geometry *geometry = *it;
            Point *point;
            if (point = dynamic_cast<Point *>(geometry))
            {
                x = point->X();
                y = point->Y();
                x0 = (x < x0) ? x : x0;
                y0 = (y < y0) ? y : y0;
                x1 = (x > x1) ? x : x1;
                y1 = (y > y1) ? y : y1;
                pointCount += 1;
            }
        }
                                    /*______________________________________________________
                                    If the geometries member had some content, the bounding
                                    box for that content is tallied here. Also the density
                                    of data that is in the bounding box is tallied.
                                    _________________________________________________________*/
        if (pointCount)
        {
            BSP_NODE_AREA *sabot = new BSP_NODE_AREA();
            LineString *envelope = new LineString();
            double area;
                                    /*_____________________________________________________
                                    The point data that is being tallied here can have
                                    a bounding box that is a single point in the case 
                                    that a point is isolated in the geography. Another
                                    phenomenon is that the bounding box can be a line
                                    in the event that some points are artificially 
                                    aligned due to the quantization effect of how the
                                    points are gathered. In both of these cases, a bounding
                                    box having an area of 0 will result.  This section ensures
                                    that the area will not be 0.

                                    Further, in the case of the solitary point, the area will
                                    be 1.0 - ensuring that solitary points do not get undue
                                    emphasis.
                                    ___________________________________________________________*/
            x1 = (x0 == x1) ? x1 + 1.0 : x1;
            y1 = (y0 == y1) ? y1 + 1.0 : y1;

            area = (y1 - y0) * (x1 - x0);
            area = (area < 1.0) ? 1.0 : area;
            sabot->density = double(pointCount)/area;
                                    /*___________________________________________
                                    The idea here is to only capture results for
                                    regions of data that exceed a chosen density.
                                    The density (ie. threshold) can be specified 
                                    in the instance before this visiting method 
                                    is called.
                                    ______________________________________________*/
            if (sabot->density > threshold)
            {
                envelope->addPoint(sfa::Point(double(x0), double(y0)));
                envelope->addPoint(sfa::Point(double(x1), double(y0)));
                envelope->addPoint(sfa::Point(double(x1), double(y1)));
                envelope->addPoint(sfa::Point(double(x0), double(y1)));
                envelope->addPoint(sfa::Point(double(x0), double(y0)));
                sabot->line = envelope;
                results.push_back(sabot);
            }
            else
            {
                delete(sabot);
                delete(envelope);
            }
        }
                                    /*________________________________________________
                                    Return to the caller if this node has no children.
                                    __________________________________________________*/
        if (!bsp->minChild)
            return;
                                    /*________________________
                                    Visit ALL of the children
                                    __________________________*/
        visit(bsp->minChild);
        visit(bsp->maxChild);
    }
    /*_________________________________________________________
    These are the constructor and destructor methods for the
    BSPEditVisitor class.
    ____________________________________________________________*/
    BSPEditVisitor::BSPEditVisitor(void) :visitMode(NO_MODE), elementToEdit(NULL)
    {
    }

    BSPEditVisitor::~BSPEditVisitor(void)
    {
    }
    /*______________________________________________________________
    This method sets the element pointer that is to be involved in
    the edit operation. The element will either be added or removed
    from the BSP according to the visit mode that is set. The method
    expects one pass parameter:

    target        This is the pointer to the Geometry that will be
                involved in the edit operation.
    _________________________________________________________________*/
    void BSPEditVisitor::setEditElement(Geometry *target)
    {
        if (target != NULL)
        {
            elementToEdit = target;
            LineString *envelope = dynamic_cast<LineString *>(target->getEnvelope());
            if (envelope && (envelope->getNumPoints() == 2))
            {
                Point *minPoint = envelope->getPointN(0);
                Point *maxPoint = envelope->getPointN(1);
                minX = minPoint->X();
                minY = minPoint->Y();
                maxX = maxPoint->X();
                maxY = maxPoint->Y();
            }
            else
            {
                elementToEdit = NULL;
            }
            delete envelope;
        }
    }
    /*_________________________________________________________________
    This method sets the mode in which the visit is to be done. The 
    method expects one pass parameter:

    mode    This is one of INSERT or REMOVE. These are const variables
            that are set to 0 or 1 respectively.
    ____________________________________________________________________*/
    void BSPEditVisitor::setVisitMode(int mode)
    {
        switch (mode)
        {
        case INSERT:
        case REMOVE:
        case POLY_REMOVE:    visitMode = mode;
                            break;
        }
    }
    /*____________________________________________________________________
    This method walks the BSP tree in order to perform the specified
    edit operation. Note that in the case of inserts, no new nodes will
    be added to the tree. Any inserts that need to happen will be done
    by inserting into the geometries list of an existing node.
    ______________________________________________________________________*/
    void BSPEditVisitor::visiting(BSP *bsp)
    {                                    
                                        /*______________________________________________________________
                                        If the setEditElement method was not called or else was provided
                                        with an invalid value, the method exits, having done nothing.
                                        ________________________________________________________________*/
        if (elementToEdit == NULL)
        {
            return;
        }
                                        /*____________________________________________________
                                        This section handles the simple cases of:
                                        - no edit mode was specified

                                        - the walk is at the bottom of the tree and
                                          the target element has not yet been installed.

                                        - the target element to be removed is in the current
                                          geometries list.
                                        ________________________________________________________*/
        switch (visitMode)
        {
            case NO_MODE: return;
            case INSERT: if (bsp->minChild == NULL)
                        {
                             bsp->addGeometry(elementToEdit);
                             return;
                        }
                         break;
            case REMOVE: if (bsp->removeGeometry(elementToEdit))
                        {
                             return;
                        }
                         break;
            case POLY_REMOVE:    Polygon *bounds = dynamic_cast<Polygon *>(elementToEdit);
                                if ((bounds != NULL) && (bsp->minChild == NULL) && bsp->removeBoundingPolygon(bounds))
                                {
                                    return;
                                }
                                break;
        }
                                            /*________________________________________________
                                            Return to the caller if this node has no children.
                                            __________________________________________________*/
        if (bsp->minChild == NULL)
            return;

                                            /*____________________________________________________
                                            This section extends the search to those child nodes
                                            that touch the bounding box. This check is done on the
                                            X or Y portions of the bounding box according to the
                                            direction of the split. 2 cases are addressed:

                                            1) min < split
                                            2) max > split

                                            A third, and implied case is the one for which both are
                                            true.
                                            ________________________________________________________*/
        if (bsp->vertical)
        {
            if (minX <= bsp->split)
                visit(bsp->minChild);
            if (maxX >= bsp->split)
                visit(bsp->maxChild);
        }
        else
        {
            if (minY <= bsp->split)
                visit(bsp->minChild);
            if (maxY >= bsp->split)
                visit(bsp->maxChild);
        }

    }
}