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
#include <scenegraph/Face.h>
#include <sfa/Polygon.h>
#include <string>

namespace scenegraph
{

    Face::Face(void) : userData(NULL), groupID(0), groupName("root"), clipped(false), area(0), transparency(0.0), drawBothSides(false), featureID(0), smc(0)
    {

    }

    Face::Face(const Face &in) : userData(NULL), area(0)
    {
        if(&in!=this)
            *this = in;
    }

    Face &Face::operator=(Face const &in)
    {
        userData = in.userData;
        vertexNormals = in.vertexNormals;
        verts = in.verts;
        textures = in.textures;
        materials = in.materials;
        groupID = in.groupID;
        clipped = in.clipped;
        groupName = in.groupName;
        area = in.area;
        attributes = in.attributes;
        primaryColor = in.primaryColor;
        alternateColor = in.alternateColor;
        featureID = in.featureID;
        smc = in.smc;
        transparency = in.transparency;
        id = in.id;
        drawBothSides = in.drawBothSides;
        legacyComment = in.legacyComment;
        return *this;
    }

    Face::~Face()
    {

    }
    

    void Face::SetFaceNormal(const sfa::Point &faceNormal)
    {
        sfa::Point norm(faceNormal);
        norm.normalize();
        int numPoints = int(verts.size());
        for(int i=0;i<numPoints;i++)
        {
            setNormalN(i,norm);
        }
    }
    /*
    void Face::SetP1Normal(const sfa::Point &normal)
    {
        if(!normals)
        {
            normals = new FaceNormals;
        }
        normals->p1Normal = normal;        
    }

    void Face::SetP3Normal(const sfa::Point &normal)
    {
        if(!normals)
        {
            normals = new FaceNormals;
        }
        normals->p3Normal = normal;        
    }

    void Face::SetP2Normal(const sfa::Point &normal)
    {
        if(!normals)
        {
            normals = new FaceNormals;
        }
        normals->p2Normal = normal;        
    }

    sfa::Point Face::GetP1Normal() const
    {
        if(!normals)
            return sfa::Point(0,0,0);
        else
            return normals->p1Normal;
    }
    sfa::Point Face::GetP2Normal() const
    {
        if(!normals)
            return sfa::Point(0,0,0);
        else
            return normals->p2Normal;
    }

    sfa::Point Face::GetP3Normal() const
    {
        if(!normals)
            return sfa::Point(0,0,0);
        else
            return normals->p3Normal;
    }

    
    */


    sfa::Point Face::GetFaceNormal() const
    {
        return computeNormal();
    }

    /*
     * Generates a key string of all the textures this face uses (textures sorted)
     * This method also includes materials and attributes (sorted) in the key
     */
    std::string Face::getGroupAndTextureKey() const
    {
        std::set<std::string> sorted_textures;

        size_t num_textures = textures.size();
        for(size_t i=0;i<num_textures;i++)
        {
            const MappedTexture &mt = textures.at(i);
            sorted_textures.insert(mt.GetTextureName());
        }

        std::stringstream ss;
        ss << int(verts.size());//add the number of verts to the output
        ss << groupName << ";";
        std::set<std::string>::iterator iter = sorted_textures.begin();
        while(iter!=sorted_textures.end())
        {
            ss << *iter++ << ";";
        }
        
        //Add the material key and pass it back as an output parameter.
        for (size_t mi = 0; mi < materials.size(); mi++)
            ss << materials[mi].toString() << ";";
        

        std::set<std::string> sorted_attributes;        
        std::vector<std::string> keys = attributes.getKeys();
        BOOST_FOREACH(std::string key,keys)
        {
            if(attributes.getAttributeAsBool(key))
            {
                sorted_attributes.insert(key);
            }
        }
        BOOST_FOREACH(std::string key, sorted_attributes)
        {
            ss << key << ";";
        }
        ss << verts.size() << ";";//keep triangles and quads separate

        return ss.str();

    }
    sfa::Point Face::computeNormal() const
    {
        if(verts.size()>2)
        {
            sfa::Point A = getVertN(1) - getVertN(0);
            sfa::Point B = getVertN(2) - getVertN(0);
            sfa::Point n = A.cross(B);
            n.normalize();
            return n;
        }
        return sfa::Point();
    }

