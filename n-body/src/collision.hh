
#ifndef __COLLISION__
#define __COLLISION__

#include<cmath>
#define G 6.673848
#define EPS 1e-06
#define REAL_MAX 10000000000.0
#define REAL_MIN -10000000000.0

struct Position{
	double x, y, z;
	Position(){}
	Position(double x, double y, double z):x(x), y(y), z(z){}
	Position operator + (const Position &v) const{
		return Position(x+v.x, y+v.y, z+v.z);
	}
	Position & operator += (const Position &v){
		x+=v.x, y+=v.y, z+=v.z;
		return *this;
	}
	Position operator - (const Position &v) const{
		return Position(x-v.x, y-v.y, z-v.z);
	}
	Position & operator -= (const Position &v){
		x-=v.x, y-=v.y, z-=v.z;
		return *this;
	}
	Position operator * (const double v) const{
		return Position(x*v, y*v, z*v);
	}
	Position & operator *= (const double v){
		x*=v, y*=v, z*=v;
		return *this;
	}
	Position operator / (const double v) const{
		return Position(x/v, y/v, z/v);
	}
	Position & operator /= (const double v){
		x/=v, y/=v, z/=v;
		return *this;
	}
	double dot(const Position &v) const{
		return x*v.x + y*v.y+ z*v.z;
	}
	double lengthsqr() const{
		return x*x+y*y+z*z;
	}
	double length() const{
		return sqrt(lengthsqr());
	}
	Position unit() const{
		return (*this) / length();
	}
	Position &unitize(){
		return (*this) /= length();
	}
	const double &coord(int axis) const{
		if (axis == 0)
			return x;
		if (axis == 1)
			return y;
		return z;
	}
	double &coord(int axis){
		if (axis == 0)
			return x;
		if (axis == 1)
			return y;
		return z;
	}
	double &operator [] (int axis){
		if (axis == 0) return x;
		if (axis == 1) return y;
		return z;
	}
	const double &operator [] (int axis) const{
		if (axis == 0) return x;
		if (axis == 1) return y;
		return z;
	}
};

struct Collision{
	int type;
	int item0, item1;
	int axis, mag;
	double time;
	double pos;

	Collision(int type = 0):type(type){}
	Collision(int item0,int item1, double time):item0(item0), item1(item1), time(time){}
};

#endif
