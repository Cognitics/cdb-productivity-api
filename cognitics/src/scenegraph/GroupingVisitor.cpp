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

#include "scenegraph/GroupingVisitor.h"
#include "scenegraph/LOD.h"

namespace scenegraph
{
    GroupingVisitor::~GroupingVisitor(void)
    {
    }

    GroupingVisitor::GroupingVisitor(void) : resultScene(NULL)
    {
    }

    bool GroupingVisitor::splitTextures(Scene *scene)
    {
        // get the unique texture list
        std::map<Face *, std::string> textureNameMap;
        for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
        {
            Face &face = scene->faces.at(i);
            std::string textureName;
            for(size_t j = 0, jc = face.textures.size(); j < jc; ++j)
            {
                if(j > 0)
                    textureName += ";";
                textureName += face.textures[j].GetTextureName();
            }
            textureNameMap[&face] = textureName;
        }

        // count the uniques and bail if we don't need to split
        std::set<std::string> textureNames;
        for(std::map<Face *, std::string>::iterator it = textureNameMap.begin(), end = textureNameMap.end(); it != end; ++it)
            textureNames.insert(it->second);
        scene->faceTextures.clear();
        scene->faceTextures.insert(scene->faceTextures.end(), textureNames.begin(), textureNames.end());
        if(textureNames.size() < 2)
            return false;

        // set up a map of new scenes based on the texture list
        std::map<std::string, Scene *> textureSceneMap;
        for(std::set<std::string>::iterator it = textureNames.begin(), end = textureNames.end(); it != end; ++it)
        {
            Scene *child = new Scene;
            child->faceTextures.push_back(*it);
            textureSceneMap[*it] = child;
        }

        // add faces to child scenes and remove from current scene
        for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
        {
            Face &face = scene->faces.at(i);
            Scene *child = textureSceneMap[textureNameMap[&face]];
            child->faces.push_back(face);
        }
        scene->faces.clear();

        // convert to a group if necessary (and update parent)
        Scene *sceneGroup = dynamic_cast<Scene *>(scene);
        if(!sceneGroup)
        {
            sceneGroup = (scene->type() == "LOD") ? new LOD : new Scene;
            scene->copyTo(sceneGroup);
            if(scene->parent)
            {
                for(size_t i = 0, c = scene->parent->children.size(); i < c; ++i)
                {
                    if(scene->parent->children[i] == scene)
                        scene->parent->children[i] = sceneGroup;
                }
            }
            delete scene;
            scene = NULL;
        }

        // add children
        for(std::map<std::string, Scene *>::iterator it = textureSceneMap.begin(), end = textureSceneMap.end(); it != end; ++it)
        {
            sceneGroup->children.push_back(it->second);
            it->second->parent = sceneGroup;
        }

        traverse(sceneGroup);
        return true;
    }

    bool GroupingVisitor::splitMaterials(Scene *scene)
    {
        // get the unique material list
        std::map<Face *, std::set<Material> > materialSetMap;
        for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
        {
            Face &face = scene->faces.at(i);
            std::set<Material> materials;
            for(size_t j = 0, jc = face.materials.size(); j < jc; ++j)
                materials.insert(face.materials.at(j));
            materialSetMap[&face] = materials;
        }

        // count the uniques and bail if we don't need to split
        std::set<std::set<Material> > materialSets;
        for(std::map<Face *, std::set<Material> >::iterator it = materialSetMap.begin(), end = materialSetMap.end(); it != end; ++it)
            materialSets.insert(it->second);
        if(materialSets.size() < 2)
        {
            if(materialSets.size() > 0)
            {
                scene->faceMaterials.clear();
                scene->faceMaterials.insert(scene->faceMaterials.end(), materialSets.begin()->begin(), materialSets.begin()->end());
            }
            return false;
        }

        // set up a map of new scenes based on the material list
        std::map<std::set<Material>, Scene *> materialSceneMap;
        for(std::set<std::set<Material> >::iterator it = materialSets.begin(), end = materialSets.end(); it != end; ++it)
        {
            Scene *child = new Scene;
            for(std::set<Material>::iterator itm = it->begin(), endm = it->end(); itm != endm; ++itm)
                child->faceMaterials.push_back(*itm);
            materialSceneMap[*it] = child;
        }

        // add faces to child scenes and remove from current scene
        for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
        {
            Face &face = scene->faces.at(i);
            Scene *child = materialSceneMap[materialSetMap[&face]];
            child->faces.push_back(face);
        }
        scene->faces.clear();

        // convert to a group if necessary (and update parent)
        Scene *sceneGroup = dynamic_cast<Scene *>(scene);
        if(!sceneGroup)
        {
            sceneGroup = (scene->type() == "LOD") ? new LOD : new Scene;
            scene->copyTo(sceneGroup);
            if(scene->parent)
            {
                for(size_t i = 0, c = scene->parent->children.size(); i < c; ++i)
                {
                    if(scene->parent->children[i] == scene)
                        scene->parent->children[i] = sceneGroup;
                }
            }
            delete scene;
            scene = NULL;
        }

        // add children
        for(std::map<std::set<Material>, Scene *>::iterator it = materialSceneMap.begin(), end = materialSceneMap.end(); it != end; ++it)
        {
            sceneGroup->children.push_back(it->second);
            it->second->parent = sceneGroup;
        }

        traverse(sceneGroup);
        return true;
    }

