
#include"openmp.hh"
#include"collision.hh"
#include"engine.hh"

#include<cstring>
#include<cassert>

int OpenmpWorker::advance(const NBodyConfig &conf, double dtime){
	int ncollide = 0;
	double threshold = 0.85;

	double time_remain = dtime;
	while (time_remain){
		assert(time_remain > 0);
		engine->init(conf);

#pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
		for (int i = 0; i < conf.nbody; i++)
			engine->calc_vel(i, threshold, dtime);
		Collision min_col;
		min_col.time = REAL_MAX;

#pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
		for (int i = 0; i < conf.nbody; i++){
			Collision col = engine->collision_detect(i, dtime);
			if (col.item0 == -1)
				continue;
			if (col.time > 0 && col.time < min_col.time)
				min_col = col;
		}
		if (min_col.time < 0 || min_col.time > time_remain)
			break;
		ncollide++;
		time_remain -= min_col.time;

#pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
		for (int i = 0; i < conf.nbody; i++)
			if (i != min_col.item0 && i != min_col.item1)
				conf.body[i].advance(min_col.time);
		if (min_col.item1 == -1)
			conf.body[min_col.item0].v[min_col.axis] *= -1;
		else
			body_collide(conf.body[min_col.item0], conf.body[min_col.item1], min_col.time, true);
	}

#pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
	for (int i = 0; i < conf.nbody; i++)
		conf.body[i].advance(time_remain);

#pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
	for (int i = 0; i < conf.nbody; i++)
		engine->calc_vel(i, threshold, dtime);

	Body *new_body = new Body[conf.nbody];
	memcpy(new_body, conf.body, sizeof(Body)*conf.nbody);


#pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
	for (int i = 0; i < conf.nbody; i++)
	{
		int p = i;
		Collision col = engine->collision_detect(i, dtime);
		if (col.item1 != -1){
			if (body_collide(new_body[p], conf.body[col.item1], col.time))
			{
				ncollide++;
				new_body[p].advance(dtime - col.time);
			}
			else new_body[p].advance(dtime);
		}
		else if(col.time > 0){
			ncollide++;
			Body &b = new_body[p];
			b.advance(col.time);
			b.v[col.axis] *= -1;
			b.advance(dtime - col.time);
		}
		else new_body[p].advance(dtime);
	}
	memcpy(conf.body, new_body, sizeof(Body) * conf.nbody);
	delete [] new_body;

	return true;
}


