

#ifndef __SIMPLE__
#define __SIMPLE__

#include"engine.hh"
#include"AABox.hh"

class SimpleEngine : public Engine{
	public:
		virtual void init(const NBodyConfig &conf);
		virtual Collision collision_detect(int id, double dtime);

		virtual void calc_vel(int id, const double &threshold, const double &dtime);

		NBodyConfig conf;
		AABox boundary;
		Body *body;
};


#endif
