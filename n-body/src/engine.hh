
#ifndef __ENGINE__
#define __ENGINE__

#include"nbody.hh"
#include"collision.hh"

class Engine{
	public:
		virtual void init(const NBodyConfig &conf) = 0;
		virtual Collision collision_detect(int id, double dtime) = 0;
		virtual void calc_vel(int id, const double &threshold, const double &dtime) =0;
};

#endif
