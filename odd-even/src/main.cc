#include<iostream>
#include<mpi.h>
#include"base.h"
#include"utils.h"
#include"mpiutils.h"
#include<cstdlib>
#include<cassert>

using namespace std;

int npro;
int pro_id;
int *data;
int ndata;
int npdata;

void gen(){
	if (pro_id == 0){
		for (int i = 1; i < npro; i++){
			int seed = rand();
			sendint(seed, i, 1);
		}
		for (int i = 0; i < npdata; i++){
			data[i] = rand();
		}
	}
	else{
		int seed;
		recvint(seed, 0, 1);
		srand(seed);
		for (int i = 0; i < npdata; i++){
			data[i] = rand();
		}
	}
}

void sort(){

	For(phase, ndata){
		int isodd = phase & 1;

		for (int i = isodd+1; i < npdata; i+=2){
			if (data[i-1] > data[i])
				swap(data[i-1], data[i]);
		}

		int rec;
		if (isodd == 1){
			for (int i = 0; i < 2; i++){
				if ((pro_id & 1)==i){
					if (pro_id > 0){
						MPI_Recv(&rec, 1, MPI_INT, pro_id-1, 0, MPI_COMM_WORLD, &status);
						if (rec > data[0]) {swap(rec, data[0]);
						}
						MPI_Send(&rec, 1, MPI_INT, pro_id-1, 0, MPI_COMM_WORLD);
					}
				}

				else{
					if (pro_id < npro-1){
						MPI_Send(&data[npdata-1], 1, MPI_INT, pro_id+1, 0, MPI_COMM_WORLD);
						MPI_Recv(&data[npdata-1], 1, MPI_INT, pro_id+1, 0, MPI_COMM_WORLD, &status);
					}
				}
			}
		}
	}
}

bool check(){
	for (int i = 1; i < npdata; i++)
		assert(data[i-1] <= data[i]);
	For(i, 2){
		if ((pro_id & 1) == i){
			if (pro_id > 0){
				int rec;
				MPI_Recv(&rec, 1, MPI_INT, pro_id-1, 2, MPI_COMM_WORLD, &status);
				assert(rec <= data[0]);
			}
		}
		else{
			if (pro_id < npro-1){
				MPI_Send(&data[npdata-1], 1, MPI_INT, pro_id+1, 2, MPI_COMM_WORLD);
			}
		}
	}
	return 1;
}




int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &npro);
	MPI_Comm_rank(MPI_COMM_WORLD, &pro_id);
	assert(argc == 2);
	ndata = atoi(argv[1]);
	npdata = ndata / npro;

	data = new int [npdata];

	gen();

	double start = MPI_Wtime();
	sort();
	double end = MPI_Wtime();
	bool if_chk = check();
	if (if_chk == 1 && pro_id == 0) cout << "Have sorted!" << pro_id << ":" << end-start << endl;
	MPI_Finalize();
	return 0;

}
