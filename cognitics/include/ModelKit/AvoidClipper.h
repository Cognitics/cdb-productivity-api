/*************************************************************************
Copyright (c) 2020 Cognitics, Inc.

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
#include <sfa/Point.h>
#include <sfa/Projection2D.h>
#include <scenegraph/Face.h>
#include <scenegraph/Scene.h>
#include <ccl/ProgressObserver.h>

#include <vector>

#include "ModelKit/CrossSection.h"
#include "ModelKit/SceneData.h"

namespace modelkit {

	const double CLIP_EPSILON = 1e-5;
	const double COLLAPSED_AREA = 1e-4;

	

	class ClippingTriangle;
	class ClippingPlane; 
	class ClippingZone;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	BOUNDING BOX

	Simple bounding box. Used in place of 2 sfa::Points to save memory (since a bounding box doesn't need the m value,
	the 2 flags, the internal weak_ptr, or the CS pointer).
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class BoundingBox
	{
	public:
		double					minX;
		double					minY;
		double					minZ;
		double					maxX;
		double					maxY;
		double					maxZ;

		inline BoundingBox(void)
		{
			minX = minY = minZ = -DBL_MAX;
			maxX = maxY = maxZ = DBL_MAX;
		}

		inline BoundingBox(const PointList& points)
		{
			minX = minY = minZ = DBL_MAX;
			maxX = maxY = maxZ = -DBL_MAX;
			for(PointList::const_iterator it = points.begin(), end = points.end(); it != end; it++)
			{
			  minX = std::min<double> ( minX , it->X() );
			  minY = std::min<double> ( minY , it->Y() );
			  minZ = std::min<double> ( minZ , it->Z() );
			  maxX = std::max<double> ( maxX , it->X() );
				maxY = std::max<double> ( maxY , it->Y() );
				maxZ = std::max<double> ( maxZ , it->Z() );
			}
		}

		inline BoundingBox(const sfa::Point* points, int n)
		{
			minX = minY = minZ = DBL_MAX;
			maxX = maxY = maxZ = -DBL_MAX;
			for(int i = 0; i < n; i++)
			{
			  minX = std::min<double> ( minX , points[i].X() );
				minY = std::min<double> ( minY , points[i].Y() );
				minZ = std::min<double> ( minZ , points[i].Z() );
				maxX = std::max<double> ( maxX , points[i].X() );
				maxY = std::max<double> ( maxY , points[i].Y() );
				maxZ = std::max<double> ( maxZ , points[i].Z() );
			}
		}

		inline BoundingBox(const BoundingBox& box)
		{
			minX = box.minX;
			minY = box.minY;
			minZ = box.minZ;
			maxX = box.maxX;
			maxY = box.maxY;
			maxZ = box.maxZ;
		}

		inline BoundingBox& operator=(const BoundingBox& other)
		{
			minX = other.minX;
			minY = other.minY;
			minZ = other.minZ;
			maxX = other.maxX;
			maxY = other.maxY;
			maxZ = other.maxZ;
			return *this;
		}

		inline bool intersect(const BoundingBox& other) const
		{
			if		( minX > other.maxX || maxX < other.minX ) return false;
			else if ( minY > other.maxY || maxY < other.minY ) return false;
			else if ( minZ > other.maxZ || maxZ < other.minZ ) return false;
			else return true;
		}

		inline bool intersect(const sfa::Point& other) const
		{
			if		( other.X() < minX || other.X() > maxX ) return false;
			else if ( other.Y() < minY || other.Y() > maxY ) return false;
			else if ( other.Z() < minZ || other.Z() > maxZ ) return false;
			else return true;
		}
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	BSP TREE
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class BSPSegment
	{
	public:
		ClippingZone*			parent;
		sfa::Point				start;
		sfa::Point				middle;
		sfa::Point				end;
		double					cRadius;	//	Cylindar Radius
		double					sRadius;	//	Spherical Radius

		BSPSegment( void ) : parent(NULL), cRadius(0), sRadius(0) { } 
		BSPSegment( const sfa::Point& p1, const sfa::Point& p2 , double width = 0, ClippingZone* zone = 0 );
		BSPSegment( const BSPSegment& segment );
		~BSPSegment(void) { }

		BSPSegment& operator=( const BSPSegment& rhs );

		bool touch( const BSPSegment* segment ) const;
	};

	typedef std::vector<BSPSegment*> BSPSegmentList;

	class BSPNode
	{
	public:
		static bool sorting;
		static const unsigned int AXIS_X;
		static const unsigned int AXIS_Y;
		static const unsigned int AXIS_Z;
		static const unsigned int AXIS_NONE;

		static const unsigned int LEAF;
		static const unsigned int BRANCH;

		static const unsigned int MAX_DEPTH;
		static const unsigned int MAX_CHILDREN;

	private:
		BSPNode*				parent;
		std::vector<BSPNode>	children;
		unsigned int			axis;
		double					value;
		BSPSegmentList			data;

		void split(void);

	public:
		BSPNode( BSPNode* parent = NULL );
		BSPNode( const BSPNode& node );
		~BSPNode(void) { }

		BSPNode& operator=( const BSPNode& node );

		unsigned int type(void) const;
		unsigned int depth(void) const;

//!	Add a BSPSegment to this BSPNode Tree
		void addSegment( BSPSegment* segment );

		void sort(void);

//!	Get all zones that are found to conflict with the given point or segment.
		void getSegments( const sfa::Point& point , BSPSegmentList& segments ) const;
		void getSegments( const BSPSegment* segment , BSPSegmentList& segments ) const;
		void getSegments( const BoundingBox& bb, BSPSegmentList& segments ) const;

	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	CLIPPING TRIANGLE
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class ClippingTriangle
	{
	public:
		static const unsigned char PLANAR_ABOVE;
		static const unsigned char PLANAR_BELOW;
		static const unsigned char PLANAR_BOTH;
		static const unsigned char HIGH_PRIORITY;

	public:
		scenegraph::Face		*parent;
		unsigned char			flags;			//	Flags used accross all AvoidZones
		unsigned char			_flags;			//	Flags used by a single AvoidZone
		sfa::Point				points[3];
		BoundingBox				boundingBox;

		ClippingTriangle(void) : parent(NULL), flags(0x00), _flags(0x00) { }
		
		ClippingTriangle( const ClippingTriangle& triangle )
		{
			parent = triangle.parent;
			flags = triangle.flags;
			_flags = triangle._flags;
			points[0] = triangle[0];
			points[1] = triangle[1];
			points[2] = triangle[2];
			boundingBox = triangle.boundingBox;
		}

		ClippingTriangle(const sfa::Point& p1, const sfa::Point& p2, const sfa::Point& p3, scenegraph::Face* face = NULL)
		{
			parent = face;
			flags = 0x00;
			_flags = 0x00;
			points[0] = p1;
			points[1] = p2;
			points[2] = p3;
			boundingBox = BoundingBox(points,3);
		}

		ClippingTriangle(const sfa::Point& p1, const sfa::Point& p2, const sfa::Point& p3, const ClippingTriangle& parent, unsigned char _flags = 0x00)
		{
			this->parent = parent.parent;
			flags = parent.flags;
			this->_flags = _flags;
			points[0] = p1;
			points[1] = p2;
			points[2] = p3;
			boundingBox = BoundingBox(points,3);
		}

		~ClippingTriangle(void) { }

		inline ClippingTriangle& operator=(const ClippingTriangle& rhs)
		{
			parent = rhs.parent;
			flags = rhs.flags;
			_flags = rhs._flags;
			points[0] = rhs[0];
			points[1] = rhs[1];
			points[2] = rhs[2];
			boundingBox = rhs.boundingBox;
			return *this;
		}

		inline sfa::Point& operator[](int i) { return points[i]; }

		inline bool collapsed(void) const 
		{
			double areaTest = (points[1] - points[0]).cross(points[2] - points[0]).length2();
			return areaTest < COLLAPSED_AREA;
		}

		inline const sfa::Point& operator[](int i) const { return points[i]; }

		scenegraph::Face asFace(const sfa::Point& origin) const;

		void resetFlags(void) { flags |= _flags; _flags = 0x00; }
	};
	
	typedef std::vector<ClippingTriangle> ClippingTriangleList;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	CLIPPING EDGE
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class ClippingEdge
	{
	public:
		sfa::Point				p1;
		sfa::Point				p2;
		bool					added;
		ClippingEdge*			next;
		ClippingEdge*			prev;

		ClippingEdge(const sfa::Point& a, const sfa::Point& b) : p1(a), p2(b), added(false), next(NULL), prev(NULL) { }
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	CLIPPING PLANE
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class ClippingPlane
	{
	public:
		sfa::Point				normal;
		sfa::Point				vertex;
		sfa::Polygon			boundary;

		std::vector<ClippingEdge> edges;
		
		ClippingPlane(void) { }
		ClippingPlane(const PointList& points);
		ClippingPlane(const sfa::Point& p0, const sfa::Point& p1, const sfa::Point& p2, const sfa::Point& p3);
		/*inline ClippingPlane(const sfa::Point& n, const sfa::Point& v) : normal(n), vertex(v) { }
		inline ClippingPlane(const ClippingTriangle& triangle) 
		{
			vertex = triangle[0];
			normal = (triangle[1] - vertex).cross(triangle[2] - vertex);
			normal.normalize();
		}
		*/
		inline ClippingPlane(const ClippingPlane& plane)
		{
			normal = plane.normal;
			vertex = plane.vertex;
			boundary = plane.boundary;
			edges = plane.edges;
		}

		bool inside(const sfa::Point& point) const { return (point - vertex).dot(normal) < CLIP_EPSILON; }
		bool	outside(const sfa::Point& point) const { return (point - vertex).dot(normal) > CLIP_EPSILON; }
		bool on(const sfa::Point& point) const 
		{ 
			double temp = (point - vertex).dot(normal); 
			return temp < CLIP_EPSILON && temp > -CLIP_EPSILON; 
		} 

		int location(const sfa::Point& point) const
		{
			double temp = (point - vertex).dot(normal);
			if (temp < -CLIP_EPSILON) return -1;
			else if (temp > CLIP_EPSILON) return 1;
			else return 0;
		}

