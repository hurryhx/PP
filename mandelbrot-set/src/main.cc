#include<cstdio>
#include<cstdlib>
#include<string>
#include<getopt.h>
#include<unistd.h>

#include"mandelbrot.hh"
#include"gtk.hh"
#include"rect.hh"
#include"render.hh"
#include"mpi.hh"
#include"openmp.hh"
#include"pthread.hh"


GraphRender *GraphRenderFactory(int category, int argc, char*argv[]){
	switch(category){
		case 0:
			return new MPIRender(argc, argv, 100000);
			break;
		case 1:
			return new OpenmpRender();
			break;
		case 2:
			return new PthreadRender();
			break;
	}
	exit(0);
}

char* name;



int main(int argc, char* argv[]){
	name = argv[0];

//	int width = atoi(argv[1]), height = atoi(argv[2]);
//	int niter_max = atoi(argv[3]);
	int width = 600, height = 600, niter_max = 2048;

	bool show = true;

	int parallel_method = 1;

	int nworker = sysconf(_SC_NPROCESSORS_ONLN);

	option options[] = {
		{"nworker", required_argument, NULL, 'n'},
		{"niter", required_argument, NULL, 'i'},
		{"type", required_argument, NULL, 't'}
	};
	int opt;
	while ((opt = getopt_long(argc, argv, "t:n:i:", options, NULL)) != -1){
		switch(opt){
			case 't':
				parallel_method = atoi(optarg);
				break;
			case 'i':
				niter_max = atoi(optarg);
				break;
			case 'n':
				nworker = atoi(optarg);
		}

	}

	bool is_res = true;

	rcfg.gc.domain.x = 1000;
	rcfg.gc.domain.y = 1;

	rcfg.rdr = GraphRenderFactory(parallel_method, argc, argv);
	rcfg.func = new Mandelbrot(Setting(TERM_FLAG_NITER, niter_max));
	Rect rect(-1, -1, 2, 2);
	rcfg.gc = GraphRenderConf(width, height, rect, nworker, is_res);
	if(!rcfg.render())
		return 0;

	image_init(argc, argv);
	rcfg.to_pixbuf();

//	if (is_res == true && rcfg.output){
//		img2ppm(rcfg.output->rgb, coutput);
//	}
	if (show)
		image_show();

}
