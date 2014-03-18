
#ifndef __AABOX__
#define __AABOX__

#include"body.hh"
#include<cstdlib>
#include<cmath>

#define REAL_MAX 10000000000.0
#define REAL_MIN -10000000000.0

struct AABox{
	Position min_coord, max_coord;
	inline double volume()const{
		double ret = 1;
		for (int i = 0; i < 3; i++){
			ret *= max_coord[i] - min_coord[i];
		}
		return ret;
	}

	void reset(){
		min_coord = Position(REAL_MAX, REAL_MAX, REAL_MAX);
		max_coord = Position(REAL_MIN, REAL_MIN, REAL_MIN);
	}

	bool in_box(const Position &pos) const{
		for (int i = 0; i < 3;i++){
			if (pos[i] > max_coord[i] || pos[i] < min_coord[i])
				return false;
		}
		return true;
	}

	AABox enhance(double width) const{
		AABox ret(*this);
		for (int i = 0; i < 3; i++){
			ret.max_coord.coord(i) += width;
			ret.min_coord.coord(i) -= width;
		}
		return ret;
	}

	bool collide(const Position &pos, const Position &vel, double dtime)const{
		if (in_box(pos))
			return true;
		Position eps = vel.unit() * EPS;
		for (int axis = 0; axis < 3; axis++){
			double v = vel[axis];
			if (fabs(v) < EPS) continue;

			double dmin = min_coord[axis] - pos[axis];
			double tmin = dmin / v;
			if (tmin > 0 && tmin <= dtime && in_box(pos+vel*tmin+eps)) return true;
			double dmax = max_coord[axis] - pos[axis];
			double tmax = dmax / v;
			if  (tmax > 0 && tmax <= dtime && in_box(pos+vel*tmax+eps)) return true;
		}
		return false;
	}
	bool collide_info(const Body &body, double &time, int &axis, int &mag){
		time = REAL_MAX;
		for (int i = 0; i < 3; i++){
			double v = body.v[i];
			if (fabs(v) < EPS) continue;
			double dmax = max_coord[i] - body.pos[i] - body.r;
			double tmax = dmax / v;
			if (tmax > 0 && tmax < time){
				time = tmax; axis = i; mag = 1;
			}
			double dmin = min_coord[i] - body.pos[i] + body.r;
			double tmin = dmin / v;
			if (tmin > 0 && tmin < time){
				time = tmin; axis = i; mag = 0;
			}
		}
		return time != REAL_MAX;
	}
};

#endif
