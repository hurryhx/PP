#include<cstdlib>
#include<mpi.h>
#include<cmath>
#include<algorithm>
#include"mpi.hh"
#include<cassert>
#include"timer.hh"


#define TAG_APPLY 1
#define TAG_NEW 2
#define TAG_JOB_FINISHED 3
#define TAG_IMG 4



MPIRender::MPIRender(int argc, char* argv[], int maxtnum){
	this->maxtnum = maxtnum;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npro);
	if (npro == 1){
		exit(0);
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &pro_id);

	MPI_Type_contiguous(sizeof(ColorRGB), MPI_CHAR, &MPI_TYPE_COLOR);
	MPI_Type_contiguous(sizeof(GraphRenderConf), MPI_CHAR, &MPI_TYPE_GRAPHRENDERCONF);

	MPI_Type_commit(&MPI_TYPE_COLOR);
	MPI_Type_commit(&MPI_TYPE_GRAPHRENDERCONF);
}

MPIRender::~MPIRender(){
	if (pro_id == 0){
		GraphRenderConf conf;
		conf.nworker = -1;
		send_new_conf(conf);
	}
	MPI_Finalize();
}

void MPIRender::task_dis(){
	int range[2];
	MPI_Recv(range, 2, MPI_INT, MPI_ANY_SOURCE, TAG_JOB_FINISHED, MPI_COMM_WORLD, &status);

	int worker = status.MPI_SOURCE;
	if (range[0] != range[1]){
		assert(range[1] > range[0]);
		if (conf.is_res){
			ColorRGB *data;

			data = res->rgb->data + range[0];

			MPI_Recv(data, range[1] - range[0], MPI_TYPE_COLOR, worker, TAG_IMG, MPI_COMM_WORLD, &status);

		}
		pixels_finished += range[1] - range[0];
	}
	ts->fetch_task(range[0], range[1]);

	MPI_Send(range, 2, MPI_INT, worker, TAG_NEW, MPI_COMM_WORLD);
}

void MPIRender::send_result(const MPIRender::Task &task){
	int range[2] = {task.start, task.end};
	MPI_Send(range, 2, MPI_INT, 0, TAG_JOB_FINISHED, MPI_COMM_WORLD);
	if (conf.is_res && task.start != task.end){
		MPI_Send(task.rgb, task.end - task.start, MPI_TYPE_COLOR, 0, TAG_IMG, MPI_COMM_WORLD);
	}
}

void MPIRender::task_get(MPIRender::Task &task){
	int range[2];
	MPI_Recv(range, 2, MPI_INT, 0, TAG_NEW, MPI_COMM_WORLD, &status);
	task.start = range[0];
	task.end = range[1];
}

GraphRenderRes *MPIRender::render(Func *func, const GraphRenderConf &iconf){
	res = NULL;
	GraphRenderConf tconf = iconf;
	Timer timer;

	this->conf = tconf;
	if (pro_id == 0){
		send_new_conf(conf);
		if (conf.is_res)
			res = new GraphRenderRes(conf.width, conf.height);
		npixel = conf.width*conf.height;
		maxtnum = std::max(10000, (int)floor(ceil((double)npixel / npro)));
		maxtnum = std::min(maxtnum, 1000000);
		ts = new TaskSche(npixel, maxtnum);
		pixels_finished = 0;

		timer.set_start();
		while(pixels_finished != npixel)
			task_dis();
		timer.get_time();

		if (conf.is_res){
			for (int i = 0; i < conf.width; i++){
				for (int j = 0; j < conf.height/2; j++){
					int p0 = i * conf.height + j;
					int p1 = i * conf.height + (conf.height - 1 - j);
					std::swap(res->rgb->data[p0], res->rgb->data[p1]);
					std::swap(res->rgb->data[p0], res->rgb->data[p1]);
				}
			}
		}
		delete ts;

	}

	else {
		res = NULL;
		int alloc_pixel = 0;
		Task task;
		while (recv_new_conf(conf)){
			npixel = conf.width * conf.height;
			task.start = task.end = -1;
			if (conf.is_res && npixel > alloc_pixel){
				alloc_pixel = npixel;
				if (task.rgb) delete task.rgb;
				if (task.gray) delete task.gray;
				task.rgb = new ColorRGB[npixel];
				task.gray = new ColorRGB[npixel];

			}
			const Rect &domain = conf.domain;
			RenderTask rtask;

			while(1){
				send_result(task);
				task_get(task);

				if (task.start == task.end)
					break;

				int i = task.start / conf.height;
				int j = task.start % conf.height;
				int p = task.start;
				if (!conf.is_res){
					rtask.gpixel = NULL;
					rtask.cpixel = NULL;
				}
				for (int dpos = 0; p < task.end; p++, dpos++){
					rtask.num.real = domain.x + (double) i/conf.width*domain.width;
					rtask.num.imag = domain.y + (double) j/conf.height*domain.height;
					if (conf.is_res){
						rtask.gpixel = task.gray + dpos;
						rtask.cpixel = task.rgb + dpos;
					}
					func->render(rtask);
					j++;
					if (j == conf.height){
						i++;
						j = 0;
					}
				}
			}
		}
		if (conf.is_res)
		{
			delete task.gray;
			delete task.rgb;
		}
	}
	return res;
}


void MPIRender::send_new_conf(const GraphRenderConf &conf){
	GraphRenderConf buf = conf;
	MPI_Bcast(&buf, 1, MPI_TYPE_GRAPHRENDERCONF, 0, MPI_COMM_WORLD);
}

bool MPIRender::recv_new_conf(GraphRenderConf &conf){
	MPI_Bcast(&conf, 1, MPI_TYPE_GRAPHRENDERCONF, 0, MPI_COMM_WORLD);
	if (conf.nworker == -1)
		return false;
	return true;
}



