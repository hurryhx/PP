
#ifndef __NBODY__
#define __NBODY__
#include"collision.hh"
#include"body.hh"
#include<cstdlib>

struct NBody{
	Position min_coord, max_coord; //Range from min to max for n body
	NBody(){}
	NBody(const Position &min, const Position &max):min_coord(min), max_coord(max){}
	double length(int axis) const{
		return max_coord[axis] - min_coord[axis];
	}
};

struct NBodyConfig{
	Body *body;
	int nbody;
	NBody domain;
	void init(const NBody &domain, int n);
	void readfile(const char* filename);

	NBodyConfig(){body = NULL;}
	~NBodyConfig(){if(body) delete[]body;}
};

#endif
