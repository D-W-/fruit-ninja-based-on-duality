#include <iostream>
#include <cstdio>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <deque>
#include <vector>
#include <queue>
#include <string>
#include <cstring>
#include <map>
#include <stack>
#include <set>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <sys/timeb.h>

using namespace std;

struct intpoint
{
	double x;
	double y;
	bool operator < (const intpoint& i2) const
	{
		return x < i2.x || (x == i2.x && y < i2.y);
	}
	intpoint() = default;
	intpoint(double x, double y) : x(x), y(y) {}
	intpoint operator- (const intpoint& i2) const
	{
		return intpoint(x - i2.x, y - i2.y);
	}
};

struct vec4
{
	intpoint a;
	intpoint b;
	vec4() = default;
	vec4(const intpoint& a, const intpoint& b) : a(a), b(b) {}
	intpoint vec()
	{
		return b - a;
	}
};

intpoint uppoint[10010], downpoint[10010];
int n;

vec4 hull[20020];
int hullp;

int check(int x)
{
	for (int i = 0; i < n; i++)
	{
		double intersect = hull[x].a.y * (hull[x].a.x - hull[x].b.x) + (uppoint[i].x - hull[x].a.x) * (hull[x].a.y - hull[x].b.y);
		if (intersect < uppoint[i].y * (hull[x].a.x - hull[x].b.x)) return 0;
		if (intersect > downpoint[i].y * (hull[x].a.x - hull[x].b.x)) return 0;
	}
	return 1;
}

//counter-clockwise positive
double xproduct(const intpoint& p1, const intpoint& p2)
{
	return 1ll * p1.x * p2.y - 1ll * p2.x * p1.y;
}

int main()
{
	//scanf("%d", &n);
	ifstream fin;
	//time_t start, stop;
	struct timeb t_start, t_end;
	string dir_name, file_name, path_name;
	dir_name = "round_vertical_";
	file_name = "group_";
	int j, k;
	for (k = 1; k <= 5; k++)
	{
		stringstream ss;
		ss << k;
		string str1 = ss.str();

		string path_name1 = dir_name + str1 + "\\";
		for (j = 1; j <= 10; j++)
		{
			stringstream ss1;
			ss1 << j;
			string str2 = ss1.str();
			path_name = path_name1 + file_name + str2 + ".txt";
			//cout << path_name << endl;
			fin.open(path_name.c_str());
			if (fin.bad())
			{
				cout << "can not open " + path_name << endl;
				return 0;
			}
			//start = clock();
			ftime(&t_start);

			int n = 0, i = 0;
			double x1, y1, x2, y2;
			while (fin >> x1 >> y1 >> x2 >> y2)
			{
				uppoint[i] = intpoint(x1, y1);
				downpoint[i] = intpoint(x2, y2);
				i++;
			}
			n = i;
			/*
			for(int i = 0; i < n; i++){
			double x, y1, y2;
			scanf("%lf%lf%lf", &x, &y1, &y2);
			uppoint[i] = intpoint(x, y1);
			downpoint[i] = intpoint(x, y2);
			}
			*/
			sort(uppoint, uppoint + n);
			int ntmp = n;
			int xp = 0;
			for (int i = 1; i < n; i++)
			{
				if (uppoint[i].x == uppoint[xp].x)
				{
					uppoint[i].x = 1e9;
					uppoint[xp].y = min(uppoint[i].y, uppoint[xp].y);
					ntmp--;
				}
				else
				{
					xp = i;
				}
			}
			xp = 0;
			sort(downpoint, downpoint + n);
			for (int i = 1; i < n; i++)
			{
				if (downpoint[i].x == downpoint[xp].x)
				{
					downpoint[i].x = 1e9;
					downpoint[xp].y = max(downpoint[i].y, downpoint[xp].y);
				}
				else
				{
					xp = i;
				}
			}
			sort(uppoint, uppoint + n);
			sort(downpoint, downpoint + n);
			n = ntmp;
			if (n == 1)
			{
				printf("%d %d %d %d\n", uppoint[0].x - 1, uppoint[0].y, uppoint[0].x + 1, uppoint[0].y);
				return 0;
			}
			hullp = 0;
			hull[hullp++] = vec4(uppoint[0], uppoint[1]);
			for (int i = 2; i < n; i++)
			{
				while (hullp > 0 && xproduct(hull[hullp - 1].vec(), uppoint[i] - hull[hullp - 1].b) < 0)
				{
					hullp--;
				}
				hull[hullp] = vec4(hullp > 0 ? hull[hullp - 1].b : uppoint[0], uppoint[i]);
				hullp++;
			}
			int fixed = hullp;
			hull[hullp++] = vec4(downpoint[0], downpoint[1]);
			for (int i = 2; i < n; i++)
			{
				while (hullp > fixed && xproduct(hull[hullp - 1].vec(), downpoint[i] - hull[hullp - 1].b) > 0)
				{
					hullp--;
				}
				hull[hullp] = vec4(hullp > fixed ? hull[hullp - 1].b : downpoint[0], downpoint[i]);
				hullp++;
			}
			for (int i = 0; i < hullp; i++)
			{
				if (check(i))
				{
					//printf("%lf %lf %lf %lf\n", hull[i].a.x, hull[i].a.y, hull[i].b.x, hull[i].b.y);
					break;
					//return 0;
				}
			}

			//stop = clock();
			//double dur = (double)(stop - start);
			//cout << path_name << ": " << dur * 1000 / CLOCKS_PER_SEC << endl;
			ftime(&t_end);
			cout << (t_end.time - t_start.time) * 1000 + (t_end.millitm - t_start.millitm) << endl;
			fin.close();

		}
	}

	return 0;
}
