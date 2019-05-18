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
#include "scenegraph/SceneCropper.h"
#include "scenegraph/MappedTextureMatrix.h"

//#pragma optimize( "", off )

namespace scenegraph {

//!    Append points to a list while ensuring no duplicate points are added
    static void appendCropPoint(std::vector<sfa::Point>& points, const sfa::Point& p)
    {
        if (points.empty())
            points.push_back(p);
        else if (!points.back().equals3D(&p) && !points.front().equals3D(&p))
            points.push_back(p);
    }

//!    Crop a single point using parametric clipping (Liang-Barsky)
    static bool cropTest
        (
        double num,
        double denom,
        double& tEnter,
        double& tLeave
        )
    {
        if (denom == 0)
            return num < 0;

        double t = num/denom;

        if (denom > 0)
        {
            if (t > tLeave)
                return false;
            if (t > tEnter)
                tEnter = t;
        }
        else
        {
            if (t < tEnter)
                return false;
            if (t < tLeave)
                tLeave = t;
        }

        return true;
    }

//!    Clip away any portion of the line that is outside the clipping window.
//!    Uses parametric clipping (Liang-Barsky)
    static void cropLine
        (
        const sfa::Point& p1, 
        const sfa::Point& p2, 
        double xmin,
        double xmax, 
        double ymin,
        double ymax,
        std::vector<sfa::Point>& output
        )
    {
        double tEnter = 0;
        double tLeave = 1;
        sfa::Point d = p2 - p1;

        //Collapsed Line
        if ((d.X() == 0) && (d.Y() == 0) && (d.Z() == 0))
            return;

        if((d.X() == 0) && (d.Y() == 0))
        {
            if(p1.X() < xmin)
                return;
            if(p1.X() > xmax)
                return;
            if(p1.Y() < ymin)
                return;
            if(p1.Y() > ymax)
                return;
            appendCropPoint(output, p1);
            appendCropPoint(output, p2);
            return;
        }

        //Crop
        if(!cropTest(xmin - p1.X(), d.X(), tEnter, tLeave))
            return;
        if(!cropTest(p1.X() - xmax, -d.X(), tEnter, tLeave))
            return;
        if(!cropTest(ymin - p1.Y(), d.Y(), tEnter, tLeave))
            return;
        if(!cropTest(p1.Y() - ymax, -d.Y(), tEnter, tLeave))
            return;
        appendCropPoint(output, p1 + d*tEnter);
        appendCropPoint(output, p1 + d*tLeave);
    }

//!    Clip away any portion of a line that is to the left of the line p3->p4
    static void cropLine
        (
        const sfa::Point& p1,
        const sfa::Point& p2,
        const sfa::Point& p3,
        const sfa::Point& p4,
        std::vector<sfa::Point>& output
        )
    {
        sfa::Point p21 = p2 - p1;
        sfa::Point p31 = p3 - p1;
        sfa::Point p43 = p4 - p3;

        double denom = p43.X()*p21.Y() - p21.X()*p43.Y();

        if (abs(denom) < 1e-9)
        {
            appendCropPoint(output, p1);
            appendCropPoint(output, p2);
        }
        else
        {
            bool p1Inside = (p1 - p3).cross(p43).Z() > 0;
            bool p2Inside = (p2 - p3).cross(p43).Z() > 0;
            double s = (p43.X()*p31.Y() - p31.X()*p43.Y()) / denom;

            if (p1Inside)
                appendCropPoint(output, p1);

            if (s > 0 && s < 1)
                appendCropPoint(output, p1 + p21*s);

            if (p2Inside)
                appendCropPoint(output, p2);
        }
    }

//!    Copy textures from a parent face into all children faces
    static void copyTextures(const Face& parent, FaceList& faces)
    {
        for (size_t i=0; i<parent.textures.size(); i++)
        {
            //Create a working copy of the texture
            MappedTexture texture = parent.textures[i];
            
            //Create a transformation matrix from real to texture coordinates
            sfa::Matrix matrix;
            if (!CreateMappedTextureMatrix(parent, texture, matrix))
                continue;

            //Apply the texture to all new faces
            for (size_t j=0; j<faces.size(); j++)
            {
                Face& face = faces[j];
                texture.uvs.clear();
                for (size_t k=0; k<face.verts.size(); k++)
                    texture.uvs.push_back(matrix*face.verts[k]);
                face.textures.push_back(texture);
                face.smc = parent.smc;
                face.featureID = parent.featureID;
                face.materials = parent.materials;
                face.alternateColor = parent.alternateColor;
                face.primaryColor = parent.primaryColor;
                face.groupName = parent.groupName;
                face.groupID = parent.groupID;
                face.attributes = parent.attributes;
                face.legacyComment = parent.legacyComment;
            }
        }
    }

//!    Clip all the triangles in a scene to a clipping window
    Scene* cropScene2 ( const Scene* scene, double xmin, double xmax, double ymin, double ymax, const sfa::Matrix &mat )
    {
        //Check for valid scene
        if (!scene)
            return NULL;

        // transform points if the scene has a transform
        sfa::Matrix m = mat;
        m.invert();
        sfa::Point minpt(xmin, ymin);
        sfa::Point maxpt(xmax, ymax);
        minpt = m * minpt;
        maxpt = m * maxpt;
        xmin = minpt.X();
        ymin = minpt.Y();
        xmax = maxpt.X();
        ymax = maxpt.Y();

        //Check for valid bounding box
        // if not valid, return a copy of the scene rather than NULL
        // this is to protect from a matrix combined with a DBL_MAX bounding box, which creates a #INF
        if (xmin >= xmax || ymin >= ymax)
            return scene->copy();

        Scene* newScene = new Scene();
        newScene->matrix = scene->matrix;

        //Crop each face
        for (size_t i=0; i<scene->faces.size(); i++)
        {
            const Face& face = scene->faces[i];

            //Check for invalid/empty Face
            if (face.verts.size() < 3)
                continue;

            //Clip Face
            std::vector<sfa::Point> points;
            for (size_t j = 0, n = face.verts.size(); j < n; j++)
                cropLine(face.verts[j], face.verts[(j+1)%n], xmin, xmax, ymin, ymax, points);

            //Check if triangle was completely clipped
            if (points.size() < 3)
                continue;

            //Generate new Faces
            FaceList newFaces;
            if (face.verts.size() == 3)
            {
                //Use triangle faces
                for (size_t j=2; j<points.size(); j++)
                {
                    Face newFace;
                    newFace.verts.push_back(points[0]);
                    newFace.verts.push_back(points[j-1]);
                    newFace.verts.push_back(points[j]);
                    newFaces.push_back(newFace);
                    newFace.smc = face.smc;
                    newFace.featureID = face.featureID;
                }
            }
            else
            {
                Face newFace;
                newFace.smc = face.smc;
                newFace.featureID = face.featureID;
                newFace.verts = points;
                newFaces.push_back(newFace);
            }

            //Copy textures to new faces
            copyTextures(face, newFaces);

            //Add new faces to new scene
            newScene->faces.insert(newScene->faces.begin(), newFaces.begin(), newFaces.end());
        }

        return newScene;
    }

