
#ifndef __MPI__
#define __MPI__


#include"worker.hh"
#include"nbody.hh"

#include<mpi.h>

#define NEW_CONF 1
#define EXIT_CONF 0

enum MPITaskType{
	MPI_VEL,
	MPI_POS,
	MPI_END
};
struct MPITask{
	MPITaskType type;
	int left, right;
};

struct TaskConf{
	int type, nbody;
	double dtime;
};

class MPIWorker : public Worker{
	public:
	virtual void init(int argc, char* argv[]);
	virtual int advance(const NBodyConfig &conf, double dtime);
	virtual std::string name() const{return "MPI";}
	virtual int get_nworker() const {return npro;}

	MPIWorker();
	~MPIWorker();

	MPI_Datatype MPI_TYPE_CONF, MPI_TYPE_BODY;
	NBodyConfig nbconf;
	int npro, pro_id;
	int ntask_assigned;
	int task_start, task_end;
	int decompose_task(int ntask, int &left, int &right);
	int get_task_per_proc(int ntask);
	void work_vel(double dtime);
	void work_col(double dtime);
	void advanceall(double dtime);
	void gather_body();

	bool master_conf(TaskConf conf, const NBodyConfig &nbconf);
	Body *body;
	int nbody;

	bool slave_conf(TaskConf &conf, NBodyConfig &nbconf);

};


#endif
