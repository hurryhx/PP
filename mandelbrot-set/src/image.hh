

#ifndef __IMAGE__
#define __IMAGE__

#include"comp.hh"

struct ColorRGB{
	double r, g, b;
	ColorRGB(){r = g = b = 0;}
	ColorRGB(double red, double green, double blue):r(red), g(green), b(blue){}
	ColorRGB(double gray):r(gray), g(gray), b(gray){}
};

struct Image{
	int width, height;
	ColorRGB *data;
	Image(int width, int height):width(width), height(height){
		data = new ColorRGB[width * height];
	}
	~Image(){
		delete[] data;
	}
};
#endif
