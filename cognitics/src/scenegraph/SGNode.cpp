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

#include "scenegraph/Node.h"
#include "scenegraph/Relationship.h"
#include <boost/foreach.hpp>
#include <set>
#include <sstream>

namespace scenegraph
{
    Node::~Node(void)
    {
        for(size_t i = 0, c = relationships.size(); i < c; ++i)
            delete relationships[i];
    }

    Node::Node(void)
    {
    }
/*
    Node::Node(const Node &in)
    {
        this->attributes = in.attributes;
        BOOST_FOREACH(Relationship *rel,this->relationships)
        {
            //TODO:.....


        }

    }

    Node *clone()
    {
        Node *newnode = new Node(*this);
        return newnode;
    }
*/
    std::string Node::gv(void)
    {
        std::string name = getName();
        std::string result;
        for(size_t i = 0, c = relationships.size(); i < c; ++i)
        {
            Relationship *r = relationships.at(i);
            std::string relationshipName = r->getName();
            std::stringstream relationshipText;
            relationshipText.precision(2);
            relationshipText << std::fixed;
            if(r->confidence < 1.0f)
                relationshipText << ":C:" << r->confidence;
            if(r->rating != 0.0f)
                relationshipText << ":R:" << r->rating;
            result += "\"" + name + "\" -> \"" + r->node->getName() + "\"";
            if(!relationshipName.empty())
                result += " [label=\"" + relationshipName + relationshipText.str() + "\"]";
            result += "\n";
        }
        return result;
    }

    std::string Node::getName(void)
    {
        return "NODE";
    }

    size_t Node::getNumRelatedNodes(void)
    {
        std::set<Node *> nodes;
        for(size_t i = 0, c = relationships.size(); i < c; ++i)
        {
            Relationship *r = relationships.at(i);
            nodes.insert(r->node);
        }
        return nodes.size();
    }

}
