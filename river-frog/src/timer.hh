#include<sys/time.h>


class Timer{
	public:
		timeval t1, t2;
		void set_start(){
			gettimeofday(&t1, NULL);
		}
		void set_stop(){
			gettimeofday(&t2, NULL);
		}
		int get_time(){
			int res = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000;
			return res;
		}
};
