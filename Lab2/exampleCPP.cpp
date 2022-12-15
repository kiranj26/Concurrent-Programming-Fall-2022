#include <barrier>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <ctime>


using namespace std;

vector<thread*> threads;
size_t NUM_THREADS;

/*
Note that the barrier requires C++20 (use flag --std=c++2a)  
If you don't have access to C++20 on your local machine,
you'll get a #include <barrier> no such file error on compilation.
Jupyter does have C++20 installed.

Note that you can always use a pthread barrier instead, even in C++.
just #include<pthread> and see the C example for usage.
*/
barrier<> *bar; 
mutex* lk;

struct timespec startTime, endTime;

void global_init(){
	bar = new barrier(NUM_THREADS);
	lk = new mutex();
}
void global_cleanup(){
	delete bar;
	delete lk;
}

void local_init(){}
void local_cleanup(){}

void* thread_main(size_t tid){
	local_init();
	bar->arrive_and_wait();
	if(tid==1){
		clock_gettime(CLOCK_MONOTONIC,&startTime);
	}
	bar->arrive_and_wait();
	
	// do something
	printf("Thread %zu reporting for duty\n",tid);
	lk->lock();
	printf("Thread %zu got the lock!\n",tid);
	lk->unlock();

	bar->arrive_and_wait();
	if(tid==1){
		clock_gettime(CLOCK_MONOTONIC,&endTime);
	}
	local_cleanup();
	
	return 0;
}




int main(int argc, const char* argv[]){
	
	// parse args
	if(argc==2){
		NUM_THREADS = atoi( argv[1] );
		if(NUM_THREADS > 150){
			printf("ERROR; too many threads\n");
			exit(-1);
		}
	}
	else{
		NUM_THREADS = 5;
	}
	
	
	global_init();
	
	// launch threads
	int ret; size_t i;
	threads.resize(NUM_THREADS);
	for(i=1; i<NUM_THREADS; i++){
		threads[i] = new thread(thread_main,i+1);
	}
	i = 1;
	thread_main(i); // master also calls thread_main
	
	// join threads
	for(size_t i=1; i<NUM_THREADS; i++){
		threads[i]->join();
		printf("joined thread %zu\n",i+1);
		delete threads[i];
	}
	
	global_cleanup();
	
	unsigned long long elapsed_ns;
	elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);
}