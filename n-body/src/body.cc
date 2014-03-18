

#include"body.hh"
#include"collision.hh"
#include<cassert>

bool body_collide(Body &b1, Body &b2, double dtime, bool modify){

	b1.advance(dtime);
	Body *b, btmp;
	if (modify) b = &b2;
	else b = &btmp, btmp = b2;
	b->advance(dtime);
	double dist = (b1.pos - b->pos).lengthsqr();
	double rsum = (b1.r + b->r) * (b1.r+b->r);

	if (dist <= rsum){
		double delta = (rsum - dist) / 2 * 1.1 + EPS;
		Position d = b->pos - b1.pos;
		b1.set_pos(b1.pos - d * delta);
		b->set_pos(b->pos + d*delta);

	}
	assert(!b1.intersect(*b));

	double m1, m2, x1, x2;
	Position v1, v2, v1x, v2x, v1y, v2y, x(b1.pos- b->pos); // sub vector

	x.unitize();
	v1 = b1.v;
	x1 = x.dot(v1); // in direction vector x1
	v1x = x*x1;
	v1y = v1-v1x;
	m1 = b1.mass;

	x = x * -1;
	v2 = b->v;
	x2 = x.dot(v2);
	v2x = x*x2;
	v2y = v2 - v2x;
	m2 = b->mass;

	double s = m1+m2; //momentum
	b1.v = v1x*(m1-m2)/s+v2x*(2*m2)/s+v1y;
	b->v = v1x*(2*m1)/s+v2x*(m2-m1)/s+v2y;

	return true;
}


static bool solve_equation(const double &a, const double &b, const double &c, double &r0, double &r1){
	if (fabs(a) < EPS){
		return false;
	}

	double delta = b*b-4*a*c;
	if(delta >= 0){
		double sq = sqrt(delta);
		double d = 1.0/(2*a);
		r0 = (-b-sq)*d;
		r1 = (-b+sq)*d;
		return true;
	}
	return false;
}

Collision body_clsdtct(Body &b0, Body &b1, double dtime){
	int id0 = b0.id;
	int id1 = b1.id;
	if (id0 == id1)
		return Collision(-1, -1, -1);
	Position AB = b1.pos - b0.pos;
	Position vab = b0.v*(-1) + b1.v;
	double rab = b0.r+b1.r;
	double a = vab.dot(vab), b = 2*vab.dot(AB), c = AB.dot(AB)-rab*rab;

	double t0, t1, t;
	if (!solve_equation(a,b,c,t0,t1))
		return Collision(-1,-1,-1);
	t = t0;
	if (t < 0) t = t1;
	if (t < 0 || t > dtime) return Collision(-1,-1,-1);
	if (t > EPS)
		t -= EPS/2;
	if (t > dtime)
		t = -1;
	return Collision(id0, id1, t);
}

Position calc_acc(const Position &p0, const Position &p1, double m1){
	Position d = p1 - p0;
	double mag = G * m1 / (d.lengthsqr());
	return d/d.length() * mag;
}
