
#include"gtk.hh"
#include"nbody.hh"
#include"pthread.hh"
#include"openmp.hh"
#include"mpi.hh"
#include"bsp.hh"
#include"simple.hh"
#include"AABox.hh"

#include <string>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <cstring>

Worker *worker_factory(const char *name)
{
	std::string w = name;
	if (w == "openmp")
		return new OpenmpWorker();
	if (w == "pthread")
		return new PthreadWorker();
	if (w == "mpi")
		return new MPIWorker();
	fprintf(stderr, "Unkown worker: %s\n", name);
	exit(-1);
}


int main(int argc, char *argv[])
{
	srand(2);
	progname = argv[0];

	int nworker = sysconf(_SC_NPROCESSORS_ONLN);
	option long_options[] = {
		{"nworker",		required_argument,	NULL, 'n'},
		{"size",		required_argument,	NULL, 's'},
		{"silent",		no_argument,		NULL, 'w'},
		{"parallel",	required_argument,	NULL, 'p'},
		{"input",		required_argument,	NULL, 'i'},
		{"nstar",		required_argument,	NULL, 'r'},
		{"runtime",		required_argument,	NULL, 't'},
		{"fps-test",	no_argument,		NULL, 'f'},
		{"interval",	required_argument,	NULL, 'l'},
	};

	int width = 600, height = 600;
	std::string para = "pthread", input = "";
	int nstar = 20;
	int opt;
	double time_interval = 0.01;
	bool fpstest = false;
	while ((opt = getopt_long(argc, argv, "l:fr:t:n:s:wp:i:", long_options, NULL)) != -1)
	{
		switch (opt)
		{
			case 'l':
				time_interval = str2real(optarg);
				break;
			case 'f':
				fpstest = true;
				break;
			case 't':
				runtime = str2real(optarg);
				break;
			case 'r':
				nstar = str2num(optarg);
				break;
			case 'n':
				nworker = str2num(optarg);
				break;

			case 's':
				{
					int w, h;
					if (sscanf(optarg, "%dx%d", &w, &h) != 2)
						return 0;
					width = w, height = h;
				}
				break;

			case 'w':
				show_window = false;
				break;

			case 'p':
				para = optarg;
				break;

			case 'i':
				input = optarg;
				break;
		}
	}

	show_init(argc, argv);

	NBody domain(Position(0, 0, 0), Position(600, 600, 100));
	rconf.nbody.init(domain, nstar);
	if (input.length())
		rconf.nbody.readfile(input.c_str());

	rconf.nbody.domain = domain;
	rconf.wwidth = width, rconf.wheight = height;

	Worker *worker = worker_factory(para.c_str());
	worker->init(argc, argv);
	worker->set_nworker(nworker);
	worker->set_engine(new BSPTree());
	rconf.set_worker(worker);
	rconf.init();
	gtimer.begin();
	if (fpstest)
	{
		while (true)
		{
			if (gtimer.end() > runtime * 1000)
				break;
			frame_count ++;
			if (!rconf.advance(time_interval))
				break;
			rendered = true;
		}
	}
	else if (rconf.advance(0.01))
		show();
	exit_on_timeout();
	return 0;
}


