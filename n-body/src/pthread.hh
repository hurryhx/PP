
#ifndef __PTHREAD__
#define __PTHREAD__

#include"worker.hh"

#include<pthread.h>

class PthreadWorker : public Worker{
	public:
		virtual int advance(const NBodyConfig &conf, double dtime);
		virtual std::string name() const {return "pthread";}
		void thread_calc_vel();
		void thread_calc_col();

		struct TaskSche{  //fetch the calc task
			public:
				pthread_mutex_t mutex;
				int cur, ntask, ntf_max;
				void init(int ntask, int ntf_max){
					this->ntask = ntask; cur = 0; this->ntf_max = ntf_max;
				}
				int fetch_task(int &left, int &right){
					pthread_mutex_lock(&mutex);
					int n = ntask - cur;
					if (n > ntf_max) n = ntf_max;
					left = cur;
					cur += n;
					right = cur;
					pthread_mutex_unlock(&mutex);
					return n;
				}

		};

		TaskSche *taskpool;
		double dtime;
		double threshold;
		int ncollide;
		const NBodyConfig *conf;

		Body *body_buf;
};

#endif