    sfa::Point Face::getUVForVertex(const sfa::Point &newp, unsigned int tnum) const
    {
        sfa::Point newUV;
        if(verts.size()<3)
            return sfa::Point();
        
        sfa::Point p1 = verts.at(0);
        sfa::Point p2 = verts.at(1);
        sfa::Point p3 = verts.at(2);

        if(tnum>=textures.size())
            return newUV;
        const MappedTexture &mt = textures.at(tnum);
        if(mt.uvs.size()<3)
            return newUV;

        sfa::Point uvA = mt.uvs.at(0);
        sfa::Point uvB = mt.uvs.at(1);
        sfa::Point uvC = mt.uvs.at(2);

        if (newp == p1)
            return uvA;
        else if (newp == p2)
            return uvB;
        else if (newp == p3)
            return uvC;

        // Compute vectors 
        sfa::Point v0 = p3 - p1;
        sfa::Point v1 = p2 - p1;
        sfa::Point v2 = newp - p1;

        double dot00 = v0.dot(v0);
        double dot01 = v0.dot(v1);
        double dot02 = v0.dot(v2);
        double dot11 = v1.dot(v1);
        double dot12 = v1.dot(v2);

        // Compute UVs
        double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
        double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
                            
        sfa::Point t2 = uvB-uvA;
        sfa::Point t1 = uvC-uvA;

        newUV = uvA + t1*u + t2*v;

        return newUV;
    }


    void Face::setAttributes(const ccl::AttributeContainer &_attributes)
    {
        this->attributes = _attributes;
    }

    ccl::AttributeContainer &Face::getAttributes(void)
    {
        return attributes;        
    }

    const ccl::AttributeContainer &Face::getAttributes(void) const
    {
        return attributes;        
    }

    int Face::getNumVertices() const
    {
        return int(verts.size());
    }

    sfa::Point Face::getVertN(const int &n) const
    {
        if(int(verts.size())>n)
        {
            return verts.at(n);
        }
        else
        {
            return sfa::Point();
        }
    }

    bool Face::setVertN(const int &n,const sfa::Point &pt)
    {
        if(int(verts.size())>n)
        {
            verts[n] = pt;
            return true;
        }
        else
        {
            return false;
        }
    }

    int Face::addVert(const sfa::Point &pt)
    {
        verts.push_back(pt);
        return int(verts.size()-1);
    }

    MappedTexture Face::getMappedTextureN(const int &n) const
    {
        if(int(textures.size())>n)
        {
            return textures.at(n);
        }
        else
        {
            return MappedTexture();
        }
    }

    int Face::addMappedTexture(const MappedTexture &mt)
    {
        textures.push_back(mt);
        return int(textures.size());
    }

    bool Face::setMappedTextureN(const int &n, const MappedTexture &mt)
    {
        if(int(textures.size())>n)
        {
            textures[n] = mt;
            return true;
        }
        else
        {
            return false;
        }
    }

    sfa::Point Face::getNormalN(const int &n) const
    {
        if(int(vertexNormals.size())>n)
        {
            return vertexNormals.at(n);
        }
        else
        {
            return sfa::Point();
        }
    }

    void Face::setNormalN(const int &n, const sfa::Point &norm)
    {
        if(int(vertexNormals.size())<=n)
        {
            vertexNormals.resize(n+1);
        }
        vertexNormals[n] = norm;
    }