/*!
	Split does exactly what it says. It splits any triangle along this plane, returning all triangles located inside the half-space to the inside vector
	and all other triangles to the outside vector. Note that relativePriority is the priority of the triangle with respect to the plane. This relative
	priority level can be used to process coplanar triangle cases.

	By Default, this is how split currently handles coplanar triangles.

	See which side of the triangle is inside and which is outside. Flag the triangle with that side information using the planar flag. If planar flag
	has become equivalent to PLANAR_BOTH, remove that triangle.

	if triangle priority is lower than the planes priority put triangle in inside bucket.
	else put triangle in outside bucket.

	if disjointPolicy is false, any triangles that are found disjoint of the boundary are put inside, else they are put outside.

*/
		void Split(const ClippingTriangle& triangle, ClippingTriangleList& inside, ClippingTriangleList& outside, int relativePriority = 0, bool disjointPolicy = false);

//	Connect Edges
		void connectEdges(void);
		void patchHoles(ClippingTriangleList& triangles);
		void retriangulate(const PointList& points, ClippingTriangleList& triangles) const;

//	Interpolation and Intersection
		sfa::Point interpolate(const sfa::Point& p0, const sfa::Point& p1) const;
		bool intersects(const sfa::Point& p0) const;

	};

	typedef std::vector<ClippingPlane> ClippingPlaneList;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	CLIPPING ZONE
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class ClippingZone
	{
	public:
		ClippingPlaneList		planes;
		BoundingBox				boundingBox;
		BSPSegment				segment;
		ccl::uint32_t			priority;
		const scenegraph::Scene* parent;

		ClippingZone(void) { }

		ClippingZone(	const PointList& area,
						const sfa::Projection2D &proj1,
						const sfa::Projection2D &proj2,
						const sfa::Point &origin,
						const scenegraph::Scene* parent = NULL );

		ClippingZone(const ClippingZone& zone);

		ClippingZone& operator=(const ClippingZone& rhs);

		void initialize(const PointList &area, const sfa::Projection2D &proj1, const sfa::Projection2D &proj2, const sfa::Point &origin);

		sfa::Point getAvoidAreaCentroid(const PointList& area) const;
		double getAvoidAreaRadius(const PointList& area, const sfa::Point& centroid) const;

		bool disjoint(const sfa::Point& point) const;
		bool disjoint(const ClippingTriangle& triangle) const;
		bool disjoint(const ClippingZone& zone) const;

		void ProcessTriangles(const ClippingTriangleList& source, ClippingTriangleList& destination, ccl::uint32_t trianglePriority);
		void HandleCoplanar(ClippingTriangleList& queue, ClippingTriangleList& destination) const;
		void ProcessHoles(const scenegraph::Face* end, ClippingTriangleList& destination);

		bool intersect(const sfa::Point &p0, const sfa::Point &p1) const;

		BSPSegment* getSegment(void);
	};

	typedef std::vector<ClippingZone> ClippingZoneList;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	AVOID CLIPPER

	Usage.
