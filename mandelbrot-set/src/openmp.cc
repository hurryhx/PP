
#include"openmp.hh"
#include"timer.hh"


GraphRenderRes *OpenmpRender::render(Func *func, const GraphRenderConf &conf){
	const Rect &domain = conf.domain;
	int npixel = conf.width * conf.height;
	RenderTask *task = new RenderTask[npixel];
	GraphRenderRes *res = NULL;
	if (conf.is_res == true)
		res = new GraphRenderRes(conf.width, conf.height);
	int p = 0;
	for (int i = 0; i < conf.width; i++){
		for (int j = 0; j < conf.height; j++, p++){
			double x = domain.x + (double) i/conf.width * domain.width;
			double y = domain.y + (double) j/conf.height *domain.height;
			task[p].num = Complex(x, y);
			int t = i * conf.height + (conf.height-j-1);
			if (!conf.is_res){
				task[p].cpixel = NULL;
				task[p].gpixel = NULL;
			}
			else{
				task[p].cpixel = res->rgb->data+t;
				task[p].gpixel = res->rgb->data+t;
			}
		}

	}

	Timer timer;
	timer.set_start();

#pragma omp parallel for num_threads(conf.nworker) schedule(dynamic, 1)
	for (int i = 0; i < npixel; i++){
		func->render(task[i]);
	}

	int time = timer.get_time();
	printf("time: %d\n", time);

	return res;

}
