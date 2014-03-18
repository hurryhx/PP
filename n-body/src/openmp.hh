

#ifndef __OPENMP__
#define __OPENMP__

#include"worker.hh"
#include"engine.hh"
#include<string>

class OpenmpWorker : public Worker
{
	public:
		bool *hash;
		Body *new_body;
		int work(const NBodyConfig &conf, int id, double dtime);

		virtual int advance(const NBodyConfig &conf, double dtime);
		virtual std::string name() const{return "openmp";}
};


#endif
