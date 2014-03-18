
#ifndef __BODY__
#define __BODY__

#include"collision.hh"

class ColorRGB{
	public:
		double r, g, b;
		ColorRGB(){r=g=b=255;}
		ColorRGB(double r, double g, double b):r(r), g(g), b(b){}
};

class Body{
	public:
		ColorRGB color;
		int id;
		Position pos, v;
		double mass;
		double r;

		double lengthsqr(const Body &b1, const Body &b2){
			double dx = b1.pos.x - b2.pos.y;
			double dy = b1.pos.y - b2.pos.y;
			return dx*dx+dy*dy;
		}

		bool is_collide(const Body &b){
			if (lengthsqr(*this, b) > this->r*this->r + b.r*b.r)
				return false;
			return true;
		}

		bool intersect(const Body &b) const{
			double distsqr = (pos - b.pos).lengthsqr();
			double rsum = (r+b.r)*(r+b.r);
			return distsqr < rsum;
		}

		void set_pos(const Position &p){this->pos = p;}
		void move(const Position &dpos){pos += dpos;}
		void advance(const double &dtime){pos += v*dtime;}
		double boundary(int cid, int type){
			double t;
			if (cid == 0) t = pos.x;
			else if (cid == 1) t = pos.y;
			else t = pos.z;
			return t + (type == 0 ? -1:1) * r;
		}



};

bool body_collide(Body &b1, Body &b2, double dtime, bool modify_b = false);
Collision body_clsdtct(Body &id0, Body &id1, double dtime);
Position calc_acc(const Position &p0, const Position &p1, double m1);

#endif
