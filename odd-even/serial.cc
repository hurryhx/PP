#include<sys/time.h>
#include<cassert>
#include"base.h"
#include"utils.h"
#include<cstdlib>
#include<iostream>

using namespace std;

int main(){
	int n = 40000;
	int *data = new int [n];
	For(i, n){
		data[i] = rand();
	}
	clock_t start = clock();
	For(p, n){
		int begin = 2 - (p&1);
		for (int i = begin; i < n; i+=2){
			if (data[i-1] > data[i]) swap(data[i-1], data[i]);
		}
	}
	cout << clock() - start << endl;
	For(i, n){
		assert(data[i-1] <= data[i]);
	}

	return 0;
}
