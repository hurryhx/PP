

#include<cstring>
#include"pthread.hh"
#include"timer.hh"
#include<cstdio>

void *render_call(void *arg){
	PthreadArg *parg = static_cast<PthreadArg*>(arg);
	parg->render->dorender(parg->func, parg->taskpool);
	pthread_exit(NULL);
}

GraphRenderRes *PthreadRender::render(Func *func, const GraphRenderConf &conf){
	int nthread = conf.nworker;

	GraphRenderRes * res = NULL;
	if (conf.is_res == true){
		res = new GraphRenderRes(conf.width, conf.height);
	}

	TaskPool *taskpool = new TaskPool(conf, res);
	pthread_t *threads = new pthread_t[nthread];
	PthreadArg *parg = new PthreadArg[nthread];

	Timer timer;
	timer.set_start();

	for (int i = 0; i < nthread; i++){
		PthreadArg *arg = parg + i;
		arg->render = this;
		arg->taskpool = taskpool;
		arg->func = func;
		pthread_create(&threads[i], NULL, render_call, arg);
	}

	for (int i = 0; i < nthread; i++)
		pthread_join(threads[i], NULL);
	int time = timer.get_time();
	printf("time: %d\n", time);

	delete taskpool;
	delete[] threads;
	delete[] parg;
	return res;
}

void PthreadRender::dorender(Func *func, TaskPool *taskpool){
	RenderTask *tasks;
	int ntask, tottask = 0;
	while((ntask = taskpool->fetch_task(m_ntf_max, tasks))){
		tottask += ntask;
		for (int i = 0; i < ntask; i++)
			func->render(tasks[i]);
	}
}