    bool GroupingVisitor::splitAttributes(Scene *scene)
    {
        // get the unique attribute set
        std::map<Face *, ccl::VariantMap> attributeSetMap;
        for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
        {
            Face &face = scene->faces.at(i);
            attributeSetMap[&face] = *(face.attributes.getVariantMap());
        }

        // count the uniques and bail if we don't need to split
        std::set<ccl::VariantMap> attributeSets;
        for(std::map<Face *, ccl::VariantMap>::iterator it = attributeSetMap.begin(), end = attributeSetMap.end(); it != end; ++it)
            attributeSets.insert(it->second);
        if(attributeSets.size() < 2)
            return false;

        // set up a map of new scenes based on the attribute sets
        std::map<ccl::VariantMap, Scene *> attributeSceneMap;
        for(std::set<ccl::VariantMap>::iterator it = attributeSets.begin(), end = attributeSets.end(); it != end; ++it)
        {
            Scene *child = new Scene;
            *(child->attributes.getVariantMap()) = *it;
            child->attributes.updateLowerCaseMap();
            attributeSceneMap[*it] = child;
        }

        // add faces to child scenes and remove from current scene
        for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
        {
            Face &face = scene->faces.at(i);
            Scene *child = attributeSceneMap[attributeSetMap[&face]];
            child->faces.push_back(face);
        }
        scene->faces.clear();

        // convert to a group if necessary (and update parent)
        Scene *sceneGroup = dynamic_cast<Scene *>(scene);
        if(!sceneGroup)
        {
            sceneGroup = (scene->type() == "LOD") ? new LOD : new Scene;
            scene->copyTo(sceneGroup);
            if(scene->parent)
            {
                for(size_t i = 0, c = scene->parent->children.size(); i < c; ++i)
                {
                    if(scene->parent->children[i] == scene)
                        scene->parent->children[i] = sceneGroup;
                }
            }
            delete scene;
            scene = NULL;
        }

        // add children
        for(std::map<ccl::VariantMap, Scene *>::iterator it = attributeSceneMap.begin(), end = attributeSceneMap.end(); it != end; ++it)
        {
            sceneGroup->children.push_back(it->second);
            it->second->parent = sceneGroup;
        }

        traverse(sceneGroup);
        return true;
    }

    void GroupingVisitor::visiting(Scene *scene)
    {
        if(!scene)
            throw std::runtime_error("null scene");
        if(splitTextures(scene))
            return;
        if(splitMaterials(scene))
            return;
        if(splitAttributes(scene))
            return;
        traverse(scene);
    }

    Scene *GroupingVisitor::getResultScene(void)
    {
        return resultScene;
    }

    Scene *GroupingVisitor::group(Scene *scene)
    {
        if(resultScene)
            delete resultScene;
        resultScene = new Scene;
        scene->copyTo(resultScene);
        resultScene->name = "root";
        visit(resultScene);        // we are visiting the copy, not the original
        return getResultScene();
    }

}