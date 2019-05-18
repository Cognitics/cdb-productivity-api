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
#include "sfa/EdgeGroup.h"

#include <iostream>
#include <sstream>

namespace sfa {

    Label EdgeGroup::mergeLabels(Label a, Label b) const
    {
        //    Use standard label copy function to account for boundary rule
        if (a.dim == 1 || a.dim == 0)
        {
            a.copyLabel(b);
            return a;
        }
        else 
        {
            //    We now that the labels are Paritions. Override a with b
            a.loc[0] = (b.loc[0] == UNKNOWN) ? a.loc[0] : b.loc[0];
            a.loc[1] = (b.loc[1] == UNKNOWN) ? a.loc[1] : b.loc[1];
        }
        return a;
    }

    void EdgeGroup::updateCenter(const EdgeNodeEnd* e)
    {
        bool included = false;
        for (EdgeNodeList::iterator it = includedEdges.begin(); it != includedEdges.end(); ++it)
        {
            if (e->getRootParent()==(*it))
            {
                included = true;
                break;
            }
        }

        if (!included)
        {
            includedEdges.push_back(e->getRootParent());

        /*    This is a grey area, according to the specification, don't update the centerpoint label from Boundary EDGES
            only boundary points, this will prevent non-hmonogeoneous applications of the mod 2 rule on points*/
        //    Use the following for pure mod-2 application
            //    centerPoint->copyLocation(e->getCenter(),e->getParentEdge()->getGeomArg());

        //    Use the following for sfa OGC mod-2 application
            if (e->getOn(e->getRootParent()->getGeomArg()) != BOUNDARY)
                centerPoint->copyLocation(e->getOrigin(),e->getRootParent()->getGeomArg());
        }
    }

    EdgeGroup::EdgeGroup(PointNode* cp)
    {
    //    Don't use the actual center, only keep the center point's physical location
        Label unknown(0);
        centerPoint = new PointNode(cp->getPoint(),unknown,cp->getGeomArg());
    }

    EdgeGroup::EdgeGroup(const EdgeGroup& other)
    {
        this->includedEdges = other.includedEdges;
        this->E = other.E;
        this->Partitions = other.Partitions;
        this->Divisions = other.Divisions;
        this->centerPoint = new PointNode(other.getPoint());
    }

    EdgeGroup::~EdgeGroup(void)
    {
        delete centerPoint;
    }

    PointNode* EdgeGroup::getPoint(void) const
    {
        return centerPoint;
    }

    void EdgeGroup::insertEdge(EdgeNodeEnd* e)
    {
    //    Update the centerPoint's label
        updateCenter(e);

        //    If there is no current edge, add one
        if (E.size() == 0)
        {
            Label p1 = e->getRight();
            EdgeGroupDivision D;
            D.addEdge(e);
            E.push_back(e);
            Divisions.push_back(D);
            Partitions.push_back(p1);
            //set label
            centerPoint->setLabel(e->getOrigin()->getLabel());
            return;
        }

        int low, high, pivot, comp;
        low = 0;
        high = int(E.size()) - 1;
        pivot = (high - low)/2;

        while (low < high)
        {
            pivot = low + (high - low)/2;
            comp = e->compare(E[pivot]);
            if (comp == -1)
            {
                high = pivot-1;
                continue;
            }
            else if (comp == 1)
            {
                low = pivot+1;
                continue;
            }
            else
            {
                Divisions[pivot].addEdge(e);
                //    Update partitions
                //    Edge is already on the group
                int p1 = (pivot - 1 + int(Partitions.size()))%int(Partitions.size());
                Partitions[p1].copyLabel(e->getLeft());
                Partitions[pivot].copyLabel(e->getRight());

                return;
            }
        }

        int prev;

        comp = e->compare(E[low]);
        if (comp > 0)
        {
            prev = low;
            low++;
        }
        else if (comp < 0) prev = (low - 1 + int(Partitions.size()))%int(Partitions.size());
        else
        {
            Divisions[low].addEdge(e);
            int p1 = (low - 1 + int(Partitions.size()))%int(Partitions.size());
            Partitions[p1].copyLabel(e->getLeft());
            Partitions[low].copyLabel(e->getRight());

            return;
        }

        //    New division
        EdgeGroupDivision D;
        D.addEdge(e);

        //    Create new Pa\ritions P1 and P2 to replace Partitions[low];
        Label P1 = mergeLabels(Partitions[prev],e->getLeft());
        Label P2 = mergeLabels(Partitions[prev],e->getRight());

        //    Insert D into the divisions before high
        Divisions.insert(Divisions.begin()+low,D);

        //    Change the value of Partitions[pivot] and insert P2 before P[high]
        Partitions[prev] = P1;
        Partitions.insert(Partitions.begin()+low,P2);
        E.insert(E.begin()+low,e);
    }

    void EdgeGroup::LinkEdges(void)
    {
    //    This actually links all the symetric edges to these edges
    //    Create a new list of sorted Edges by pulling only the included edges into, then link CCW and CW
        EdgeNodeEndList nodes;
        for (int i = 0; i < int(Divisions.size()); i++)
            Divisions[i].getEdges(nodes);

        for (int i= 0; i < int(nodes.size()); i++)
        {
        //    We are actually linking symetric edges, so don't bother if its not included
            if (!nodes[i]->getSym()->isInResult()) continue;

        //Get next ring along the clockwise order, this is minimum ring link
            int min = -1;
            int j = (i+1)%int(nodes.size());
            while (j != i)
            {
                if (nodes[j]->isInResult())
                {
                    min = j;
                    break;
                }
                j = (j+1)%int(nodes.size());
            }

            if (min != -1) nodes[i]->getSym()->setNext(nodes[min]);
        }
    }

