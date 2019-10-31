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

#include "scenegraph/Scene.h"
#include <map>
#include <fstream>
#include <boost/foreach.hpp>

namespace scenegraph
{
    Scene::~Scene()
    {
        for(size_t i = 0, c = nodes.size(); i < c; ++i)
            delete nodes[i];
        for (std::vector<Scene *>::iterator it = children.begin(), end = children.end(); it != end; it++)
            delete *it;

        if(userData)
            delete userData;
    }

    Scene::Scene(Scene *p) : parent(p), userData(NULL), hasVertexNormals(false), sharpEdges(false)
    {
        faces.reserve(10000);
        if(p)
            p->children.push_back(this);
    }

    void Scene::addChild(Scene *child)
    {
        children.push_back(child);
        child->parent = this;
    }

    Scene::Scene(const Scene& scene)  : userData(NULL), hasVertexNormals(false), sharpEdges(false)
    {
        *this = scene;
    }

    std::string Scene::type(void) const
    {
        return "Scene";
    }

    Scene *Scene::copy(void) const
    {
        Scene *scene = new Scene;
        copyTo(scene);
        return scene;
    }

    void Scene::copyTo(Scene *scene) const
    {
        if(userData)
            scene->userData = userData->Clone();
        scene->faces = faces;
        scene->sectionFaceIndices = sectionFaceIndices;
        scene->hasVertexNormals = hasVertexNormals;
        scene->sharpEdges = sharpEdges;
        scene->lineStrings = lineStrings;
        scene->lightPoints = lightPoints;
        scene->externalReferences = externalReferences;
        //std::vector<Node *> nodes;
        scene->matrix = matrix;
        scene->attributes = attributes;
        scene->parent = parent;
        scene->faceTextures = faceTextures;
        scene->faceMaterials = faceMaterials;
        scene->footprints = footprints;
        scene->terrainHoles = terrainHoles;
        scene->parent = NULL;
        scene->ID = ID;
        scene->name = name;
        scene->fltobject = fltobject;
        for(size_t i = 0, c = children.size(); i < c; i++)
        {
            Scene *child = children[i]->copy();
            child->parent = scene;
            //scene->children.push_back(child);
            scene->addChild(child);
        }
        //make sure you add to = operator also
    }

    void Scene::visit(Visitor *visitor)
    {
        visitor->visit(this);
    }
    

    // Incomplete! nodes should be owned by the scene, this does a shallow copy.
    // Use CopyFacesFrom instead.
    Scene &Scene::operator=(Scene const &in)
    {
        if(in.userData)
        {
            this->userData = in.userData->Clone();
        }
        this->faces = in.faces;
        this->sectionFaceIndices = in.sectionFaceIndices;
        this->hasVertexNormals = in.hasVertexNormals;
        this->sharpEdges = in.sharpEdges;
        this->lineStrings = in.lineStrings;
        this->lightPoints = in.lightPoints;
        this->externalReferences = in.externalReferences;
        this->matrix = in.matrix;
        this->parent = in.parent;
        this->faceTextures = in.faceTextures;
        this->faceMaterials = in.faceMaterials;
        this->footprints = in.footprints;
        this->terrainHoles = in.terrainHoles;
        //this->origin = in.origin;
        //this->nodes = in.nodes;
        //this->bsp = in.bsp;
        //make sure you add to copyTo also
        return *this;
    }


    /*
    size_t Scene::getOrAddLOD(double min, double max)
    {
        for(LODList::iterator it = lods.begin(), end = lods.end(); it != end; ++it)
        {
            if((it->min == min) && (it->max == max))
                return it - lods.begin();
        }
        lods.push_back(LOD(min, max));
        return lods.size() - 1;
    }
    */

