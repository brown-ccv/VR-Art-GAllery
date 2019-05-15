#ifndef POINT_H
#define POINT_H

namespace GiGaPixelViewer
{
	 struct Point {
		Point() :x(0), y(0), z(0), a(0), b(0), c(0) {};
		Point(double _x, double _y, double _z = 0.0, double _a = 0.0, double _b = 0.0, double _c = 0.0):
		x(_x), y(_y), z(_z), a(_a), b(_b), c(_c) {};

		double x;
		double y;
		double z;

		double a;
		double b;
		double c;
	};

}

#endif