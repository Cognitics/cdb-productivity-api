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
#include "sfa/EdgeNodeEnd.h"
#include "sfa/PointMath.h"

namespace sfa {

    EdgeNodeEnd::EdgeNodeEnd(EdgeNode* e, bool away, bool saveZ)
    {
    //    This allows split edges to account for their original parent...this is crucial for proper BOUNDARY rules
    //    If this wasn't implemented, then if an edge where to split, the BOUNDARY nodes might cancel each other out improperly.
        parent = e;
        this->away = away;
        this->saveZ = saveZ;
        inResult = false;
        visited = false;
        next = NULL;
        sym = NULL;

        if (away)
        {
            point = e->getStartPoint();
            dx = e->getPointN(1)->getPoint()->X() - e->getPointN(0)->getPoint()->X();
            dy = e->getPointN(1)->getPoint()->Y() - e->getPointN(0)->getPoint()->Y();    
            right = e->getRight();
            left = e->getLeft();
        }
        else
        {
            point = e->getEndPoint();
            dx = e->getPointN(e->getNumPoints()-2)->getPoint()->X() - e->getEndPoint()->getPoint()->X();
            dy = e->getPointN(e->getNumPoints()-2)->getPoint()->Y() - e->getEndPoint()->getPoint()->Y();
            right = e->getLeft();
            left = e->getRight();
        }

        on = e->getOn();

        //    Determine quadrant
        if ( dy > 0 && dx >= 0 ) Quadrant = 1;
        else if ( dy <= 0 && dx > 0 ) Quadrant = 2;
        else if ( dy < 0 && dx <= 0 ) Quadrant = 3;
        else Quadrant = 4;
    }

    EdgeNode* EdgeNodeEnd::getParent(void) const
    {
        return parent;
    }

    EdgeNode* EdgeNodeEnd::getRootParent(void) const
    {
        if( parent->getParent() )
            return parent->getParent();
        else
            return parent;
    }

    Label EdgeNodeEnd::getLeft(void) const
    {
        return left;
    }

    Location EdgeNodeEnd::getLeft(int n) const
    {
        return left.loc[n];
    }

    void EdgeNodeEnd::setLeft(int n, Location l)
    {
        left.loc[n] = l;
    }

    void EdgeNodeEnd::setLeft(Label label)
    {
        left = label;
    }

    Label EdgeNodeEnd::getOn(void) const
    {
        return on;
    }

    Location EdgeNodeEnd::getOn(int n) const
    {
        return on.loc[n];
    }

    void EdgeNodeEnd::setOn(int n, Location l)
    {
        on.loc[n] = l;
    }

    void EdgeNodeEnd::setOn(Label label)
    {
        on = label;
    }

    Label EdgeNodeEnd::getRight(void) const
    {
        return right;
    }

    Location EdgeNodeEnd::getRight(int n) const
    {
        return right.loc[n];
    }

    void EdgeNodeEnd::setRight(int n, Location l)
    {
        right.loc[n] = l;
    }

    void EdgeNodeEnd::setRight(Label label)
    {
        right = label;
    }

    PointNode* EdgeNodeEnd::getOrigin(void) const
    {
        return point;
    }

    void EdgeNodeEnd::setOriginLabel(const Label& label)
    {
        getOrigin()->setLabel(label);
    }

    double EdgeNodeEnd::getDx(void) const
    {
        return dx;
    }

    double EdgeNodeEnd::getDy(void) const
    {
        return dy;
    }

    void EdgeNodeEnd::setNext(EdgeNodeEnd* next)
    {
        this->next = next;
    }

    EdgeNodeEnd* EdgeNodeEnd::getNext(void) const
    {
        return next;
    }

    bool EdgeNodeEnd::isInResult(void) const
    {
        return inResult;
    }

    void EdgeNodeEnd::setInResult(bool flag)
    {
        inResult = flag;
    }

    bool EdgeNodeEnd::isVisited(void) const
    {
        return visited;
    }

    void EdgeNodeEnd::setVisited(bool flag)
    {
        visited = flag;
    }

    void EdgeNodeEnd::visit(void)
    {
        visited = true;
    }

    EdgeNodeEnd* EdgeNodeEnd::getSym(void) const
    {
        return sym;
    }

    void EdgeNodeEnd::setSym(EdgeNodeEnd* edge)
    {
        sym = edge;
    }

    bool EdgeNodeEnd::isLine(void) const
    {
        bool line0 = left.loc[0] == right.loc[0];
        bool line1 = left.loc[1] == right.loc[1];
        return line0 || line1;
    }

    bool EdgeNodeEnd::isArea(void) const
    {
        bool area0 = left.loc[0] != right.loc[0];
        bool area1 = left.loc[1] != right.loc[1];
        return area0 || area1;
    }

    bool EdgeNodeEnd::equals(const EdgeNodeEnd* other) const
    {
        return parent->equals(other->parent);
    }

    int EdgeNodeEnd::compare(const EdgeNodeEnd* other) const
    {
        if (Quadrant < other->Quadrant) return -1;
        else if (Quadrant > other->Quadrant) return 1;
        else
        {
            return CrossProduct(Point(0,0), Point(dx, dy), Point(0,0), Point(other->dx, other->dy)); 
            /*
            double cross = dx*other->dy - dy*other->dx;
            if (cross > SFA_EPSILON) return 1;
            else if (cross < -SFA_EPSILON) return -1;
            else return 0; 
            */
        }
    }

    void EdgeNodeEnd::appendToLineString(LineString* line) const
    {
        if (away)
        {
            for (int i = 1; i < parent->getNumPoints(); i++)
            {
                line->addPoint(Point(parent->getPointN(i)->getPoint()));
                if(!saveZ)
                    line->getEndPoint()->setZ(0);
            }

        }
        else
        {
            for (int i = parent->getNumPoints() - 2; i >= 0; i--)
            {
                line->addPoint(Point(parent->getPointN(i)->getPoint()));
                if(!saveZ)
                    line->getEndPoint()->setZ(0);
            }
        }
    }

    void EdgeNodeEnd::setLabels(const Label& left, const Label& on, const Label& right)
    {
    //    Replace the values of this and its parent with the given values, if the given values are not UNKNOWN
        this->left.loc[0] = (left.loc[0] != UNKNOWN ? left.loc[0] : this->left.loc[0]);
        this->left.loc[1] = (left.loc[1] != UNKNOWN ? left.loc[1] : this->left.loc[1]);
        this->on.loc[0] = (on.loc[0] != UNKNOWN ? on.loc[0] : this->on.loc[0]);
        this->on.loc[1] = (on.loc[1] != UNKNOWN ? on.loc[1] : this->on.loc[1]);
        this->right.loc[0] = (right.loc[0] != UNKNOWN ? right.loc[0] : this->right.loc[0]);
        this->right.loc[1] = (right.loc[1] != UNKNOWN ? right.loc[1] : this->right.loc[1]);
    }

    void EdgeNodeEnd::updateIM(de9im* matrix) const
    {
        matrix->setAtLeast(left);
        matrix->setAtLeast(on);
        matrix->setAtLeast(right);
    }

    void EdgeNodeEnd::updateIM(de9im& matrix) const
    {
        matrix.setAtLeast(left);
        matrix.setAtLeast(on);
        matrix.setAtLeast(right);
    }

}