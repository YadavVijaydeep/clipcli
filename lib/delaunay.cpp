/* https://github.com/Bl4ckb0ne/delaunay-triangulation */
/*
 * Copyright (c) 2015 Simon Zeni (simonzeni@gmail.com)
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "delaunay.h"
#include <algorithm>

const std::vector<Triangle>& Delaunay::triangulate(std::vector<Vec2f> &vertices)
{
	// Store the vertices localy
	_vertices = vertices;

	// Determinate the super triangle
	double minX = vertices[0].x;
	double minY = vertices[0].y;
	double maxX = minX;
	double maxY = minY;

	for(std::size_t i = 0; i < vertices.size(); ++i) 
	{
		if (vertices[i].x < minX) minX = vertices[i].x;
    	if (vertices[i].y < minY) minY = vertices[i].y;
    	if (vertices[i].x > maxX) maxX = vertices[i].x;
    	if (vertices[i].y > maxY) maxY = vertices[i].y;
	}
	
	double dx = maxX - minX;
	double dy = maxY - minY;
	double deltaMax = std::max(dx, dy);
	double midx = (minX + maxX) / 2.f;
	double midy = (minY + maxY) / 2.f;

	Vec2f p1(midx - 20 * deltaMax, midy - deltaMax);
	Vec2f p2(midx, midy + 20 * deltaMax);
	Vec2f p3(midx + 20 * deltaMax, midy - deltaMax);	

	//std::cout << "Super triangle " << std::endl << Triangle(p1, p2, p3) << std::endl;
	
	// Create a list of triangles, and add the supertriangle in it
	_triangles.push_back(Triangle(p1, p2, p3));

	for(auto p = begin(vertices); p != end(vertices); p++)
	{
		//std::cout << "Traitement du point " << *p << std::endl;
		//std::cout << "_triangles contains " << _triangles.size() << " elements" << std::endl;	

		std::vector<Triangle> badTriangles;
		std::vector<Edge> polygon;

		for(auto t = begin(_triangles); t != end(_triangles); t++)
		{
			//std::cout << "Processing " << std::endl << *t << std::endl;

			if(t->circumCircleContains(*p))
			{
				//std::cout << "Pushing bad triangle " << *t << std::endl;
				badTriangles.push_back(*t);
				polygon.push_back(t->e1);	
				polygon.push_back(t->e2);	
				polygon.push_back(t->e3);	
			}
			else
			{
				//std::cout << " does not contains " << *p << " in his circum center" << std::endl;
			}
		}

		_triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [badTriangles](Triangle &t){
			for(auto bt = begin(badTriangles); bt != end(badTriangles); bt++)
			{	
				if(*bt == t)
				{
					//std::cout << "Removing bad triangle " << std::endl << *bt << " from _triangles" << std::endl;
					return true;		
				}
			}
			return false;
		}), end(_triangles));

		std::vector<Edge> badEdges;
		for(auto e1 = begin(polygon); e1 != end(polygon); e1++)
		{
			for(auto e2 = begin(polygon); e2 != end(polygon); e2++)
			{
				if(e1 == e2)
					continue;
				
				if(*e1 == *e2)
				{
					badEdges.push_back(*e1);	
					badEdges.push_back(*e2);	
				}
			}
		}

		polygon.erase(std::remove_if(begin(polygon), end(polygon), [badEdges](Edge &e){
			for(auto it = begin(badEdges); it != end(badEdges); it++)
			{
				if(*it == e)
					return true;
			}
			return false;
		}), end(polygon));

		for(auto e = begin(polygon); e != end(polygon); e++)
			_triangles.push_back(Triangle(e->p1, e->p2, *p));
	
	}

	_triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [p1, p2, p3](Triangle &t){
		return t.containsVertex(p1) || t.containsVertex(p2) || t.containsVertex(p3);
	}), end(_triangles));

	for(auto t = begin(_triangles); t != end(_triangles); t++)
	{
		_edges.push_back(t->e1);
		_edges.push_back(t->e2);
		_edges.push_back(t->e3);
	} 

	return _triangles;
}
