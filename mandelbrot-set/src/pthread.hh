


#ifndef __PTHREAD__
#define __PTHREAD__

#include<pthread.h>
#include<cstdio>
#include<string.h>
#include"render.hh"

#define NTF_MAX_DEFAULT 10000

class TaskPool{
	public:
		pthread_mutex_t m_mutex;

		RenderTask *task;
		int taskcnt, ntask;
		TaskPool(const GraphRenderConf &conf, GraphRenderRes *res){
			memset(&m_mutex, 0, sizeof(m_mutex));
			int npixel = conf.width*conf.height;
			task = new RenderTask[npixel];
			ntask = npixel;
			taskcnt=0;
			const Rect &domain = conf.domain;
			for (int i = 0, p = 0; i < conf.width; i++)
				for (int j = 0; j < conf.height; j++, p++){

					task[p].num.real = domain.x + (double)i/conf.width*domain.width;
					task[p].num.imag = domain.y + (double)j/conf.height*domain.height;
					if (!conf.is_res){
						task[p].cpixel = NULL;
						task[p].gpixel = NULL;
					}
					else{
						int dpos = i * conf.height + (conf.height-j-1);
						task[p].cpixel = res->rgb->data+dpos;
						task[p].gpixel = res->gray->data+dpos;
					}
				}
		}
		virtual double progress(){
			return (double) taskcnt / ntask;
		}

		virtual int fetch_task(int nnum, RenderTask *&mem){
			pthread_mutex_lock(&m_mutex);
			int n = ntask - taskcnt; //task remained
			if (nnum <= n) n = nnum; //if enough
			mem = task + taskcnt; //next pos
			taskcnt+=n;
			pthread_mutex_unlock(&m_mutex);
			return n;
		}
		virtual ~TaskPool(){delete [] task;}
};

class PthreadRender : public GraphRender{
	public:
		int m_ntf_max;

		virtual void dorender(Func *func, TaskPool *taskpool);
		friend void *render_call(void *arg);

//		PthreadRender(int ntf_max = NTF_MAX_DEFAULT):m_ntf_max(ntf_max){}
		PthreadRender(){m_ntf_max = NTF_MAX_DEFAULT;}
		virtual GraphRenderRes *render(Func *func, const GraphRenderConf &conf);
};

struct PthreadArg{
	PthreadRender *render;
	TaskPool *taskpool;
	Func *func;
};

#endif
