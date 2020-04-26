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
#include "ModelKit/AvoidClipper.h"
#include <ctl/QTriangulate.h>
#include <ctl/Util.h>
#include <algorithm>

#include <boost/foreach.hpp>

#include <cad/AttributeDictionary.h>

namespace modelkit {

		const unsigned int BSPNode::AXIS_X		= 0;
		const unsigned int BSPNode::AXIS_Y		= 1;
		const unsigned int BSPNode::AXIS_Z		= 2;
		const unsigned int BSPNode::AXIS_NONE		= 2;
		const unsigned int BSPNode::LEAF			= 0;
		const unsigned int BSPNode::BRANCH		= 1;
		const unsigned int BSPNode::MAX_DEPTH		= 5;
		const unsigned int BSPNode::MAX_CHILDREN	= 3;
		const unsigned char ClippingTriangle::PLANAR_ABOVE		= 0x01;
		const unsigned char ClippingTriangle::PLANAR_BELOW		= 0x02;
		const unsigned char ClippingTriangle::PLANAR_BOTH		= 0x03;
		const unsigned char ClippingTriangle::HIGH_PRIORITY	= 0x04;	//	This flag tells the split function to add resulting triangle to the outside array when done.





	bool pointCheck( const sfa::Point& p1, const sfa::Point& p2 )
	{
		sfa::Point test = p1 - p2;
		if ( abs(test.X()) > CLIP_EPSILON ) return false;
		else if ( abs(test.Y()) > CLIP_EPSILON ) return false;
		else if ( abs(test.Z()) > CLIP_EPSILON ) return false;
		else return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	BSP TREE
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BSPSegment::BSPSegment( const sfa::Point& p1, const sfa::Point& p2, double width, ClippingZone* zone )
	{
		parent = zone;
		start = p1;
		end = p2;
		middle = (start + end)*0.5;
		cRadius = width;
		sRadius = (end - middle).length() + cRadius;
	}

	BSPSegment::BSPSegment( const BSPSegment& segment )
	{
		parent = segment.parent;
		start = segment.start;
		middle = segment.middle;
		end = segment.end;
		cRadius = segment.cRadius;
		sRadius = segment.sRadius;
	}

	BSPSegment& BSPSegment::operator=( const BSPSegment& rhs )
	{
		parent = rhs.parent;
		start = rhs.start;
		middle = rhs.middle;
		end = rhs.end;
		cRadius = rhs.cRadius;
		sRadius = rhs.sRadius;
		return *this;
	}

	bool BSPSegment::touch( const BSPSegment* segment ) const
	{
		sfa::Point temp = segment->middle - middle;
		if (temp.length() > (segment->sRadius + sRadius)) return false;

		sfa::Point w = start - segment->start;
		sfa::Point u = end - start;
		sfa::Point v = segment->end - segment->start;

		double uu = u.dot(u);
		double uv = u.dot(v);
		double vv = v.dot(v);
		double uw = u.dot(w);
		double vw = v.dot(w);

		double D = uu*vv - uv*uv;
		double sc, sN, sD = D;
		double tc, tN, tD = D;

	//	Find the points to use for comparison and handle any cases that could result in division by 0.
		if ( D < CLIP_EPSILON )	//	Segments are nearly parallel, use a
		{
			sN = 0;
			sD = 1;
			tN = vw;
			tD = vv;
		}
		else
		{
			sN = uv*vw - vv*uw;
			tN = uu*vw - uv*uw;
			if ( sN < 0 )		//	Nearest point is before a_b so use a
			{
				sN = 0;
				tN = vw;
				tD = vv;
			}
			else if (sN > sD )	//	Nearest point is after a_b so use b
			{
				sN = sD;
				tN = vw + uv;
				tD = vv;
			}
		}

	//	Account for the fact that the closest points may not be actually on the line segment and correct for that.
		if (tN < 0.0)					//	Point is before c_d so use c
		{
			tN = 0.0;
			if (-uw < 0) sN = 0;
			else if (-uw > uu) sN = sD;
			else
			{
				sN = -uw;
				sD = uu;
			}
		}
		else if (tN > tD)				//	Point is after c_d so use d
		{
			tN = tD;
			double f = (-uw + uv);
			if (f < 0)			sN = 0;
			else if (f > uu)	sN = sD;
			else
			{
				sN = f;
				sD = uu;
			}
		}

	//	find the final distances and calculate the vector comparable distance between them.
		sc = abs(sN) < CLIP_EPSILON ? 0 : sN / sD;
		tc = abs(tN) < CLIP_EPSILON ? 0 : tN / tD;

		temp = w + (u*sc - v*tc);
		double dist = temp.length();

		if (dist < (cRadius + segment->cRadius) )
			return true;
		else
			return false;

	}

	bool BSPNode::sorting = false;

	void BSPNode::split(void)
	{
		if (data.size() < MAX_CHILDREN || type() == BRANCH) return;

		double bsp_min = DBL_MAX;
		double bsp_max = -DBL_MAX;

		children.reserve(2);
		children.push_back( BSPNode(this) );
		children.push_back( BSPNode(this) );

		axis = parent ? (parent->axis+1)%3 : AXIS_X;

		if (axis = AXIS_X)
		{
			BOOST_FOREACH( const BSPSegment* seg, data )
			{
				bsp_min = std::min<double>( bsp_min , std::min<double>( seg->start.X() , seg->end.X() ) );
				bsp_max = std::max<double>( bsp_max , std::max<double>( seg->start.X() , seg->end.X() ) );
			}
		}
		else if (axis = AXIS_Y)
		{
			BOOST_FOREACH( const BSPSegment* seg, data )
			{
				bsp_min = std::min<double>( bsp_min , std::min<double>( seg->start.Y() , seg->end.Y() ) );
				bsp_max = std::max<double>( bsp_max , std::max<double>( seg->start.Y() , seg->end.Y() ) );
			}
		}
		else if (axis = AXIS_Z)
		{
			BOOST_FOREACH( const BSPSegment* seg, data )
			{
				bsp_min = std::min<double>( bsp_min , std::min<double>( seg->start.X() , seg->end.Z() ) );
				bsp_max = std::max<double>( bsp_max , std::max<double>( seg->start.X() , seg->end.Z() ) );
			}
		}

		value = (bsp_min+bsp_max)*0.5;

		BSPSegmentList temp;
		temp.swap(data);

		BOOST_FOREACH( BSPSegment* seg, temp )
		{
			addSegment(seg);
		}

	//	If the children have no data, remove them
		if ((children[0].data.size() + children[1].data.size()) == 0)
		{
			children.resize(0);
			axis = AXIS_NONE;
		}
	}

	BSPNode::BSPNode( BSPNode* parent ) : parent(parent)
	{
		axis = AXIS_NONE;
		value = 0;
	}

	BSPNode::BSPNode( const BSPNode& node )
	{
		parent = node.parent;
		children = node.children;
		axis = node.axis;
		value = node.value;
		data = node.data;
	}

	BSPNode& BSPNode::operator=( const BSPNode& node )
	{
		parent = node.parent;
		children = node.children;
		axis = node.axis;
		value = node.value;
		data = node.data;
		return *this;
	}

	unsigned int BSPNode::type(void) const
	{
		return axis == AXIS_NONE ? LEAF : BRANCH;
	}

	unsigned int BSPNode::depth(void) const
	{
		return parent ? parent->depth() + 1 : 0;
	}

	void BSPNode::addSegment( BSPSegment* segment )
	{
		if (type() == LEAF)
		{
			data.push_back(segment);

			if (data.size() > MAX_CHILDREN && depth() < MAX_DEPTH && sorting)
				split();
		}
		else
		{
			bool left, right;

			if (axis == AXIS_X)
			{
				left = (std::max<double>( segment->start.X() , segment->end.X() ) + segment->cRadius ) < value;
				right = (std::min<double>( segment->start.X() , segment->end.X() ) - segment->cRadius ) > value;
			}
			else if (axis == AXIS_Y)
			{
				left = (std::max<double>( segment->start.Y() , segment->end.Y() ) + segment->cRadius ) < value;
				right = (std::min<double>( segment->start.Y() , segment->end.Y() ) - segment->cRadius ) > value;
			}
			else if (axis == AXIS_Z)
			{
				left = (std::max<double>( segment->start.Z() , segment->end.Z() ) + segment->cRadius ) < value;
				right = (std::min<double>( segment->start.Z() , segment->end.Z() ) - segment->cRadius ) > value;
			}
			else //	Debug
				printf("Error adding segment to BSP!\n");

			if (left)
				children[0].addSegment(segment);
			else if (right)
				children[1].addSegment(segment);
			else
				data.push_back(segment);
		}
	}

	void BSPNode::sort(void)
	{
		sorting = true;
		if (data.size() > MAX_CHILDREN && depth() < MAX_DEPTH)
			split();
		sorting = false;
	}

	void BSPNode::getSegments( const BSPSegment* segment, BSPSegmentList& segments ) const
	{
		segments.reserve(segments.size() + data.size());
		BOOST_FOREACH( BSPSegment* seg, data )
		{
			if (seg->touch(segment))
				segments.push_back(seg);
		}

		if (type() == BRANCH)
		{
			bool left, right;

			if (axis == AXIS_X)
			{
				right = (std::max<double>( segment->start.X() , segment->end.X() ) + segment->cRadius ) > value;
				left = (std::min<double>( segment->start.X() , segment->end.X() ) - segment->cRadius ) < value;
			}
			else if (axis == AXIS_Y)
			{
				right = (std::max<double>( segment->start.Y() , segment->end.Y() ) + segment->cRadius ) > value;
				left = (std::min<double>( segment->start.Y() , segment->end.Y() ) - segment->cRadius ) < value;
			}
			else if (axis == AXIS_Z)
			{
				right = (std::max<double>( segment->start.Z() , segment->end.Z() ) + segment->cRadius ) > value;
				left = (std::min<double>( segment->start.Z() , segment->end.Z() ) - segment->cRadius ) < value;
			}
			else	//	DEBUG
				printf("Error gettings segments in BSP!\n");

			if (left)
				children[0].getSegments(segment,segments);
			if (right)
				children[1].getSegments(segment,segments);
		}
	}

	void BSPNode::getSegments( const BoundingBox& box, BSPSegmentList& segments ) const
	{
		segments.reserve( segments.size() + data.size());
		BOOST_FOREACH( BSPSegment* seg, data )
		{
			double xmin, ymin, xmax, ymax;

			xmax = seg->middle.X() + seg->sRadius;
			xmin = seg->middle.X() - seg->sRadius;
			ymax = seg->middle.Y() + seg->sRadius;
			ymin = seg->middle.Y() - seg->sRadius;

			if (box.minX <= xmax && box.maxX >= xmin &&
				box.minY <= ymax && box.maxY >= ymax)
				segments.push_back(seg);
		}

		if (type() == BRANCH)
		{
			bool left, right;

			if (axis == AXIS_X)
			{
				left = box.minX < value;
				right = box.maxX > value;
			}
			else if (axis == AXIS_Y)
			{
				left = box.minY < value;
				right = box.maxY > value;
			}
			else if (axis == AXIS_Z)
				left = right = true; // only search X and Y
			else	//	DEBUG
				printf("Error gettings segments in BSP!\n");

			if (left)
				children[0].getSegments(box,segments);
			if (right)
				children[1].getSegments(box,segments);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	CLIPPING TRIANGLE
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	scenegraph::Face ClippingTriangle::asFace(const sfa::Point& origin) const
	{
		scenegraph::Face face;

		face.addVert(points[0] - origin);
		face.addVert(points[1] - origin);
		face.addVert(points[2] - origin);

		if (!parent) return face;

		face.userData = parent->userData;
		face.SetFaceNormal(parent->computeNormal());
		face.groupID = parent->groupID;
		face.groupName = parent->groupName;
		face.textures = parent->textures;
		face.materials = parent->materials;

		for (size_t i = 0, n = face.textures.size(); i < n; i++)
		{
			face.textures[i].uvs[0] = (face.getVertN(0) != parent->getVertN(0)) ? parent->getUVForVertex(face.getVertN(0),int(i)) : parent->textures[i].uvs[0];
			face.textures[i].uvs[1] = (face.getVertN(1) != parent->getVertN(1)) ? parent->getUVForVertex(face.getVertN(1),int(i)) : parent->textures[i].uvs[1];
			face.textures[i].uvs[2] = (face.getVertN(2) != parent->getVertN(2)) ? parent->getUVForVertex(face.getVertN(2),int(i)) : parent->textures[i].uvs[2];
		}

		return face;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	CLIPPING PLANE
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ClippingPlane::ClippingPlane(const PointList& points)
	{
		sfa::LineString* line = new sfa::LineString;
		for (size_t i = 0, n = points.size(); i < n; i++)
			line->addPoint(points[i]);
		line->addPoint(points[0]);
		boundary.addRing(line);

		vertex = points[0];
		normal = (points[1] - points[0]).cross(points[2] - points[0]);
		normal.normalize();
	}

	ClippingPlane::ClippingPlane(const sfa::Point& p0, const sfa::Point& p1, const sfa::Point& p2, const sfa::Point& p3)
	{
		sfa::LineString* line = new sfa::LineString;
		line->addPoint(p0);
		line->addPoint(p1);
		line->addPoint(p2);
		line->addPoint(p3);
		line->addPoint(p0);
		boundary.addRing(line);

		vertex = (p0 + p2)*0.5;
		normal = (p1 - p0).cross(p2 - p0);
		normal.normalize();
	}

	void ClippingPlane::Split(const ClippingTriangle& triangle, ClippingTriangleList& inside, ClippingTriangleList& outside, int relativePriority, bool disjointPolicy)
	{
		int location[3] = { this->location(triangle[0]), this->location(triangle[1]), this->location(triangle[2]) };
		int sum = location[0] + location[1] + location[2];

	//Sum == +/- 3 means completely inside or completely outside
		if (sum == -3 || sum == -2)
			inside.push_back(triangle);
		else if (sum == 3 || sum == 2)
			outside.push_back(triangle);
		else if (sum == -1)
		{
			if ( (location[0] == 0) || (location[1] == 0) || (location[2] == 0) )
				inside.push_back(triangle);
			else
			{
				if (location[0] == 1)
				{
					sfa::Point p1 = interpolate(triangle[1], triangle[0]);
					sfa::Point p2 = interpolate(triangle[2], triangle[0]);

					if ( (!intersects(p1)) && (!intersects(p2)) )
					{
					//	(disjointPolicy ? inside : outside).push_back(triangle);
						return;
					}

					outside.push_back(ClippingTriangle(p1, p2, triangle[0], triangle));
					inside.push_back(ClippingTriangle(triangle[1], p2, p1, triangle, triangle._flags));
					inside.push_back(ClippingTriangle(triangle[2], p2, triangle[1], triangle, triangle._flags));

					edges.push_back(ClippingEdge(p2,p1));
				}
				else if (location[1] == 1)
				{
					sfa::Point p2 = interpolate(triangle[2], triangle[1]);
					sfa::Point p0 = interpolate(triangle[0], triangle[1]);

					if ( (!intersects(p2)) && (!intersects(p0)) )
					{
					//	(disjointPolicy ? inside : outside).push_back(triangle);
						return;
					}

					outside.push_back(ClippingTriangle(p2,p0,triangle[1],triangle));
					inside.push_back(ClippingTriangle(triangle[2],p0,p2,triangle,triangle._flags));
					inside.push_back(ClippingTriangle(triangle[0],p0,triangle[2],triangle,triangle._flags));

					edges.push_back(ClippingEdge(p0,p2));
				}
				else// (location[2] == 1)
				{
					sfa::Point p0 = interpolate(triangle[0], triangle[2]);
					sfa::Point p1 = interpolate(triangle[1], triangle[2]);

					if ( (!intersects(p1)) && (!intersects(p0)) )
					{
					//	(disjointPolicy ? inside : outside).push_back(triangle);
						return;
					}

					outside.push_back(ClippingTriangle(p0,p1,triangle[2],triangle));
					inside.push_back(ClippingTriangle(triangle[0],p1,p0,triangle,triangle._flags));
					inside.push_back(ClippingTriangle(triangle[1],p1,triangle[0],triangle,triangle._flags));

					edges.push_back(ClippingEdge(p1,p0));
				}
			}
		}
		else if (sum == 1)
		{
			if ( (location[0] == 0) || (location[1] == 0) || (location[2] == 0) )
				outside.push_back(triangle);
			else
			{

				if (location[0] == -1)
				{
					sfa::Point p2 = interpolate(triangle[0], triangle[2]);
					sfa::Point p1 = interpolate(triangle[0], triangle[1]);

					if ( (!intersects(p1)) && (!intersects(p2)) )
					{
					//	(disjointPolicy ? inside : outside).push_back(triangle);
						return;
					}

					inside.push_back(ClippingTriangle(triangle[0],p1,p2,triangle,triangle._flags));
					outside.push_back(ClippingTriangle(triangle[2],p2,p1,triangle));
					outside.push_back(ClippingTriangle(triangle[1],triangle[2],p1,triangle));

					edges.push_back(ClippingEdge(p1,p2));
				}
				else if (location[1] == -1)
				{
					sfa::Point p0 = interpolate(triangle[1], triangle[0]);
					sfa::Point p2 = interpolate(triangle[1], triangle[2]);

					if ( (!intersects(p0)) && (!intersects(p2)) )
					{
					//	(disjointPolicy ? inside : outside).push_back(triangle);
						return;
					}

					inside.push_back(ClippingTriangle(triangle[1],p2,p0,triangle,triangle._flags));
					outside.push_back(ClippingTriangle(triangle[0],p0,p2,triangle));
					outside.push_back(ClippingTriangle(triangle[2],triangle[0],p2,triangle));

					edges.push_back(ClippingEdge(p2,p0));
				}
				else // (location[2] == -1)
				{
					sfa::Point p1 = interpolate(triangle[2], triangle[1]);
					sfa::Point p0 = interpolate(triangle[2], triangle[0]);

					if ( (!intersects(p1)) && (!intersects(p0)) )
					{
					//	(disjointPolicy ? inside : outside).push_back(triangle);
						return;
					}

					inside.push_back(ClippingTriangle(triangle[2],p0,p1,triangle,triangle._flags));
					outside.push_back(ClippingTriangle(triangle[1],p1,p0,triangle));
					outside.push_back(ClippingTriangle(triangle[0],triangle[1],p0,triangle));

					edges.push_back(ClippingEdge(p0,p1));
				}			
			}
		}
		else //if (sum == 0)
		{
			if ( (location[0] == 0) && (location[1] == 0) && (location[2] == 0) ) 	//	Coflags
			{
				inside.push_back(triangle);

				unsigned char flags = triangle._flags;

				if (relativePriority < 0)	//	Treat like just another inside triangle
					flags = 0x00;
				else
				{
					sfa::Point norm = (triangle[1] - triangle[0]).cross(triangle[2] - triangle[0]);
					flags |= (norm.dot(normal) > 0) ? ClippingTriangle::PLANAR_BELOW : ClippingTriangle::PLANAR_ABOVE;
			
					if (relativePriority > 0)
						flags |= ClippingTriangle::HIGH_PRIORITY;
				}

				inside.back()._flags = flags;
			}
			else	//	Crosses along a vertex
			{
				if (location[0] == 0)
				{
					sfa::Point p = interpolate(triangle[1],triangle[2]);

					if ( (!intersects(triangle[0])) && (!intersects(p)) )
					{
					//	(disjointPolicy ? inside : outside).push_back(triangle);
						return;
					}

					ClippingTriangle t1(triangle[0],triangle[1],p,triangle);
					ClippingTriangle t2(triangle[0],p,triangle[2],triangle);
					
					inside.push_back( location[1] == -1 ? t1 : t2);
					outside.push_back( location[1] == 1 ? t1 : t2);
					inside.back()._flags = triangle._flags;

					edges.push_back(ClippingEdge( location[1] ==1 ? triangle[0] : p, location[1] == 1 ? p : triangle[0]));
				}
				else if (location[1] == 0)
				{
					sfa::Point p = interpolate(triangle[0],triangle[2]);

					if ( (!intersects(triangle[1])) && (!intersects(p)) )
					{
					//	(disjointPolicy ? inside : outside).push_back(triangle);
						return;
					}

					ClippingTriangle t1(triangle[1],triangle[2],p,triangle);
					ClippingTriangle t2(triangle[1],p,triangle[0],triangle);

					inside.push_back( location[2] == -1 ? t1 : t2);
					outside.push_back( location[2] == 1 ? t1 : t2);
					inside.back()._flags = triangle._flags;

					edges.push_back(ClippingEdge( location[2] ==1 ? triangle[1] : p, location[2] == 1 ? p : triangle[1]));
				}
				else //if (location[2] == 0)
				{
					sfa::Point p = interpolate(triangle[0],triangle[1]);

					if ( (!intersects(triangle[2])) && (!intersects(p)) )
					{
					//	(disjointPolicy ? inside : outside).push_back(triangle);
						return;
					}

					ClippingTriangle t1(triangle[2],triangle[0],p,triangle);
					ClippingTriangle t2(triangle[2],p,triangle[1],triangle);

					inside.push_back( location[0] == -1 ? t1 : t2);
					outside.push_back( location[0] == 1 ? t1 : t2);
					inside.back()._flags = triangle._flags;

					edges.push_back(ClippingEdge( location[0] ==1 ? triangle[2] : p, location[0] == 1 ? p : triangle[2]));
				}
			}
		}
	}

	void ClippingPlane::connectEdges(void)
	{
		size_t num = edges.size();

	//	Connect touching Edges by testing each pair of edges for a link
		for (size_t i = 0; i < num; i++)
		{
			if (edges[i].next && edges[i].prev) continue;

			for (size_t j = 0; j < num; j++)
			{
				if (i == j) continue;

				if ( pointCheck(edges[i].p2,edges[j].p1) )
				{
					edges[i].next = &edges[j];
					edges[j].prev = &edges[i];
				}
				else if ( pointCheck(edges[i].p2,edges[j].p2) )
				{
					edges[i].next = &edges[j];
					edges[j].next = &edges[i];
				}
				else if ( pointCheck(edges[i].p1,edges[j].p1) )
				{
					edges[i].prev = &edges[j];
					edges[j].prev = &edges[i];
				}
				else if ( pointCheck(edges[i].p1,edges[j].p2) )
				{
					edges[i].prev = &edges[j];
					edges[j].next = &edges[i];
				}

				if (edges[i].next && edges[i].prev) break;
			}
		}
	}

	void ClippingPlane::patchHoles(ClippingTriangleList& triangles)
	{
		connectEdges();

		PointList current;
		std::vector<PointList> rings;

		size_t num = edges.size();
		unsigned int processed = 0;
		int last = 0;
		while (processed < num)
		{
			while (edges[last].added)
				last++;

			ClippingEdge* next = &edges[last];
			while (next)
			{
				if (next->added) break;
				processed++;
				next->added = true;

				if (current.size() == 0)
				{
					current.push_back(next->p1);
					current.push_back(next->p2);
					next = next->next;
				}
				else if (pointCheck(current.back(),next->p1))
				{
					current.push_back(next->p2);
					next = next->next;
				}
				else //if (pointCheck(current.back(),next->p2))
				{
					current.push_back(next->p1);
					next = next->prev;
				}
			}
 
			if (pointCheck(current.front(),&current.back()))
				rings.push_back(current);

			current.clear();
		}

	//	Retriangulate all rings
		for (size_t i = 0, n = rings.size(); i < n; i++)
			retriangulate(rings[i],triangles);
	}

	void ClippingPlane::retriangulate(const PointList& points, ClippingTriangleList& triangles) const
	{
		ctl::PointList input;
		ctl::PointList output;

		for (size_t i = 0, n = points.size(); i < n-1; i++)
			input.push_back(ctl::Point(points[i].X(),points[i].Y(),points[i].Z()));

		output = ctl::QTriangulate::apply(input);

		for (size_t i = 0, n = (output.size() - 2); i < n ; i+=3)
		{
			ClippingTriangle triangle(
				sfa::Point(output[i].x,output[i].y,output[i].z),
				sfa::Point(output[i+2].x,output[i+2].y,output[i+2].z),
				sfa::Point(output[i+1].x,output[i+1].y,output[i+1].z)
				);
			triangle.flags = ClippingTriangle::PLANAR_BELOW;
			triangles.push_back(triangle);
		}
	}

	sfa::Point ClippingPlane::interpolate(const sfa::Point& p0, const sfa::Point& p1) const
	{	
		sfa::Point u = p1 - p0;
		double dist = normal.dot(vertex - p0)/normal.dot(u);
		return p0 + u*dist;
	}

	bool ClippingPlane::intersects(const sfa::Point& point) const
	{
		return true;

		//return (point.distance(&boundary) < CLIP_EPSILON);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

	CLIPPING ZONE

*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool ClippingZone::disjoint(const sfa::Point& point) const
	{
		return !boundingBox.intersect(point);
	}

	bool ClippingZone::disjoint(const ClippingTriangle& triangle) const
	{
		return !boundingBox.intersect(triangle.boundingBox);
	}

	ClippingZone::ClippingZone(const PointList &area, const sfa::Projection2D &proj1, const sfa::Projection2D &proj2, const sfa::Point &origin, const scenegraph::Scene* parent)
	{
		this->parent = parent;
		priority = 0;
		SceneData* data = parent?dynamic_cast<SceneData*>(parent->userData):NULL;
		if (data)
		{
			if (data->feature)
			{
				cad::AttributeDictionary dict;
				priority = dict.getPriority(data->feature->attributes);
			}
		}
		initialize(area,proj1,proj2,origin);
	}

	ClippingZone::ClippingZone(const ClippingZone& zone)
	{
		(*this) = zone;
	}

	ClippingZone& ClippingZone::operator=(const ClippingZone& rhs)
	{
		planes = rhs.planes;
		segment = rhs.segment;
		segment.parent = this;
		parent = rhs.parent;
		boundingBox = rhs.boundingBox;
		priority = rhs.priority;
		return *this;
	}

	void ClippingZone::initialize(const PointList& area, const sfa::Projection2D &proj1, const sfa::Projection2D &proj2, const sfa::Point &origin)
	{
		int num = int(area.size()) - 1;

		PointList upper(num);
		PointList lower(num);

		for (int i = 0; i < num; i++)
		{
			upper[i] = proj1.transformPointTo3D(area[i]) + origin;
			lower[i] = proj2.transformPointTo3D(area[i]) + origin;
		}

	//	Check for proper orientation
		if ( ((upper[1] - upper[0]).cross(upper[2] - upper[0])).dot(upper[0] - lower[0]) < 0)
		{
			reverse(upper.begin(),upper.end());
			reverse(lower.begin(),lower.end());
		}

		reverse(lower.begin(),lower.end());

	//	Construct top and bottom planes
		planes.push_back( ClippingPlane(upper) );
		planes.push_back( ClippingPlane(lower) );

		reverse(lower.begin(),lower.end());

	//	Construct Side planes
		for (int i = 0; i < num; i++)
		{
			int next = (i+1)%num;
			planes.push_back( ClippingPlane(lower[i],lower[next],upper[next],upper[i]) );
		}

	//	Find bounding box
		upper.insert(upper.end(),lower.begin(),lower.end());
		boundingBox = BoundingBox(upper);

		sfa::Point centroid = getAvoidAreaCentroid(area);
		double radius = getAvoidAreaRadius(area,centroid);

		sfa::Point seg_1 = proj1.transformPointTo3D(centroid) + origin;
		sfa::Point seg_2 = proj2.transformPointTo3D(centroid) + origin;
		
		segment = BSPSegment(seg_1, seg_2, radius, this );
	}

	sfa::Point ClippingZone::getAvoidAreaCentroid(const PointList& area) const
	{
		double temp, A, x, y;
		A = x = y = 0;

		for (int i = 0, n = int(area.size()-1); i < n; i++)
		{
			temp = area[i].X()*area[i+1].Y() - area[i+1].X()*area[i].Y();
			x += (area[i].X() + area[i+1].X())*temp;
			y += (area[i].Y() + area[i+1].Y())*temp;
			A += temp;
		}

		x *= (1.0/(3.0*A));
		y *= (1.0/(3.0*A));
		return sfa::Point(x,y);
	}

	double ClippingZone::getAvoidAreaRadius(const PointList& area, const sfa::Point& centroid) const
	{
		double radius2 = 0;
		for (int i = 0, n = int(area.size()); i < n; i++)
			radius2 = std::max<double>(radius2, (area[i] - centroid).length2());
		return sqrt(radius2);
	}

	void ClippingZone::ProcessTriangles(const ClippingTriangleList& source, ClippingTriangleList& destination, ccl::uint32_t trianglePriority)
	{
		ClippingTriangleList queue1, queue2;

		queue1.reserve(source.size());
		queue2.reserve(source.size());

		for (ClippingTriangleList::const_iterator it = source.begin(), end = source.end(); it != end; it++)
		{
			if (disjoint(*it))
				destination.push_back(*it);
			else
				queue1.push_back(*it);
		}

		int relativePriority = trianglePriority - priority;

		if (relativePriority < 0)
			int breakpoint = 0;

	//	Alternate back and forth between the queues to reduce copy time. Split the triangle accross each plane
		for (int i = 0, n = int(planes.size()); i < n; i++)
		{
			bool policy = i == n-1;

			planes[i].edges.clear();
			if (queue1.empty())
			{
				for (ClippingTriangleList::iterator it = queue2.begin(), end = queue2.end(); it != end; it++)
					planes[i].Split(*it,queue1,destination,relativePriority,policy);

				queue2.clear();
			}
			else
			{
				for (ClippingTriangleList::iterator it = queue1.begin(), end = queue1.end(); it != end; it++)
					planes[i].Split(*it,queue2,destination,relativePriority,policy);

				queue1.clear();
			}
		}

		ClippingTriangleList& triangles = queue1.empty() ? queue2 : queue1;
		HandleCoplanar( triangles, destination);

#ifdef PATCH_HOLES
		for (int i = 0, n = planes.size(); i < n; i++)
			planes[i].patchHoles(destination);
#endif
	}

	void ClippingZone::HandleCoplanar(ClippingTriangleList& queue, ClippingTriangleList& destination) const
	{
		for (ClippingTriangleList::iterator it = queue.begin(), end = queue.end(); it != end; it++)
		{
			it->resetFlags();
			if (it->flags != 0x00)
			{
				if ((it->flags & ClippingTriangle::PLANAR_BOTH) != ClippingTriangle::PLANAR_BOTH)
				{
					destination.push_back(*it);
				}
			}
		}
	}
		

	void ClippingZone::ProcessHoles(const scenegraph::Face *end, ClippingTriangleList& destination)
	{
	//	For each plane connect all the edges in the plane.
		//	Clip and sort edges around boundary

		//	Snap Edges together
		//	Make resulting polygons.
	}

	bool ClippingZone::intersect(const sfa::Point &p0, const sfa::Point &p1) const
	{
		double d0 = -1e13;
		double d1 = 1e13;

		sfa::Point u = p1 - p0;

		for (int i = 0, j = int(planes.size()); i < j; i++)
		{
			double num = planes[i].normal.dot(p0 - planes[i].vertex);
			double denom = planes[i].normal.dot(u);
			double dist = -num/denom;

			if (denom > 0 && dist < d1)
				d1 = dist;
			else if (denom < 0 && dist > d0)
				d0 = dist;
			else if (denom == 0 && num > -CLIP_EPSILON)
				return false;
			
			if (d1 < d0)
				return false;
		}

		if ( (d0 > (1-CLIP_EPSILON) && d1 > (1-CLIP_EPSILON)) || (d0 < CLIP_EPSILON && d1 < CLIP_EPSILON) )
			return false;
		else 
			return true;
	}

	BSPSegment* ClippingZone::getSegment(void)
	{
		return &segment;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	AVOID CLIPPER
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	AvoidClipper::AvoidClipper(const std::vector<scenegraph::Scene *>& scenes)
	{
		reset();
		initialize(scenes);
	}

	AvoidClipper::AvoidClipper(const std::vector<scenegraph::Scene>& scenes)
	{
		reset();
		initialize(scenes);
	}

	bool AvoidClipper::addZone(const scenegraph::Scene& scene, int index)
	{
		sfa::Point offset = scene.matrix.getTranslation() - origin;

	//	!!ERROR!! No userData so this cannot have AvoidAreas
		if (!scene.userData) return false;

		SceneData* scenedata = dynamic_cast<SceneData*>(scene.userData);

	//	!!ERROR!! No scenedata so this cannot have AvoidAreas
		if (!scenedata) return false;

		CrossSectionSP cs = scenedata->crossSection;
		sfa::Projection2D start = scenedata->projections[index];
		sfa::Projection2D stop = scenedata->projections[index+1];

		std::vector<AvoidArea*> areas = cs->getAvoidAreas();
		
		for (std::vector<AvoidArea*>::iterator it = areas.begin(), end = areas.end(); it != end; it++)
		{
			ClippingZone zone((*it)->getPoints(),start,stop,offset,&scene);
			zones.push_back(zone);
		}

		return true;
	}

	bool AvoidClipper::addZones(const scenegraph::Scene& scene)
	{
	//	!!ERROR!! This Scene has no LineStrings and thus cannot have AvoidAreas
		if (scene.lineStrings.empty()) return false;

		int num = scene.lineStrings[0].lineString.getNumPoints();

		for (int i = 0; i < num-1; i++)
		{
			if (!addZone(scene,i))
			return false;
		}

		return true;
	}

	bool AvoidClipper::addZones(const AvoidZone &avoid)
	{
		if (avoid.projections.empty()) 
			return false;

		int num = int(avoid.projections.size());

		for (int i = 0; i < num-1; i++)
		{
			sfa::Point offset = avoid.origin - origin;

			CrossSectionSP cs = avoid.crossSection;
			sfa::Projection2D start = avoid.projections[i];
			sfa::Projection2D stop = avoid.projections[i+1];

			std::vector<AvoidArea*> areas = cs->getAvoidAreas();
			
			for (std::vector<AvoidArea*>::iterator it = areas.begin(), end = areas.end(); it != end; it++)
			{
				ClippingZone zone((*it)->getPoints(),start,stop,offset);
				zones.push_back(zone);
			}
		}

		return true;
	}


//	Clipping by Section

	bool AvoidClipper::clipSection(scenegraph::Scene& scene, int section, std::vector<int>& freeBlocks) const
	{
		sfa::Point offset = scene.matrix.getTranslation() - origin;

	//	!!ERROR!! No userdata available
		if (!scene.userData) return false;

	//	!!ERROR!! No scenedata available
		SceneData* scenedata = dynamic_cast<SceneData*>(scene.userData);
		if (!scenedata) return false;

		CrossSectionSP cs = scenedata->crossSection;
		sfa::Projection2D start = scenedata->projections[section];
		sfa::Projection2D stop = scenedata->projections[section+1];

		BSPSegment segment( *(start.getOrigin()) + offset, *(stop.getOrigin()) + offset, cs->getRadius());
		BSPSegmentList results;
		root.getSegments(&segment,results);

		return clipSection(scene,section,results,freeBlocks);
	}

	bool AvoidClipper::clipSection(scenegraph::Scene& scene, int section, BSPSegmentList& segments, std::vector<int>& freeBlocks) const
	{
		sfa::Point offset = scene.matrix.getTranslation() - origin;

	// Shrink down segments list and remove all avoid areas that come from this scene
		BSPSegmentList::iterator it = segments.begin();
		while(it != segments.end())
		{
			if ((*it)->parent->parent == &scene)
				it = segments.erase(it);
			else 
				it++;
		}

	//	!!WARNING!! Early return, but only because no segments intersect this scene.
		if (segments.size() == 0) return true;

	//	Create triangle list
		ClippingTriangleList queue1;
		ClippingTriangleList queue2;

		queue1.reserve(scene.sectionFaceIndices[section].size());
		queue2.reserve(scene.sectionFaceIndices[section].size());

	//	Create triangles to clip
		for (int i = 0, n = int(scene.sectionFaceIndices[section].size()); i < n; i++)
		{
			scenegraph::Face& face = scene.faces[ scene.sectionFaceIndices[section][i] ];
			if(face.getNumVertices()>=3)
			{
				ClippingTriangle triangle(face.getVertN(0) + offset, face.getVertN(1) + offset, face.getVertN(2) + offset, &face);
				queue1.push_back(triangle);
			}
		}

	//	Get priority
		ccl::uint32_t priority = 0;
		SceneData* data = dynamic_cast<SceneData*>(scene.userData);
		if (data)
		{
			if (data->feature)
			{
				cad::AttributeDictionary dict;
				priority = dict.getPriority(data->feature->attributes);
			}
		}

	//	CLIP TRIANGLES
		ClippingZone* next = NULL;
		for (it = segments.begin(); it != segments.end(); it++)
		{
			next = ((*it)->parent);
			if (queue1.empty())
			{
				next->ProcessTriangles(queue2,queue1,priority);
				queue2.clear();
			}
			else
			{
				next->ProcessTriangles(queue1,queue2,priority);
				queue1.clear();
			}
		}

	//	Create new Faces
		ClippingTriangleList &results = queue1.empty() ? queue2 : queue1;
		scenegraph::FaceList newFaces;
		newFaces.reserve(results.size());

		for(int i = 0, n = int(results.size()); i < n; i++)
		{
			if (!results[i].collapsed())
				newFaces.push_back(results[i].asFace(offset));
		}

	//	Set free blocks section
		freeBlocks.reserve( freeBlocks.size() + scene.sectionFaceIndices[section].size() );
		for (int i = 0; i < int(scene.sectionFaceIndices[section].size()); i++)
			freeBlocks.push_back(scene.sectionFaceIndices[section][i]);

	//	Replace triangles in scene bucket 
		int numFaces = int(newFaces.size());
		int bucketSize = int(freeBlocks.size());

	//	Reuse memory currently there
		for (int i = 0 ; i < bucketSize; i++)
		{
			if (i < numFaces)
			{
				scene.faces[freeBlocks.back()] = newFaces[i];
				freeBlocks.pop_back();
			}
			else
				break;
		}

		if (numFaces > bucketSize)
			scene.faces.insert(scene.faces.end(),newFaces.begin()+bucketSize,newFaces.end());

		return true;
	}

//	CLIP BY BSP

	void AvoidClipper::clipBSP(scenegraph::Scene& scene, scenegraph::BSPNode* node, std::vector<int>& freeBlocks, ccl::ProgressObserver* progress ) const
	{
		sfa::Point offset = scene.matrix.getTranslation() - origin;
		double minX, maxX, minY, maxY;
		node->getBoundingBox(minX,maxX,minY,maxY);
		BoundingBox box;
		box.minX = minX + offset.X();
		box.maxX = maxX + offset.X();
		box.minY = minY + offset.Y();
		box.maxY = maxY + offset.Y();
		box.minZ = box.maxZ = 0;

		BSPSegmentList segments;
		root.getSegments(box,segments);

		if (!segments.empty())
		{
			ClippingTriangleList queue1, queue2;

		//	PREPARE TRIANGLES
			BOOST_FOREACH ( int index, node->face_indices )
			{
				freeBlocks.push_back(index);
				if (!scene.faces[index].clipped)
					queue1.push_back( ClippingTriangle( 
						scene.faces[index].getVertN(0) + offset, 
						scene.faces[index].getVertN(1) + offset,
						scene.faces[index].getVertN(2) + offset, 
						&scene.faces[index] ));
			}

		//	CLIP TRIANGLES
			ClippingZone* next = NULL;
			for (BSPSegmentList::iterator it = segments.begin(); it != segments.end(); it++)
			{
				next = ((*it)->parent);
				if (queue1.empty())
				{
					next->ProcessTriangles(queue2,queue1,0);
					queue2.clear();
				}
				else
				{
					next->ProcessTriangles(queue1,queue2,0);
					queue1.clear();
				}
			}

		//	CREATE NEW FACES
			ClippingTriangleList &results = queue1.empty() ? queue2 : queue1;
			scenegraph::FaceList newFaces;
			newFaces.reserve(results.size());

			for(int i = 0, n = int(results.size()); i < n; i++)
			{
				if (!results[i].collapsed())
					newFaces.push_back(results[i].asFace(offset));
			}

		//	RESUSE MEMORY AVAILABLE IN FACES ARRAY -> both to conserve resizes, and to ensure any index references by other nodes remain valid
			int numFaces = int(newFaces.size());
			int bucketSize = int(freeBlocks.size());
			for (int i = 0 ; i < bucketSize; i++)
			{
				if (i < numFaces)
				{
					scene.faces[freeBlocks.back()] = newFaces[i];
					freeBlocks.pop_back();
				}
				else
					break;
			}

			if (numFaces > bucketSize)
				scene.faces.insert(scene.faces.end(),newFaces.begin()+bucketSize,newFaces.end());
		}

	//	Update progress bar
		if (progress)
			progress->update(int(node->face_indices.size()));

		if (!node->children.empty())
		{
			clipBSP(scene,node->children[0],freeBlocks,progress);
			clipBSP(scene,node->children[1],freeBlocks,progress);
		}
	}


//	INITIALIZATION -----------------------------
	bool AvoidClipper::initialize(const std::vector<scenegraph::Scene>& scenes)
	{
		reset();

	//	!!ERROR!! No scenes to initialize with
		if (scenes.empty()) return false;

		origin = scenes[0].matrix.getTranslation();

	//	Create AvoidZones
		for(std::vector<scenegraph::Scene>::const_iterator it = scenes.begin(), end = scenes.end(); it != end; it++)
		{
			if (!addZones(*it))
			{
				reset();
				return false;
			}
		}

	//	Sort AvoidZones
		for (ClippingZoneList::iterator it = zones.begin(), end = zones.end(); it != end; it++)
			root.addSegment(it->getSegment());

		root.sort();

		return true;
	}



	//	INITIALIZATION -----------------------------
	bool AvoidClipper::initialize(const AvoidInfo &avoidInfo)
	{
		reset();

	//	!!ERROR!! No scenes to initialize with
		if (avoidInfo.avoidZones.empty()) 
			return false;

		origin = avoidInfo.origin;

	//	Create AvoidZones
		BOOST_FOREACH(const AvoidZone &zone, avoidInfo.avoidZones)
		{
			if (!addZones(zone))
			{
				reset();
				return false;
			}
		}

	//	Sort AvoidZones
		for (ClippingZoneList::iterator it = zones.begin(), end = zones.end(); it != end; it++)
			root.addSegment(it->getSegment());

		root.sort();

		return true;
	}



	bool AvoidClipper::initialize(const std::vector<scenegraph::Scene *>& scenes)
	{
		reset();

	//	!!ERROR!! No scenes to initialize with
		if (scenes.empty()) return false;

		origin = scenes[0]->matrix.getTranslation();

	//	Create AvoidZones
		for(std::vector<scenegraph::Scene *>::const_iterator it = scenes.begin(), end = scenes.end(); it != end; it++)
		{
			scenegraph::Scene *scene = *it;
			if (!addZones(*scene))
			{
				reset();
				return false;
			}
		}

	//	Sort AvoidZones
		for (ClippingZoneList::iterator it = zones.begin(), end = zones.end(); it != end; it++)
			root.addSegment(it->getSegment());

		root.sort();

		return true;
	}

	void AvoidClipper::reset(void)
	{
		zones.clear();
		root = BSPNode();
		origin = sfa::Point();
		progress = NULL;
	}

//	CLIPPING ----------------------------------

	void AvoidClipper::ClipSceneByBSP(scenegraph::Scene& scene, ccl::ProgressObserver* progress) const
	{
		double temp = sfa::SFA_EPSILON;
		sfa::Geometry::setEpsilon(CLIP_EPSILON);

		std::vector<int> freeBlocks;
		scene.sortFaces();

		if (progress)
			progress->start(int(scene.faces.size()));

		clipBSP(scene,&scene.bsp,freeBlocks,progress);

		sfa::Geometry::setEpsilon(temp);
	}

	void AvoidClipper::ClipFaces(std::vector<scenegraph::Face>& faces, const sfa::Point& sceneOrigin, ccl::ProgressObserver* progress) const
	{
		progress->start(int(faces.size()));

		ClippingTriangleList queue1;
		ClippingTriangleList queue2;
		ClippingTriangleList clippedTriangles;

		sfa::Point offset = sceneOrigin - origin;

	//	For each face
		for (int i = 0, n = int(faces.size()); i < n; i++)
		{
			sfa::Point p1 = faces[i].getVertN(0) + offset;
			sfa::Point p2 = faces[i].getVertN(1) + offset;
			sfa::Point p3 = faces[i].getVertN(2) + offset;

			queue1.clear();
			queue2.clear();
			queue1.push_back(ClippingTriangle( p1, p2, p3, &faces[i]));

			BSPSegment ref_seg(p1,p2, std::max<double>((p3-p1).length(),(p3-p2).length()));

			BSPSegmentList results;
			root.getSegments(&ref_seg,results);

			for (BSPSegmentList::iterator it = results.begin(), end = results.end(); it != end; it++)
			{
				ClippingZone* parent = (*it)->parent;
				if (parent)
				{
					if (queue1.empty())
					{
						parent->ProcessTriangles(queue2,queue1,0);
						queue2.clear();
					}
					else
					{
						parent->ProcessTriangles(queue1,queue2,0);
						queue1.clear();
					}
				}
			}

			if (queue2.empty())
				clippedTriangles.insert(clippedTriangles.end(),queue1.begin(),queue1.end());
			else
				clippedTriangles.insert(clippedTriangles.end(),queue2.begin(),queue2.end());

			progress->update();
		}

		std::vector<scenegraph::Face> clippedFaces;
		clippedFaces.reserve(clippedTriangles.size());

		for (ClippingTriangleList::iterator it = clippedTriangles.begin(), end = clippedTriangles.end(); it != end; it++)
		{
			clippedFaces.push_back(it->asFace(offset));
		}

		faces.swap(clippedFaces);
	}

	void AvoidClipper::ClipScene(scenegraph::Scene& scene, ccl::ProgressObserver* progress) const
	{
	//	Making freeBlocks working space
		std::vector<int> freeBlocks;

		for (int i = 0, n = int(scene.faces.size()); i < n; i++)
			if (scene.faces[i].clipped) freeBlocks.push_back(i);

		if (scene.lineStrings.empty()) return;
		int num = scene.lineStrings[0].lineString.getNumPoints();

		for (int i = 0; i < num-1; i++)
			clipSection(scene,i,freeBlocks);

		if (progress) progress->update();

		//	Set all triangle flags to invalid
		for (int i = 0, n = int(freeBlocks.size()); i < n; i++)
		{
			scenegraph::Face& face = scene.faces[freeBlocks[i]];

			face.vertexNormals.clear();
			face.textures.clear();
			face.materials.clear();
			face.clipped = true;
		}

	//	Handle external references
		for (scenegraph::ExternalReferenceList::iterator it = scene.externalReferences.begin(); it != scene.externalReferences.end();)
		{
			sfa::Point o = it->position + scene.matrix.getTranslation() - origin;
			sfa::Point up = it->attitude*sfa::Point(0,0,1);
			sfa::Point start = o + up*it->zMax;
			sfa::Point stop = o + up*it->zMin;

			BSPSegment ref_seg(start,stop,it->radius);

			BSPSegmentList results;
			root.getSegments(&ref_seg,results);

		//	Test if removal is needed
			int j = 0, j_n = int(results.size());
			for (; j < j_n; j++)
			{
				if (results[j]->parent->parent != &scene)
					//if (results[j]->parent->intersect(start,stop))
						break;
			}

			if (j == j_n)
				it++;
			else
				it = scene.externalReferences.erase(it);
		}

		if (progress) progress->update();
	}

	void AvoidClipper::ClipScenes(std::vector<scenegraph::Scene>& scenes, ccl::ProgressObserver* progress)
	{
		double temp = sfa::SFA_EPSILON;
		sfa::Geometry::setEpsilon(CLIP_EPSILON);

		if (progress) progress->start(int(scenes.size()*2 + 1));

		initialize(scenes);
		
		if (progress) progress->update();

		for (std::vector<scenegraph::Scene>::iterator it = scenes.begin(), end = scenes.end(); it != end; it++)
			ClipScene(*it,progress);

		sfa::Geometry::setEpsilon(temp);
	}

}