    void Face::getBoundingBox(double& minX, double& maxX, double& minY, double& maxY) const
    {
        int numPoints = int(verts.size());
        for(int i=0;i<numPoints;i++)
        {
            const sfa::Point &pt = verts.at(i);
            minX = std::min<double>(minX,pt.X());
            minY = std::min<double>(minY,pt.Y());
            maxX = std::max<double>(maxX,pt.X());
            maxY = std::max<double>(maxY,pt.Y());
        }
    }

    void Face::getBoundingBox(sfa::Point &minPt, sfa::Point &maxPt)  const
    {
        double minX = DBL_MAX;
        double minY = DBL_MAX;
        double minZ = DBL_MAX;
        double maxX = -DBL_MAX;
        double maxY = -DBL_MAX;
        double maxZ = -DBL_MAX;

        int numPoints = int(verts.size());
        for(int i=0;i<numPoints;i++)
        {
            const sfa::Point &pt = verts.at(i);
            minX = std::min<double>(minX,pt.X());
            minY = std::min<double>(minY,pt.Y());
            minZ = std::min<double>(minZ,pt.Z());
            maxX = std::max<double>(maxX,pt.X());
            maxY = std::max<double>(maxY,pt.Y());
            maxZ = std::max<double>(maxZ,pt.Z());
        }
        minPt.setX(minX);
        minPt.setY(minY);
        minPt.setZ(minZ);
        maxPt.setX(maxX);
        maxPt.setY(maxY);
        maxPt.setZ(maxZ);
    }

    bool Face::hasVertex(const sfa::Point &pt)
    {
        int numPoints = int(verts.size());
        for(int i=0;i<numPoints;i++)
        {
            sfa::Point &facept = verts.at(i);
            if(pt==facept)
                return true;
        }
        return false;
    }

    double Face::getArea()
    {
        if(area<=0)
        {
            sfa::Polygon poly;
            sfa::LineString ring;
            int numPoints = int(verts.size());
            for(int i=0;i<numPoints;i++)
            {
                ring.addPoint(verts.at(i));
            }
            ring.addPoint(verts.at(0));
            poly.addRing(ring);
            try
            {
                area = abs(poly.getArea3D());

            }
            catch(std::runtime_error e)
            {
                return 0;
            }
        }
        return area;
    }

    bool Face::interpolatePointInFace(sfa::Point &p) const
    {
        if(verts.size()<3)
            return false;
        sfa::Point p1 = verts.at(0);
        sfa::Point p2 = verts.at(1);
        sfa::Point p3 = verts.at(2);
        //TODO: make sure the 3 points are independent
        sfa::Point P = p - p1;
        sfa::Point U = p2 - p1;
        sfa::Point V = p3 - p1;

        double denom = V.X()*U.Y() - V.Y()*U.X();
        double v = ( P.X()*U.Y() - P.Y()*U.X() )/denom;
        double u = ( P.Y()*V.X() - P.X()*V.Y() )/denom;

        if (v < 0 || u < 0) 
            return false;
        else if (u + v > 1) 
            return false;
        else
        {
            p.setZ( p1.Z() + u*U.Z() + v*V.Z() );
            return true;
        }
    }

    void Face::RemapTextureCoords(unsigned int a_idx, unsigned int b_idx, unsigned int c_idx)
    {
        std::vector<MappedTexture>::iterator iter = textures.begin();
        while(iter!=textures.end())
        {
            MappedTexture &ti = *iter++;
            ti.RemapTextureCoords(a_idx,b_idx,c_idx);
        }
    }

    sfa::Polygon Face::getPolygon() const
    {
        sfa::Polygon ret;
        sfa::LineString ring;
        int numVerts = getNumVertices();
        if(numVerts)
        {
            sfa::Point testpt(getVertN(0));
            ring.addPoint(testpt);
        }
        for(int i=numVerts-1;i>=0;i--)
        {
            sfa::Point testpt(getVertN(i));
            ring.addPoint(testpt);
        }
        
        ret.addRing(ring);
        return ret;
    }
}