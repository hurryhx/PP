
#include"pthread.hh"

#include<pthread.h>
#include<cstring>


struct PthreadArg{
	PthreadWorker *worker;
	int task_type;
};

static void *call_pthread(void *arg){ //every thread calc of vel and col
	PthreadArg *parg = static_cast<PthreadArg *>(arg);
	if (parg->task_type == 0)
		parg->worker->thread_calc_vel();
	else
		parg->worker->thread_calc_col();
	pthread_exit(NULL);
}

void PthreadWorker::thread_calc_vel(){
	int left, right;
	while (taskpool->fetch_task(left, right)){
		for (int i = left; i < right; i++)
			engine->calc_vel(i, threshold, dtime);
	}
}

void PthreadWorker::thread_calc_col(){
	int left, right;
	Body *body = conf->body;
	Body *new_body = body_buf;
	while(taskpool->fetch_task(left, right)){
		for (int i = left; i < right; i++){
			int p = i;
			Collision col = engine->collision_detect(p, dtime);
			if (col.item1 != -1){
				if (body_collide(new_body[p], body[col.item1], col.time)){
					ncollide++;
					new_body[p].advance(dtime - col.time);
				}
				else new_body[p].advance(dtime);
			}
			else if(col.time > 0){ //boundary
				ncollide ++;
				Body &b = new_body[p];
				b.advance(col.time);
				b.v[col.axis] *= -1;
				b.advance(dtime-col.time);
			}
			else new_body[p].advance(dtime);
		}
	}
}


int PthreadWorker::advance(const NBodyConfig &conf, double dtime){
	taskpool = new TaskSche();
	this->conf = &conf;
	this->dtime = dtime;
	threshold = 1;
	engine->init(conf);

	pthread_t *threads = new pthread_t[nworker];
	PthreadArg arg;
	arg.worker = this;
	arg.task_type = 0;
	int ntf_max = conf.nbody / nworker / 8;
	if (ntf_max < 10) ntf_max = 10;
	if (ntf_max > 100) ntf_max = 100;

	taskpool->init(conf.nbody, ntf_max);
	for (int i = 0; i < nworker; i++)
		pthread_create(threads + i, NULL, call_pthread, &arg);
	for (int i = 0; i < nworker; i++)
		pthread_join(threads[i], NULL);

	ncollide = 0;
	arg.task_type = 1;
	taskpool->init(conf.nbody, ntf_max);
	body_buf = new Body[conf.nbody];
	memcpy(body_buf, conf.body, sizeof(Body)*conf.nbody);
	for (int i = 0; i < nworker; i++)
		pthread_create(threads+i, NULL, call_pthread, &arg);
	for (int i = 0; i < nworker; i++)
		pthread_join(threads[i], NULL);
	memcpy(conf.body, body_buf, sizeof(Body)*conf.nbody);

	delete threads;
	delete taskpool;
	delete body_buf;
	return true;
}

