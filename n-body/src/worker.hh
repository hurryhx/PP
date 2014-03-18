
#ifndef __WORKER__
#define __WORKER__

#include<string>
#include"nbody.hh"
#include"engine.hh"

class Worker{
	public:
		int nworker;
		Engine *engine;
		virtual void init(int argc, char* argv[]){}
		virtual std::string name() const{}
		virtual int advance(const NBodyConfig &conf, double dtime){}
		void set_engine(Engine *engine){this->engine = engine;}
		void set_nworker(int n){this->nworker = n;}
		virtual int get_nworker(){return nworker;}
		virtual ~Worker(){}
};
#endif
