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

/* 
Added by Liu
Judge whether input segments are parallel or not, is yes reutrn true, otherwise return false;
IsVertical: true if input segments are all vertical to x-axis, otherwise false;
cosine_alpha: cosine of the angle (for coordinates rotation)
sine_alpha: sine of the angle (for coordinates rotation)

*/
bool isParallel(vector<Segment> segments, bool& isVertical, double& cosine_alpha, double& sine_alpha)
{
	if (segments.size() < 2)
	{
		cout <<"Error: too few input segments"<<endl;
		return false;
	}
	bool isPara = true;
	Segment initialSeg = segments[0];
	if (CGAL::to_double(initialSeg.first.x()) == CGAL::to_double(initialSeg.second.x()))
		isVertical = true;
	else
		isVertical = false;
	double slope = 0;
	if (!isVertical)
		slope = (CGAL::to_double(initialSeg.first.y()) - CGAL::to_double(initialSeg.second.y())) / (CGAL::to_double(initialSeg.first.x()) - CGAL::to_double(initialSeg.second.x()));
	for (auto segment : segments)
	{
		bool curIsVertical = (CGAL::to_double(segment.first.x()) == CGAL::to_double(segment.second.x()));
		if (curIsVertical)
			if (isVertical)
				continue;
			else
			{
				isPara = false;
				return isPara;
			}
		else
		{
			if (isVertical)
			{
				isPara = false;
				return isPara;
			}
			else
			{
				double curSlope = (CGAL::to_double(segment.first.y()) - CGAL::to_double(segment.second.y())) / (CGAL::to_double(segment.first.x()) - CGAL::to_double(segment.second.x()));
				if (curSlope == slope) // could be improved by tolerating eps error
					continue;
				else
				{
					isPara = false;
					return isPara;
				}
			}
		}
	}
	if (isPara)
	{
		/*
		----------------------------------------------------------------------------------for reference-----------------------------------------------------------------------------------------------
		alpha=theta-pi/2; where alpha is the angle to rotate the point (or the segment) clockwisely, theta is the angle between the segment and the x-axis and tangent(theta)=slope;
	    so we have, cosine(alpha)=sine(theta) and sine(alpha)=-cosine(theta) ; 
		Since tangent(theta)=slope, we have sine(theta)=slope / sqrt(1 + slope*slope); and cosine(theta)=1 / sqrt(1 + slope*slope);
		----------------------------------------------------------------------------------end reference---------------------------------------------------------------------------------------------
		*/
		if (!isVertical)
		{
			cosine_alpha = slope / sqrt(1 + slope*slope);
			sine_alpha = -1 / sqrt(1 + slope*slope);
		}
	}
	return isPara;
}
/*
Added by Liu
Find the stabbing line for cases where segments are parallel.

----------------------------begin reference---------------------------------------------------
given a point p(x,y), rotate p around the origin by alpha angle clockwisely:

x'=cosine(alpha)*x+sine(alpha)*y;
y'=-sine(alpha)*x+cosine(alpha)*y;

given a line : ax+by+c=0, rotate the line around the origin by alpha angle counterclockwisely:
(a*cosine(alpha)-b*(sine(alpha)))x +(a*sine(alpha)+b*cosine(alpha))y+c=0;

----------------------------end reference--------------------------------------------------------
*/
void getStabbingLinePara(Polyhedron& dualRegion, vector<Segment>& segments, Line&  stabbingLine, bool isVertical, double cosine_alpha, double sine_alpha)
{
	//The first step (rotation) : Rotate the coordinates to transform each segment (only if isVertical == false; otherwise do nothing)
	if (!isVertical)
	{
		vector<Segment> segmentsTrans;
		for (auto segment : segments)
		{
			double x1 = cosine_alpha*CGAL::to_double(segment.first.x()) + sine_alpha*CGAL::to_double(segment.first.y());
			double y1 = -sine_alpha*CGAL::to_double(segment.first.x()) + cosine_alpha*CGAL::to_double(segment.first.y());
			double x2 = cosine_alpha*CGAL::to_double(segment.second.x()) + sine_alpha*CGAL::to_double(segment.second.y());
			double y2 = -sine_alpha*CGAL::to_double(segment.second.x()) + cosine_alpha*CGAL::to_double(segment.second.y());
			if (x1 != x2)
			{
				cout << "Error: difference of x coordinates: " << abs(x1 - x2) << ", segment is not vertical after transformation, check cosine_alpha and sine_alpha." << endl;
				return;
			}
			segmentsTrans.push_back(make_pair(Point(x1, y1), Point(x2, y2)));
		}
		segments = segmentsTrans;
	}

	//The second step (intersection) : Call addGap() to obtain band area for each transformed segment (vertical segment now), then call explorePolyhedron() to find stabbing point that is within the intersection of the band areas, thus find the stabbing line.
	Point point1, point2;
	for (auto segment : segments) {
		point1 = segment.first;
		point2 = segment.second;
		//sort by x axis make sure the double wedge/ gap runs anticlockwise
     	if (point1.y() < point2.y()) {
			addGap(dualRegion, Line(-point1.x(), 1, point1.y()), Line(-point2.x(), 1, point2.y()));
		}
		else  if (point1.y() > point2.y()) {
			addGap(dualRegion, Line(-point2.x(), 1, point2.y()), Line(-point1.x(), 1, point1.y()));
		}
		if (dualRegion.is_empty())
		{
			cout << "Error: empty intersected region." << endl;
			return ;
		}
	}	
	bool isfind = false;
    Point stabbingPoint;
	explorePolyhedron(dualRegion, stabbingPoint);
	Explorer e = dualRegion.explorer();
	for (auto v = e.vertices_begin(); v != e.vertices_end(); ++v) {
		if (e.is_standard(v)) {
			isfind = true;
			stabbingPoint = e.point(v);
			//cout << CGAL::to_double(stabbingPoint.x()) << " " << CGAL::to_double(stabbingPoint.y()) << endl;
			break;
		}
	}
	if (isfind) 
		stabbingLine = Line(-stabbingPoint.x(), 1, stabbingPoint.y()); 
	else 
	{ 
		cout << "Error: region is non-empty, but stabbing point not found" << endl; 
		return;
	}

	//The Third step (inverse rotation) :Inversely rotate the coordinates to get the stabbing line transformed back to origianl coordinates. (only if isVertical == false; otherwise do nothing)
	if (!isVertical)
	{		
		double a = stabbingLine.a;
		double b = stabbingLine.b;
		double c = stabbingLine.c;
		double aa = a*cosine_alpha - b*sine_alpha;
		double bb = a*sine_alpha + b*cosine_alpha;
		double cc = c;
		if (bb == 0)
		{
			cout << "Error: stabbling line is vertical to the x-axis, this case cannot be handled here" << endl;
			return;
		}
		aa = aa / bb;
		cc = cc / bb;
		stabbingLine = Line(aa, 1, cc);
		stabbingPoint = Point(-aa, cc);
	}
	cout << CGAL::to_double(stabbingPoint.x()) << " " << CGAL::to_double(stabbingPoint.y()) << endl;
	return;
}
void runFromFile() {
	Polyhedron dualRegion(Polyhedron::COMPLETE);
	vector<Segment> segments;
	Line stabbingLine;
	ifstream ifs("test.data");
	getSegmentsFromStream(ifs, segments);
	/*
	Added by Liu: 
	if isParallel() returns true, the program goes to getStabbingLinePara(), otherwise it goes to getStabbingLine();
	*/
	bool isVertical = false;
	double cosine_alpha, sine_alpha;
	if (isParallel(segments, isVertical, cosine_alpha, sine_alpha))
		getStabbingLinePara(dualRegion, segments, stabbingLine, isVertical, cosine_alpha, sine_alpha);
	else
		getStabbingLine(dualRegion, segments, stabbingLine);
}
void runFromConsole() {
	Polyhedron dualRegion(Polyhedron::COMPLETE);
	vector<Segment> segments;
	Line stabbingLine;
	getSegmentsFromStream(cin, segments);
	/*
	Added by Liu:
	if isParallel() returns true, the program goes to getStabbingLinePara(), otherwise it goes to getStabbingLine();
	*/
	bool isVertical = false;
	double cosine_alpha, sine_alpha;
	if (isParallel(segments, isVertical, cosine_alpha, sine_alpha))
		getStabbingLinePara(dualRegion, segments, stabbingLine, isVertical, cosine_alpha, sine_alpha);
	else
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
	/*
	Added by Liu:
	if isParallel() returns true, the program goes to getStabbingLinePara(), otherwise it goes to getStabbingLine();
	*/
	bool isVertical = false;
	double cosine_alpha, sine_alpha;
	if (isParallel(segments, isVertical, cosine_alpha, sine_alpha))
		getStabbingLinePara(dualRegion, segments, stabbingLine, isVertical, cosine_alpha, sine_alpha);
	else
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