\code
	//Initialize clipper with scenes
		AvoidClipper clipper(scenelist);
		//or
		clipper.inialize(scenelist);

	//Clip scene
		clipper.ClipScene(scene);

	//	Or you can reinitialize a the clipper and clip all scenes to each other with one call 
		clipper.ClipScenes(scenelist);
\endcode
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class AvoidClipper
	{
		BSPNode					root;
		ClippingZoneList		zones;
		sfa::Point				origin;
		ccl::ProgressObserver*	progress;

	//	Adding/Creating AvoidZones
		bool addZone(const scenegraph::Scene& scene, int index);
		bool addZones(const scenegraph::Scene& scene);

		bool addZones(const AvoidZone &avoid);

	//	Clipping by Section
		bool clipSection(scenegraph::Scene& scene, int section, std::vector<int>& freeBlocks) const;
		bool clipSection(scenegraph::Scene& scene, int section, BSPSegmentList& segments, std::vector<int>& freeBlocks) const;

	//	Clipping by BSP Zone
		void clipBSP(scenegraph::Scene& scene, scenegraph::BSPNode* node, std::vector<int>& freeBlocks, ccl::ProgressObserver* progress = NULL) const;

	public:
		AvoidClipper(void) { }
		AvoidClipper(const std::vector<scenegraph::Scene>& scenes);
		AvoidClipper(const std::vector<scenegraph::Scene *>& scenes);
		~AvoidClipper(void) { }

//!	Initialize all avoid zones from scenes into this AvoidClipper, return false if an error was encountered and this AvoidClipper couldn't be initialized
		bool initialize(const std::vector<scenegraph::Scene>& scenes);
		bool initialize(const std::vector<scenegraph::Scene *>& scenes);


		bool initialize(const AvoidInfo &avoidInfo);
//!	Clear all AvoidZones from this AvoidClipper
		void reset(void);

		void ClipFaces(std::vector<scenegraph::Face>& faces, const sfa::Point& sceneOrigin, ccl::ProgressObserver* progress) const;
		void ClipSceneByBSP(scenegraph::Scene& scene, ccl::ProgressObserver* progress = NULL) const;
		void ClipScene(scenegraph::Scene& scene, ccl::ProgressObserver* progress = NULL) const;
		void ClipScenes(std::vector<scenegraph::Scene>& scenes, ccl::ProgressObserver* progress = NULL);
	};

}
