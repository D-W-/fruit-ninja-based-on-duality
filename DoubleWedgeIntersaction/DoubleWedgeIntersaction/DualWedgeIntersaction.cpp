#include "DualWedgeDefinations.h"

using namespace std;

void testPointLocation(Polyhedron& dualRegion) {
	cout << dualRegion.contains(dualRegion.locate(Point(0, 2))) << endl;
	cout << dualRegion.contains(dualRegion.locate(Point(-0.5, 0))) << endl;
	cout << dualRegion.contains(dualRegion.locate(Point(0, 0.5))) << endl;
	cout << dualRegion.contains(dualRegion.locate(Point(0, -0.5))) << endl;
}

void getSegmentsFromStream(istream& in, vector<Segment>& segments) {
	string buffer;
	double p1x, p1y, p2x, p2y;
	while (getline(in, buffer)) {
		if (buffer.empty())
			return;
		stringstream ss(buffer);
		ss >> p1x >> p1y >> p2x >> p2y;
		segments.push_back(make_pair(Point(p1x, p1y), Point(p2x, p2y)));
	}
}

void testCase() {
	Polyhedron dualRegion(Polyhedron::COMPLETE);
	vector<Segment> segments;
	Line stabbingLine;
	//segments.push_back(make_pair(Point(-1, 0), Point(1, 0)));
	//segments.push_back(make_pair(Point(-1, 1), Point(1, -1)));
	ifstream ifs("test.data");
	getSegmentsFromStream(ifs, segments);
	getStabbingLine(dualRegion, segments, stabbingLine);
	//testPointLocation(dualRegion);
}

void runFromFile() {
	Polyhedron dualRegion(Polyhedron::COMPLETE);
	vector<Segment> segments;
	Line stabbingLine;
	ifstream ifs("test.data");
	getSegmentsFromStream(ifs, segments);
	getStabbingLine(dualRegion, segments, stabbingLine);
}

void runFromConsole() {
	Polyhedron dualRegion(Polyhedron::COMPLETE);
	vector<Segment> segments;
	Line stabbingLine;
	getSegmentsFromStream(cin, segments);
	getStabbingLine(dualRegion, segments, stabbingLine);
}

void runFromArgc(int argc, char** argv) {
	Polyhedron dualRegion(Polyhedron::COMPLETE);
	vector<Segment> segments;
	Line stabbingLine;
	string buffer = "";
	for (int i = 1; i < argc; ++i) {
		buffer.append(string(argv[i]).append(" "));
	}
	stringstream ss(buffer);
	double p1x, p1y, p2x, p2y;
	while (ss >> p1x >> p1y >> p2x >> p2y) {
		segments.push_back(make_pair(Point(p1x, p1y), Point(p2x, p2y)));
	}
	getStabbingLine(dualRegion, segments, stabbingLine);
}

bool getStabbingLine(Polyhedron& dualRegion, vector<Segment>& segments, Line& stabbingLine) {
	Point point1, point2;
	for (auto segment : segments) {
		point1 = segment.first;
		point2 = segment.second;
		//sort by x axis make sure the double wedge/ gap runs anticlockwise
		if (point1.x() < point2.x()) {
			addDoubleWedge(dualRegion, Line(-point1.x(), 1, point1.y()), Line(-point2.x(), 1, point2.y()));
		}
		else if (point1.x() > point2.x()) {
			addDoubleWedge(dualRegion, Line(-point2.x(), 1, point2.y()), Line(-point1.x(), 1, point1.y()));
		}
		else	if (point1.y() < point2.y()) {
			addGap(dualRegion, Line(-point1.x(), 1, point1.y()), Line(-point2.x(), 1, point2.y()));
		}
		else  if (point1.y() > point2.y()) {
			addGap(dualRegion, Line(-point2.x(), 1, point2.y()), Line(-point1.x(), 1, point1.y()));
		}
		if (dualRegion.is_empty())
			return false;
	}
	Point stabbingPoint;
	explorePolyhedron(dualRegion, stabbingPoint);
	stabbingLine = Line(stabbingPoint.x(), 1, stabbingPoint.y());
	return true;
}

/*
boundary situation: segment vertical to x axis
*/
void addGap(Polyhedron& spaces, Line& a, Line& b) {
	Polyhedron gap;
	getSingleWedge(gap, a, b);
	//testPointLocation(gap);
	spaces = spaces.intersection(gap);
}

/*
spaces:	double wedge intersaction spaces
a:			A1x + B1y + C1 = 0
b:			A2x + B2y + C2 = 0
s.t.: B1 >= 0 && B2 >= 0
*/
void addDoubleWedge(Polyhedron& spaces, Line& a, Line& b) {
	//ensure B1 >=0 && b2 >= 0
	CGAL_assertion(a.b() >= 0);
	CGAL_assertion(b.b() >= 0);
	Line oppositeA(-a.a(), -a.b(), -a.c()), oppositeB(-b.a(), -b.b(), -b.c());
	Polyhedron wedge1, wedge2;
	getSingleWedge(wedge1, a, b);
	getSingleWedge(wedge2, oppositeA, oppositeB);
	Polyhedron doubleWedge = wedge1.join(wedge2);
	spaces = spaces.intersection(doubleWedge);
}

/*
singleWedge: return value, a single wedge space
a: line a
oppositeA: line a's opposite direction
b: line b
oppositeB: line b's opposite direction
*/
void getSingleWedge(Polyhedron& singleWedge, Line& a, Line& b) {
	Polyhedron firstHalf, secondHalf;
	//ensure first line's halfplane is anticlockwise
	firstHalf = Polyhedron(Line(-a.a(), -a.b(), -a.c()), Polyhedron::INCLUDED);
	//ensure second line's halfplane is clockwise
	secondHalf = Polyhedron(b, Polyhedron::INCLUDED);
	singleWedge = firstHalf.intersection(secondHalf);
}

/*
dualRegion: dual region, several convex hulls (might be unbounded)
stabbingPoint: one of the point which lie in the dualRegion
*/
void explorePolyhedron(Polyhedron& dualRegion, Point& stabbingPoint) {
	Explorer e = dualRegion.explorer();
	for (auto v = e.vertices_begin(); v != e.vertices_end(); ++v) {
		if (e.is_standard(v)) {
			stabbingPoint = e.point(v);
			//cout << "standard" << endl;
			cout << CGAL::to_double(stabbingPoint.x()) << " " << CGAL::to_double(stabbingPoint.y()) << endl;
			return;
		}
	}
}


int main(int argc, char** argv) {
	//runFromArgc(argc, argv);
	//runFromConsole();
	runFromFile();

	return 0;
}