    MappedTextureList Scene::getUniqueTextures()
    {
        std::map<std::string, bool> found;
        MappedTextureList result;
        for(FaceList::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
        {
            for(MappedTextureList::iterator tit = it->textures.begin(), tend = it->textures.end(); tit != tend; ++tit)
            {
                if(found.find(tit->GetTextureName()) == found.end())
                {
                    found[tit->GetTextureName()] = true;
                    result.push_back(*tit);
                }
            }
        }
        return result;
    }

    MaterialList Scene::getUniqueMaterials()
    {
        MaterialList result;
        std::map<Material, bool> found;
        for(FaceList::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
        {
            for(MaterialList::iterator mit = it->materials.begin(), mend = it->materials.end(); mit != mend; ++mit)
            {
                if(found.find(*mit) == found.end())
                {
                    found[*mit] = true;
                    result.push_back(*mit);
                }
            }
        }
        return result;
    }

    void Scene::getUniqueColors(std::set<Color> &colors)
    {
        for(FaceList::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
        {
            colors.insert(it->primaryColor);
            colors.insert(it->alternateColor);
        }
        for(size_t i = 0, c = children.size(); i < c; ++i)
            children[i]->getUniqueColors(colors);
    }

    void Scene::getUniqueTextures(std::set<std::string> &textures)
    {
        for(FaceList::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
        {
            for(MappedTextureList::iterator tit = it->textures.begin(), tend = it->textures.end(); tit != tend; ++tit)
                textures.insert(tit->GetTextureName());
        }
        for(size_t i = 0, c = children.size(); i < c; ++i)
            children[i]->getUniqueTextures(textures);
    }

    void Scene::getUniqueMaterials(std::set<Material> &materials)
    {
        for(FaceList::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
        {
            for(MaterialList::iterator mit = it->materials.begin(), mend = it->materials.end(); mit != mend; ++mit)
                materials.insert(*mit);
        }
        for(size_t i = 0, c = children.size(); i < c; ++i)
            children[i]->getUniqueMaterials(materials);
    }

    void Scene::setVertexNormals()
    {        
        if(sharpEdges)
        {
            // for sharp edges, we want all vertex normals to be pointing the same direction as the face normal
            for(size_t i = 0, c = faces.size(); i < c; ++i)
            {
                scenegraph::Face &face = faces.at(i);
                sfa::Point faceNormal = face.computeNormal();
                for(size_t vi = 0, vc = face.getNumVertices(); vi < vc; ++vi)
                    face.setNormalN(vi, faceNormal);
            }
            hasVertexNormals = true;
            for(size_t i = 0, c = children.size(); i < c; ++i)
                children[i]->setVertexNormals();
            return;
        }

        // Collect all the unique vertices
        std::set<sfa::Point> uniqueVerts;
        BOOST_FOREACH(const scenegraph::Face &face, faces)
        {
            int numVerts = face.getNumVertices();
            for(int i=0;i<numVerts;i++)
            {
                uniqueVerts.insert(face.getVertN(i));
            }
        }
        sortFaces();
        //For each unique vertex, find all faces that touch it.
        BOOST_FOREACH(const sfa::Point &pt, uniqueVerts)
        {
            sfa::Point normal;
            std::vector<scenegraph::Face*> touchingFaces;
            std::vector<int> indices;
            bsp.findFacesWithVert(pt, indices);
            BOOST_FOREACH(int idx,indices)
            {
                scenegraph::Face &face = faces.at(idx);
                if(face.hasVertex(pt))
                {
                    touchingFaces.push_back(&face);
                }                
            }
            // Get the total area
            double total_area = 0;
            BOOST_FOREACH(scenegraph::Face *facep, touchingFaces)
            {
                total_area += facep->getArea();
            }
            BOOST_FOREACH(scenegraph::Face *facep, touchingFaces)
            {
                sfa::Point faceNormal = facep->computeNormal();
                // Each face contributes its share to the normal
                faceNormal *= (facep->getArea()/total_area);
                normal += faceNormal;
            }
            
            // Now update the vertex normal for the faces.
            BOOST_FOREACH(scenegraph::Face *facep, touchingFaces)
            {
                int numVerts = facep->getNumVertices();

                for(int i=0;i<numVerts;i++)
                {
                    sfa::Point facept = facep->getVertN(i);
                    if(facept==pt)
                    {
                        facep->setNormalN(i,normal);
                    }
                }
                
            }
        }
        hasVertexNormals = true;
        for(size_t i = 0, c = children.size(); i < c; ++i)
            children[i]->setVertexNormals();
    }

    bool Scene::sortFaces()
    {
        bsp.setFaces(&faces,true);
        bsp.sort();
        return true;
    }

    bool statesort(Face *a, Face *b)
    {
        std::string akey = a->getGroupAndTextureKey();
        std::string bkey = b->getGroupAndTextureKey();
        if(akey < bkey)
            return true;
        if(akey > bkey)
            return false;
        return a < b;
    }

    bool Scene::buildStateFaces(void)
    {
        stateMap.clear();
        if(this->faceTextures.size()<=1)
        {
            if (faces.size() > 0)
            {
                std::string key = faces[0].getGroupAndTextureKey();
                for (size_t i = 0, c = faces.size(); i < c; ++i)
                {
                    stateMap[key].push_back(&faces[i]);
                }
            }
        }
        else
        {
            for (size_t i = 0, c = faces.size(); i < c; ++i)
            {
                std::string key = faces[i].getGroupAndTextureKey();
                stateMap[key].push_back(&faces[i]);
            }
        }
        return true;
    }

    bool Scene::getExtents(double &minX, double &maxX, double &minY, double &maxY, double &minZ, double &maxZ)
    {
        minX = DBL_MAX;
        maxX = 0;
        minY = DBL_MAX;
        maxY = 0;
        minZ = DBL_MAX;
        maxZ = 0;
        if(faces.size()<1)
            return false;
        BOOST_FOREACH(const scenegraph::Face &face, faces)
        {
            sfa::Point minPt,maxPt;
            face.getBoundingBox(minPt,maxPt);
            minX = std::min<double>(minX,minPt.X());
            maxX = std::max<double>(maxX,maxPt.X());
            minY = std::min<double>(minX,minPt.Y());
            maxY = std::max<double>(maxX,maxPt.Y());
            minZ = std::min<double>(minZ,minPt.Z());
            maxZ = std::max<double>(maxZ,maxPt.Z());
        }
        return true;
    }
    
    std::string Scene::gv(void)
    {
        std::string result = "digraph Scene {\n";
        for(size_t i = 0, c = nodes.size(); i < c; ++i)
        {
            result += "\"" + nodes[i]->getName() + "\"\n";
            result += nodes[i]->gv();
        }
        result += "}\n";
        return result;
    }

    bool Scene::gv(const std::string &filename)
    {
        std::remove(filename.c_str());
        std::ofstream of(filename.c_str());
        of << gv();
        of.close();
        return true;
    }

    int Scene::treeSize(bool counting) const
    {
        if (counting)
        {
            int size = 1;
            for (int i = 0, n = int(children.size()); i < n; i++)
            {
                Scene *group = dynamic_cast<Scene *>(children[i]);
                if(group)
                    size += group->treeSize(true);
                else
                    ++size;
            }
            return size;
        }
        else if (parent)
            return parent->treeSize(false);
        else
            return treeSize(true);
    }
}
