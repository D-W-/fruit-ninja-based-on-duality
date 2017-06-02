#pragma once

#include <CGAL/Exact_rational.h>
#include <CGAL/Filtered_extended_homogeneous.h>
#include <CGAL/Extended_cartesian.h>
#include <CGAL/Nef_polyhedron_2.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Point_2.h>
#include <CGAL/enum.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Exact_rational RT;
typedef CGAL::Extended_cartesian<RT> Extended_kernel;
typedef CGAL::Nef_polyhedron_2<Extended_kernel> Polyhedron;
typedef Polyhedron::Explorer Explorer;
typedef Polyhedron::Line  Line;
typedef Polyhedron::Point Point;
typedef Explorer::Vertex_const_handle    Vertex_const_handle;

typedef pair<Point, Point> Segment;

bool getStabbingLine(Polyhedron& dualRegion, vector<Segment>& segments, Line& stabbingLine);
void addDoubleWedge(Polyhedron& spaces, Line& a, Line& b);
void getSingleWedge(Polyhedron& singleWedge, Line& a, Line& b);
void explorePolyhedron(Polyhedron& dualRegion, Point& stabbingPoint);
void addGap(Polyhedron& spaces, Line& a, Line& b);
void getStabbingLineStabsMostSegments(vector<Segment>& segments, Line& stabbingLine);
void genStabbingLine(Polyhedron dualRegion, vector<Segment>& segments, int now, int start, int& count, Line& stabbingLine);
/*
Added by Liu
*/
bool isParallel(vector<Segment> segments, bool& isVertical, double& cosine_alpha, double& sine_alpha);
bool getStabbingLinePara(Polyhedron dualRegion, vector<Segment> segments, Line&  stabbingLine, bool isVertical, double cosine_alpha, double sine_alpha);