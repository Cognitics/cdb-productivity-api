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

#include "ip/rasterPoly.h"
#include <math.h>
#include <float.h>

namespace ip
{

    struct Edge {
        sfa::Point*    p1;
        sfa::Point*    p2;
        Edge(sfa::Point* p1, sfa::Point* p2) : p1(p1), p2(p2) { }
    };

    double InterpolateEdge(const Edge* e, double y) {
        return e->p1->X() + (y - e->p1->Y())/(e->p2->Y() - e->p1->Y())*(e->p2->X() - e->p1->X());
    }

    struct EdgeCompare {
        bool operator()(const Edge* e1, const Edge* e2) const {
            if (e1 == e2) return false;
            double temp;
            if (e1->p1->Y() < e2->p1->Y())
            {
                temp = InterpolateEdge(e1, e2->p1->Y());
                if (temp < e2->p1->X())            return true;
                else if (temp > e2->p1->X())    return false;
                temp = InterpolateEdge(e1, e2->p2->Y());
                if (temp < e2->p2->X())            return true;
                else if (temp > e2->p2->X())    return false;
            }
            else
            {
                temp = InterpolateEdge(e2, e1->p1->Y());
                if (temp < e1->p1->X())            return false;
                else if (temp > e1->p1->X())    return true;
                temp = InterpolateEdge(e2, e1->p2->Y());
                if (temp < e1->p2->X())            return false;
                else if (temp > e1->p2->X())    return true;
            }
        //    Invalid edge
            return e1 < e2;
        }
    };

    enum EventType {
        EVENT_TYPE_START = 0,
        EVENT_TYPE_STOP
    };

    struct Event {
        sfa::Point*        p;
        Edge*            e;
        EventType        type;

        Event(sfa::Point* p, Edge* e, const EventType& t) : p(p), e(e), type(t) { }
    };

    struct EventCompare {
        bool operator()(const Event* e1, const Event* e2) const {
            if (e1 == e2) return false;
            if (e1->p->Y() < e2->p->Y()) return true;
            else if (e1->p->Y() > e2->p->Y()) return false;
            else if (e1->p->X() < e2->p->X()) return true;
            else if (e1->p->X() > e2->p->X()) return false;
            else return e1 < e2;
        }
    };

    typedef std::set<Edge*,EdgeCompare> SweepLine;
    typedef std::set<Event*,EventCompare> EventList;

    void AddToEventList(const sfa::LineString& linestring, EventList& list, int height)
    {
        for (int i=1; i<linestring.getNumPoints(); i++)
        {
            sfa::Point* p1 = linestring.getPointN(i-1);
            sfa::Point* p2 = linestring.getPointN(i);
            if (std::max<double>(p1->Y(),p2->Y()) < 0)
                continue;
            if (std::min<double>(p1->Y(),p2->Y()) > height)
                continue;
            if ((int)(0.5+p1->Y()) < (int)(0.5+p2->Y()))
            {
                if (abs(p1->Y() - p2->Y()) < 2)
                    printf("");
                Edge* edge = new Edge(p1, p2);
                Event* e1 = new Event(p1, edge, EVENT_TYPE_START);
                Event* e2 = new Event(p2, edge, EVENT_TYPE_STOP);
                list.insert(e1);
                list.insert(e2);
            }
            else if ((int)(0.5+p1->Y()) > (int)(0.5+p2->Y()))
            {
                if (abs(p1->Y() - p2->Y()) < 2)
                    printf("");
                Edge* edge = new Edge(p2, p1);
                Event* e1 = new Event(p2, edge, EVENT_TYPE_START);
                Event* e2 = new Event(p1, edge, EVENT_TYPE_STOP);
                list.insert(e1);
                list.insert(e2);
            }
        }
    }

    void AddToEventList(const sfa::Polygon& poly, EventList& list, int height)
    {
        if (poly.getExteriorRing())
            AddToEventList(*poly.getExteriorRing(), list, height);
        for (int i=0; i<poly.getNumInteriorRing(); i++)
            AddToEventList(*poly.getInteriorRingN(i), list, height);
    }

    typedef unsigned char u_char;

    void rasterPolygonFill(const sfa::Polygon& poly, u_char *src, u_char *dest, int height, int width, int depth, unsigned char fixed_value)
    {
        ccl::ObjLog log("rasterPolygonFill");
        EventList    eventList;
        SweepLine    sweepLine;

        AddToEventList(poly, eventList, height);
    
        int currentRasterLine = 0;

        for (EventList::iterator nextEvent = eventList.begin(); nextEvent != eventList.end(); nextEvent++)
        {
            if (currentRasterLine >= height) break;

            double    y = (*nextEvent)->p->Y();
            int        yi = std::min<int>((int)(0.5+y), height);

            int        writeStart;
            bool    write;

            for (; currentRasterLine<yi; currentRasterLine++)
            {
                y = currentRasterLine + 0.5;
                writeStart = 0;
                write = false;
                for (SweepLine::iterator nextEdge = sweepLine.begin(); nextEdge != sweepLine.end(); nextEdge++)
                {
                    if (write)
                    {
                        write = false;
                        int writeStop = (int)(0.5 + InterpolateEdge(*nextEdge, y));
                        if (writeStart < 0)                writeStart = 0;
                        else if (writeStart > width-1)    writeStart = width-1;
                        if (writeStop < 0)                writeStop = 0;
                        else if (writeStop > width-1)    writeStop = width-1;
                        if (writeStart != writeStop)
                        {
                            int idx = (currentRasterLine*width + writeStart)*depth;
                            int len = (writeStop - writeStart +1)*depth;

                            if (len>0)
                            {
                                if (src)
                                    memcpy(&dest[idx], &src[idx], len);
                                else
                                    memset(&dest[idx], fixed_value, len);
                            }
                        }
                    }
                    else
                    {
                        write = true;
                        writeStart = (int)(0.5 + InterpolateEdge(*nextEdge, y));
                    }
                }
            }

            if ((*nextEvent)->type == EVENT_TYPE_START)
                sweepLine.insert((*nextEvent)->e);
            else
                sweepLine.erase((*nextEvent)->e);
        }

        for (EventList::iterator nextEvent = eventList.begin(); nextEvent != eventList.end(); nextEvent++)
        {
            if ((*nextEvent)->type == EVENT_TYPE_STOP)
                delete (*nextEvent)->e;
            delete *nextEvent;
        }
    }

}