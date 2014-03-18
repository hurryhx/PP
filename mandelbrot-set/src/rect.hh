

#ifndef __RECT__
#define __RECT__

#include"comp.hh"

class Rect{
	public:
		double x, y, width, height;

		Rect(){}

		Rect(double x, double y, double width, double height):x(x), y(y), width(width), height(height){}
};

#endif
