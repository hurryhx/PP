

#ifndef __MPIR__
#define __MPIR__


#include<mpi.h>
#include"render.hh"

struct TaskSche{
	int tasknum, maxtnum, taskcnt;
	TaskSche(int tasknum, int maxtnum):tasknum(tasknum), maxtnum(maxtnum), taskcnt(0){}
	void fetch_task(int &start, int &end){
		int n = tasknum-taskcnt;
		if (n > maxtnum) n = maxtnum;
		start = taskcnt;
		end = taskcnt+n;
		taskcnt = end;
	};
	bool finished(){return tasknum == taskcnt;}
};

class MPIRender : public GraphRender{
	public:

		int npro, pro_id;

		MPI_Datatype MPI_TYPE_COLOR;
		MPI_Datatype MPI_TYPE_GRAPHRENDERCONF;

		MPI_Status status;

		int maxtnum;

		int npixel;

		GraphRenderConf conf;

		TaskSche *ts;
		MPI_Request send_request;
		int pixels_finished;
		GraphRenderRes *res;

		void task_dis();

		struct Task{
			int start, end;
			ColorRGB *rgb, *gray;
			Task():rgb(NULL){}
		};

		void send_result(const Task &task);
		void task_get(Task &task);

		void send_new_conf(const GraphRenderConf &conf);
		bool recv_new_conf(GraphRenderConf &conf);

	public:
		MPIRender(int argc, char *argv[], int maxtnum);
		~MPIRender();
		virtual GraphRenderRes *render(Func *func, const GraphRenderConf &conf);
		int rank() const {return pro_id;}
};

#endif