    void EdgeGroup::completeLabels(int n)
    {
        //    For each partition, if it is missing a piece of the label, look to the right for the next valid region
        for (int i = 0; i < int(Partitions.size()); i++)
        {
            if (Partitions[i].loc[n] == UNKNOWN)
            {
                bool labelFound = false;
                for (int j = i; j < int(Partitions.size()); j++)
                {
                    if (Partitions[j].loc[n] != UNKNOWN) 
                    {
                        labelFound = true;
                        Partitions[i].loc[n] = Partitions[j].loc[n];
                        break;
                    }
                }
                if (!labelFound)
                {
                    //    Continue to the left until a node is found
                    for (int j = i; j >= 0; j--)
                    {
                        if (Partitions[j].loc[n] != UNKNOWN) 
                        {
                            Partitions[i].loc[n] = Partitions[j].loc[n];
                            break;
                        }
                    }
                }
            }
        }

        /*    
            If at this point there was an unlabeled Partition, then this Group contains edges from only
            One Geometry and thus will not be able to label any part of the matrix properly...which should
            not be the case since this Node should have been constructed from Intersecion nodes created by 
            edges intersecting from a and b/
        */
        //    Continue for each Division
        for (int i = 0; i < int(Divisions.size()); i++)
        {
            //    Update with the location of the nearest partition to the right
            if (Divisions[i].getLocation(n) == UNKNOWN) Divisions[i].setLocation(n,Partitions[i].loc[n]);
        }

        //    Special case, if all partitions are INTERIOR, then the center point must be interior, but because of
        //    The way the mod two rule handles corners, we must leave its current label alone
        bool interior = true;
        for (int i = 0; i < int(Partitions.size()); i++)
        {
            if (Partitions[i].loc[n] != INTERIOR)
            {
                interior = false;
                break;
            }
        }
        if (interior) centerPoint->setLocation(n,INTERIOR);
    }

    void EdgeGroup::updateParents(void)
    {
        for (int i = 0; i < int(Divisions.size()); i++)
        {
            int l = (i-1 + int(Partitions.size())) % int(Partitions.size());
            int r = i;

            Divisions[i].updateParents(Partitions[l],Partitions[r],centerPoint->getLabel());
        }
    }

    void EdgeGroup::updateIM(de9im* matrix) const
    {
/*
//    DEBUG
//    print edges
        std::cout << "\nGroup:  ( " << centerPoint->getPoint()->X() << " " << centerPoint->getPoint()->Y() << " )\n";
        std::cout << "==================================================================================\n";
        for (int i = 0; i < int(E.size()); i++)
        {
            std::cout << E[i]->getDx() << " " << E[i]->getDy() << "\n";
            std::cout << "\tLeft (2): " << E[i]->getLeftLabel().print() << "\n";
            std::cout << "\tOn   (1): " << E[i]->getLabel().print() << "\n";
            std::cout << "\tRight(2): " << E[i]->getRightLabel().print() << "\n";
        }

        std::cout << "\nCenterPoint:\n" << centerPoint->getLabel().print() << "\n";

        std::cout << "\nPartitions before completion:\n";
        for (int i = 0; i < int(Partitions.size()); i++)
            std::cout << i << "\t" << Partitions[i].print() << "\n";
        
        std::cout << "\nDivisions before completion:\n";
        for (int i = 0; i < int(Divisions.size()); i++)
            std::cout << i << "\t" << Divisions[i].getLabel().print() << "\n";
//    END DEBUG
*/

        //completeLabels(0);
        //completeLabels(1);

/*
//    DEBUG
        std::cout << "\nPartitions after completion:\n";
        for (int i = 0; i < int(Partitions.size()); i++)
            std::cout << i << "\t" << Partitions[i].print() << "\n";
        
        std::cout << "\nDivisions after completion:\n";
        for (int i = 0; i < int(Divisions.size()); i++)
            std::cout << i << "\t" << Divisions[i].getLabel().print() << "\n";
//    END DEBUG
*/

        //    Update from center point
        matrix->setAtLeast(centerPoint->getLabel());

        //    Itterate through the partitions and divisions and update the matrix
        for (std::vector<Label>::const_iterator it = Partitions.begin(); it != Partitions.end(); ++it)
            matrix->setAtLeast(*it);

        for (std::vector<EdgeGroupDivision>::const_iterator it = Divisions.begin(); it != Divisions.end(); ++it)
            it->updateIM(matrix);

/*
//    DEBUG
        std::cout << "\nNewly update matrix: \n";
        std::cout << "\t" << matrix->getRowAsString(0) << "\n";
        std::cout << "\t" << matrix->getRowAsString(1) << "\n";
        std::cout << "\t" << matrix->getRowAsString(2) << "\n";
        int blank = 0;
//    END DEBUG
*/
    }

    void EdgeGroup::updateIM(de9im& matrix) const
    {
        matrix.setAtLeast(centerPoint->getLabel());

        for (std::vector<Label>::const_iterator it = Partitions.begin(); it != Partitions.end(); ++it)
            matrix.setAtLeast(*it);

        for (std::vector<EdgeGroupDivision>::const_iterator it = Divisions.begin(); it != Divisions.end(); ++it)
            it->updateIM(matrix);
    }

}