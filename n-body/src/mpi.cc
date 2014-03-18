

#include<mpi.h>
#include"mpi.hh"
#include<string.h>

#define ROOT_PROC 0


MPIWorker::MPIWorker(){}

void MPIWorker::init(int argc, char* argv[]){
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npro);
	nworker = npro;
	MPI_Comm_rank(MPI_COMM_WORLD, &pro_id);

	body = NULL;

	MPI_Type_contiguous(sizeof(TaskConf), MPI_CHAR, &MPI_TYPE_CONF);
	MPI_Type_commit(&MPI_TYPE_CONF);
	MPI_Type_contiguous(sizeof(Body), MPI_CHAR, &MPI_TYPE_BODY);
	MPI_Type_commit(&MPI_TYPE_BODY);
}

int MPIWorker::get_task_per_proc(int ntask){
	int task_per_proc = ntask/npro;
	int task_remain = ntask - task_per_proc *npro;
	if (task_remain)
		task_per_proc ++;
	return task_per_proc;
}

int MPIWorker::decompose_task(int ntask, int &left, int &right){
	if (ntask < npro){
		if (pro_id < ntask){
			left = pro_id;
			right = pro_id + 1;
			return 1;
		}
		else return 0;
	}

	int task_per_proc = get_task_per_proc(ntask);

	left = task_per_proc * pro_id;
	right = task_per_proc * (pro_id + 1);
	if (right > ntask)
		right = ntask;
	return right - left;
}

void MPIWorker::work_vel(double dtime){
	ntask_assigned = decompose_task(nbody, task_start, task_end);
	double threshold = 0.5;
	for (int i = task_start; i < task_end; i++)
		engine->calc_vel(i, threshold, dtime);
}

void MPIWorker::work_col(double dtime){
	Body *new_body = new Body[task_end - task_start];
	memcpy(new_body, body+task_start, sizeof(Body)*(task_end-task_start));
	for (int i = task_start; i < task_end; i++){
		int p = i - task_start;
		Collision col = engine->collision_detect(i, dtime);
		if (col.item1 != -1){
			if (body_collide(new_body[p], body[col.item1], col.time)){
				new_body[p].advance(dtime - col.time);
			}
			else new_body[p].advance(dtime);
		}
		else if (col.time > 0){
			Body &b = new_body[p];
			b.advance(col.time);
			b.v[col.axis] *= -1;
			b.advance(dtime - col.time);
		}
		else new_body[p].advance(dtime);
	}
	memcpy(body+task_start, new_body, sizeof(Body) * task_end-task_start);
	delete [] new_body;
}

void MPIWorker::gather_body(){
	int task_per_proc = get_task_per_proc(nbody);
	MPI_Allgather(body + task_start, task_per_proc, MPI_TYPE_BODY, body, task_per_proc, MPI_TYPE_BODY, MPI_COMM_WORLD);
}

void MPIWorker::advanceall(double dtime){
	for (int i = 0; i < nbody; i++)
		body[i].advance(dtime);
}

int MPIWorker::advance(const NBodyConfig &conf, double dtime){
	if (pro_id == ROOT_PROC){
		if (body == NULL){
			int real_nbody = get_task_per_proc(conf.nbody) * npro;
			body = new Body[real_nbody];
		}
		memcpy(body, conf.body, sizeof(Body) * conf.nbody);
		nbody = conf.nbody;

		TaskConf tconf;
		tconf.type = NEW_CONF;
		tconf.nbody = conf.nbody;
		tconf.dtime = dtime;

		nbconf = conf;
		nbconf.body = body;

		master_conf(tconf, nbconf);
		engine->init(nbconf);

		work_vel(tconf.dtime);
		gather_body();

		work_col(tconf.dtime);
		gather_body();

		memcpy(conf.body, body, sizeof(Body) * nbody);
		return true;
	}
	else {
		TaskConf tconf;
		while(slave_conf(tconf, nbconf)){
			engine->init(nbconf);
			work_vel(tconf.dtime);

			gather_body();

			work_col(tconf.dtime);
			gather_body();
		}
		return false;
	}
}

bool MPIWorker::master_conf(TaskConf conf, const NBodyConfig &nbconf){
	MPI_Bcast(&conf, 1, MPI_TYPE_CONF, ROOT_PROC, MPI_COMM_WORLD);
	if (conf.type == EXIT_CONF)
		return true;

	MPI_Bcast(body, nbconf.nbody, MPI_TYPE_BODY, ROOT_PROC, MPI_COMM_WORLD);

	return true;
}

bool MPIWorker::slave_conf(TaskConf &conf, NBodyConfig &nbconf){
	MPI_Bcast(&conf, 1, MPI_TYPE_CONF, ROOT_PROC, MPI_COMM_WORLD);
	if (conf.type == EXIT_CONF)
		return false;
	nbconf.nbody = conf.nbody;
	nbody = nbconf.nbody;

	if (body == NULL)
	{
		body = new Body[get_task_per_proc(conf.nbody) * npro];
		nbconf.body = body;
	}
	MPI_Bcast(body, nbconf.nbody, MPI_TYPE_BODY, ROOT_PROC, MPI_COMM_WORLD);
	return true;
}

MPIWorker::~MPIWorker(){
	if (pro_id == ROOT_PROC){
		TaskConf conf;
		conf.type = EXIT_CONF;
		master_conf(conf, nbconf);
	}
	MPI_Finalize();
}