    Scene* cropScene( const Scene* scene, double xmin, double xmax, double ymin, double ymax, const sfa::Matrix &m )
    {
        // the liang-barsky algorithm is a line intersection test... it can't generate a point that isn't on the line
        // so, we need to crop by axis
        scenegraph::Scene *tmp = cropScene2(scene, xmin, xmax, -DBL_MAX, DBL_MAX, m);
        scenegraph::Scene *result = cropScene2(tmp, -DBL_MAX, DBL_MAX, ymin, ymax, m);
        delete tmp;

        // the "correct" solution would be to carry the normals over from the original scene
        // for now, we set the original normals this way, so we'll just update it
        // since we are weighting the adjacent vertex normals between two faces, this will change the weighting... the lighting effect will be slightly different
        result->setVertexNormals();

        return result;
    }


//!    Clip all the triangles in a scene to the right of a line p1->p2
    Scene* cropScene ( const Scene* scene, const sfa::Point& p1, const sfa::Point& p2 )
    {
        //Check for valid scene
        if (!scene)
            return NULL;

        //Check for valid bounding box
        if (p1 == p2)
            return NULL;

        Scene* newScene = new Scene();

        //Crop each face
        for (size_t i=0; i<scene->faces.size(); i++)
        {
            const Face& face = scene->faces[i];

            //Check for invalid/empty Face
            if (face.verts.size() < 3)
                continue;

            //Clip Face
            std::vector<sfa::Point> points;
            for (size_t j = 0, n = face.verts.size(); j < n; j++)
                cropLine(face.verts[j], face.verts[(j+1)%n], p1, p2, points);

            //Check if triangle was completely clipped
            if (points.size() < 3)
                continue;

            //Generate new Faces
            FaceList newFaces;
            if (face.verts.size() == 3)
            {
                //Use triangle faces
                for (size_t j=2; j<points.size(); j++)
                {
                    Face newFace;
                    newFace.verts.push_back(points[0]);
                    newFace.verts.push_back(points[j-1]);
                    newFace.verts.push_back(points[j]);
                    newFaces.push_back(newFace);
                }
            }
            else
            {
                Face newFace;
                newFace.verts = points;
                newFaces.push_back(newFace);
            }

            //Copy textures to new faces
            copyTextures(face, newFaces);

            //Add new faces to new scene
            newScene->faces.insert(newScene->faces.begin(), newFaces.begin(), newFaces.end());
        }

        // the "correct" solution would be to carry the normals over from the original scene
        // for now, we set the original normals this way, so we'll just update it
        // since we are weighting the adjacent vertex normals between two faces, this will change the weighting... the lighting effect will be slightly different
        newScene->setVertexNormals();

        return newScene;
    }

}