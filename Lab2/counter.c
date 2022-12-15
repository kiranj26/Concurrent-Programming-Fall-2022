/***********************************************/
/***********************************************/
/*     Lab 2  : Concurrent Programming         */
/*     Author : Kiran Jojare                   */
/***********************************************/
/***********************************************/

/* Include Files */
#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/time.h>
#include <stdbool.h>
#include<time.h>
#include<limits.h>
#include<stdatomic.h>
#include "threads.h"
#include "assert.h"
#define TRUE 1
#define FALSE 0

int THREAD_CNT;
int ITERATIONS;
int bar_cntr  = 0;
int lock_cntr = 0;

/* Pthread Variable */
pthread_barrier_t bar;
pthread_mutex_t lk;

/* Atomic Variables */
_Atomic _Bool * tas_lock_flag      = 0;
_Atomic _Bool * ttas_lock_flag     = 0;
_Atomic int   * ticket_next_num    = 0;
_Atomic int   * ticket_now_serving = 0;
_Atomic int   * bar_sense_cnt      = 0;
_Atomic int   * bar_sense_sense    = 0;

/* Structure definations for MCS lock */
struct spinlock_node {
   _Atomic struct spinlock_node *next;
   _Atomic _Bool wait;
}node;


struct spinlock {
   _Atomic struct spinlock_node * tail;
}lock;

/* Atomic variables for peterson algorithm  */
_Atomic _Bool * desires_seq;
_Atomic int     turn_seq;
_Atomic _Bool * desires_rel;
_Atomic int     turn_rel;

/* Structure to clock time */
struct timespec start, end;

/* Function Declarations */ 
void* bar_thread_counter(void* arg);
void* lock_thread_counter(void* arg);

void* lock_tas_counter(void* arg);
void tas_lock(void);
void tas_unlock(void);

void* lock_ttas_counter(void* arg);
void ttas_lock(void);
void ttas_unlock(void);

void* lock_ticket_counter(void* arg);
void ticket_lock(void);
void ticket_unlock(void);

void* bar_sense_counter(void* arg);
void barrier_sense_wait(void);

void* lock_mcs_counter(void* arg);
void mcs_lock(struct spinlock *lock, struct spinlock_node *my_node);
void mcs_unlock(struct spinlock *lock, struct spinlock_node *my_node);
void MCS_lock(_Atomic struct spinlock *lock, _Atomic struct spinlock_node *node);
void MCS_unlock(_Atomic struct spinlock *lock, _Atomic struct spinlock_node *node);

void* lock_peterseq_counter(void* arg);
void peterseq_lock_init(void);
void peterseq_lock(int tid);
void peterseq_unlock(int tid);

void* lock_peterrel_counter(void* arg);
void peterrel_lock_init(void);
void peterrel_lock(int tid);
void peterrel_unlock(int tid);
/***********************************************/
/***********************************************/
/*                  Main                       */
/***********************************************/
/***********************************************/

int main(int argc, char *argv[])
{
	/* Character Initialisation */
	int c             =  0;
	int option_index  =  0;
	char * bar_selected;
	char * lock_selected;
	char * dest_txt_selected;
	FILE * dest_file;
	
	bar_selected      =  (char*)malloc(10*sizeof(char));
	lock_selected     =  (char*)malloc(25*sizeof(char));
    dest_txt_selected =  (char*)malloc(50*sizeof(char));
    desires_seq       =  (_Atomic _Bool *)malloc(2*sizeof(_Atomic _Bool *));
    desires_rel       =  (_Atomic _Bool *)malloc(2*sizeof(_Atomic _Bool *));

	
	/* Command Line Args Parsing */
	static struct option long_options[] ={
    {"name",0,0,'n'},              // long option 1
    {"thread",1,0,'t'},            // long option 2
    {"iter",1,0,'i'},              // long option 3
	{"bar",1,0,'b'},               // long option 3
	{"lock",1,0,'l'},              // long option 5
	{"dest",1,0,'o'},              // long option 6
    {0,0,0,0},
    };
	
	/* Processing Command Line Args */
	while((c = getopt_long(argc,argv, "nt:i:b:l:o:", long_options,&option_index)) != -1)
    {
        switch (c)
        {
        case 'n':
            printf("Student name : Kiran Narendra Jojare\n");
            exit(1); //print name and exit code
            break;
        case 't':
            THREAD_CNT = atoi(optarg);
			printf("No of threads Selected : %d\n",THREAD_CNT);

            break;
        case 'b':
            strcpy(bar_selected, optarg);
            printf("Barrier Selected : %s\n",bar_selected);
            break;
		case 'l':
            strcpy(lock_selected, optarg);
            printf("Lock Selected : %s\n",lock_selected);
            break;
		case 'i':
            ITERATIONS = atoi(optarg);
			printf("No of iterations Selected : %d\n",ITERATIONS);

            break;
		case 'o':
            strcpy(dest_txt_selected, optarg);
            printf("Destination file name : %s\n",dest_txt_selected);
            break;
        default :
            printf("Incorrect Command Line Selection.\n! Try Again !\n");
            exit(1); //exit code
			break;
        }
    }
	
	/* Thread creation */
	pthread_t threads[THREAD_CNT];
	
	/* ****************************************** */
	/*      Run Algo using selected lock          */
	/* ****************************************** */
	
	/* Run using Pthread */
    if (((strcmp(lock_selected,"pthread"))) == 0)
	{
		/* Lock Initialisation */
		pthread_mutex_init(&lk,NULL);	
        /* thread creation */
        for(long i=0; i< THREAD_CNT;i++)
        {
            int pid = pthread_create(&threads[i],NULL,lock_thread_counter,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/* wait for threads to finish the jobs */
        for(long i=0; i< THREAD_CNT;i++)
        {
            pthread_join(threads[i],NULL);
        }
		/* Print count */
		printf("Final lock Count :: %d :: \n",lock_cntr);
		pthread_mutex_destroy(&lk);

	}
	/* Run using Test and Set TAS */
    if (((strcmp(lock_selected,"tas"))) == 0)
	{
		/* Lock Initialisation */
        /* thread creation */
        for(long i=0; i< THREAD_CNT;i++)
        {
            int pid = pthread_create(&threads[i],NULL,lock_tas_counter,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/* wait for threads to finish the jobs */
        for(long i=0; i< THREAD_CNT;i++)
        {
            pthread_join(threads[i],NULL);
        }
		/* Print count */
		printf("Final lock Count :: %d :: \n",lock_cntr);

	}
	/* Run using Test and Test and Set lock TTAS */
    if (((strcmp(lock_selected,"ttas"))) == 0)
	{
		/* Lock Initialisation */
        /* thread creation */
        for(long i=0; i< THREAD_CNT;i++)
        {
            int pid = pthread_create(&threads[i],NULL,lock_ttas_counter,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/* wait for threads to finish the jobs */
        for(long i=0; i< THREAD_CNT;i++)
        {
            pthread_join(threads[i],NULL);
        }
		/* Print count */
		printf("Final lock Count :: %d :: \n",lock_cntr);
	}
	/* Run using Ticket lock  */
    if (((strcmp(lock_selected,"ticket"))) == 0)
	{
        /* thread creation */
        for(long i=0; i< THREAD_CNT;i++)
        {
            int pid = pthread_create(&threads[i],NULL,lock_ticket_counter,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/* wait for threads to finish the jobs */
        for(long i=0; i< THREAD_CNT;i++)
        {
            pthread_join(threads[i],NULL);
        }
		/* Print count */
		printf("Final lock Count :: %d :: \n",lock_cntr);
	}
	/* Run using MCS lock  */
    if (((strcmp(lock_selected,"mcs"))) == 0)
	{
        /* thread creation */
        for(long i=0; i< THREAD_CNT;i++)
        {
            int pid = pthread_create(&threads[i],NULL,lock_ttas_counter,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/* wait for threads to finish the jobs */
        for(long i=0; i< THREAD_CNT;i++)
        {
            pthread_join(threads[i],NULL);
        }
		/* Print count */
		printf("Final lock Count :: %d :: \n",lock_cntr);
	}
	/* Run using Peterson Seq CST  lock  */
    if (((strcmp(lock_selected,"peterseq"))) == 0)
	{
        /* thread creation */
		peterseq_lock_init();
        for(long i=0; i< THREAD_CNT;i++)
        {
            int pid = pthread_create(&threads[i],NULL,lock_peterseq_counter,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/* wait for threads to finish the jobs */
        for(long i=0; i< THREAD_CNT;i++)
        {
            pthread_join(threads[i],NULL);
        }
		/* Print count */
		printf("Final lock Count :: %d :: \n",lock_cntr);
	}
	/* Run using Peterson Seq CST  lock  */
    if (((strcmp(lock_selected,"peterrel"))) == 0)
	{
        /* thread creation */
		peterrel_lock_init();
        for(long i=0; i< THREAD_CNT;i++)
        {
            int pid = pthread_create(&threads[i],NULL,lock_peterrel_counter,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/* wait for threads to finish the jobs */
        for(long i=0; i< THREAD_CNT;i++)
        {
            pthread_join(threads[i],NULL);
        }
		/* Print count */
		printf("Final lock Count :: %d :: \n",lock_cntr);
	}

	/* ****************************************** */
	/*     Run Algo using selected  barrier       */
	/* ****************************************** */
	/* Using pthread barrier */
    if (((strcmp(bar_selected,"pthread"))) == 0)
	{
		/* Lock Initialisation */
		pthread_barrier_init(&bar, NULL, THREAD_CNT);
        /* thread creation */
        for(long i=0; i<THREAD_CNT;i++)
        {
            int pid = pthread_create(&threads[i],NULL,bar_thread_counter,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/* wait for threads to finish the jobs */
        for(long i=0; i< THREAD_CNT;i++)
        {
            pthread_join(threads[i],NULL);
        }
		/* Print count */
		printf("Final bar Count :: %d :: \n",bar_cntr);
		pthread_barrier_destroy(&bar);
	}
	/* Using  barrier sense */
    if (((strcmp(bar_selected,"sense"))) == 0)
	{
        /* thread creation */
        for(long i=0; i< THREAD_CNT;i++)
        {
            int pid = pthread_create(&threads[i],NULL,bar_sense_counter,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/* wait for threads to finish the jobs */
        for(long i=0; i< THREAD_CNT;i++)
        {
            pthread_join(threads[i],NULL);
        }
		/* Print count */
		printf("Final bar Count :: %d :: \n",bar_cntr);
	}
	
	/* File IO : Opening the source file from command line argument */
	dest_file = fopen(dest_txt_selected,"w+");
	if (lock_cntr > 0){
	fprintf(dest_file,"%d\n",lock_cntr);}
	if (bar_cntr > 0){
	fprintf(dest_file,"%d\n",bar_cntr);}
	
	fclose(dest_file);
	
	/* Clear Memory */
	free(desires_seq);
	free(desires_rel);
	free(bar_selected);
	free(lock_selected);
	free(dest_txt_selected);
	
	/*  Clock Time */
	unsigned long long elapsed_ns;
	elapsed_ns = (end.tv_sec-start.tv_sec)*1000000000 + (end.tv_nsec-start.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);
return 0;
}
/* ****************************************** */
/* ****************************************** */
/*          Function Definations              */
/* ****************************************** */
/* ****************************************** */

void* bar_thread_counter(void* arg)
{
	int tid = (long)arg;  
	printf("Thread[%d] in action \n", tid);
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&start);}
	//tid--; // adjust to zero based tid's
	for(int i = 0; i<ITERATIONS*THREAD_CNT; i++){
		if(i%THREAD_CNT==tid){
		bar_cntr++;
		}
		pthread_barrier_wait(&bar);
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&end);}
	}	
}

/* ************* LOCK Pthread Functions ****************** */
void* lock_thread_counter(void* arg)
{
	int tid = (long)arg; 
	printf("Thread[%d] in action \n", tid);
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&start);}
	
	for(int i = 0; i<ITERATIONS; i++)
	{
		pthread_mutex_lock(&lk);		
		lock_cntr++;
		pthread_mutex_unlock(&lk);
	}
	
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&end);}
	printf("Thread[%d] incremented till %d \n", tid,lock_cntr);
}

/***********************************************/
/*                TAS Lock                     */
/***********************************************/
void* lock_tas_counter(void* arg)
{
	int tid = (long)arg; 
	printf("Thread[%d] in action \n", tid);
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&start);}

	for(int i = 1; i<=ITERATIONS; i++)
	{
		//sleep(1);
		tas_lock();
		lock_cntr++;
		tas_unlock();
	}
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&end);}
}

void tas_lock(void)
{
	while( (atomic_flag_test_and_set_explicit(&tas_lock_flag,memory_order_seq_cst)) == 1){};
}
	
void tas_unlock(void)
{
	atomic_store_explicit(&tas_lock_flag,0,memory_order_seq_cst); // store zero in flag
}

/***********************************************/
/*            TTAS Lock Functions              */
/***********************************************/
void* lock_ttas_counter(void* arg)
{
	int tid = (long)arg; 
	printf("Thread[%d] in action \n", tid);
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&start);}
	for(int i = 1; i<=ITERATIONS; i++)
	{
		ttas_lock();
		lock_cntr++;
		ttas_unlock();
	}
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&end);}
}
void ttas_lock(void)
{
	while(((atomic_load_explicit(&ttas_lock_flag,memory_order_seq_cst)) == 1 ) || ( (atomic_flag_test_and_set_explicit(&ttas_lock_flag,memory_order_seq_cst)) == 1 )) {};
}
	
void ttas_unlock(void)
{
	atomic_store_explicit(&ttas_lock_flag,0,memory_order_seq_cst); // store zero in flag

}
/***********************************************/
/*              Ticket Lock                    */
/***********************************************/
void* lock_ticket_counter(void* arg)
{
	int tid = (long)arg; 
	printf("Thread[%d] in action \n", tid);
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&start);}
	for(int i = 1; i<=ITERATIONS; i++)
	{
		ticket_lock();
		lock_cntr++;
		ticket_unlock();
	}
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&end);}
}

void ticket_lock(void)
{
	int my_num = atomic_fetch_add_explicit(&ticket_next_num,1,memory_order_seq_cst);
	while( (atomic_load_explicit(&ticket_now_serving,memory_order_seq_cst)) != my_num ){};
}
void ticket_unlock(void)
{
	atomic_fetch_add_explicit(&ticket_now_serving,1,memory_order_seq_cst);
}

/***********************************************/
/*              Barrier Sense                  */
/***********************************************/
void* bar_sense_counter(void* arg)
{
	int tid = (long)arg;  
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&start);}
	//tid--; // adjust to zero based tid's
	for(int i = 0; i<ITERATIONS*THREAD_CNT; i++){
		if(i%THREAD_CNT==tid){
			bar_cntr++;
		}
		barrier_sense_wait();
	}	
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&end);}
}
void barrier_sense_wait(void)
{
	/* thread local variable */
	static thread_local int my_sense =0;
	if (my_sense == 0)
	{
		my_sense = 1;
	}else
	{
		my_sense = 0;
	}
	
	int cnt_copy = atomic_fetch_add_explicit(&bar_sense_cnt,1,memory_order_seq_cst);
	if (cnt_copy == THREAD_CNT-1)
	{
		atomic_store_explicit(&bar_sense_cnt,0,memory_order_release); // store zero in flag
		atomic_store_explicit(&bar_sense_sense,my_sense,memory_order_seq_cst); // store zero in flag
	}else
	{
		while(atomic_load_explicit(&bar_sense_sense,memory_order_seq_cst) != my_sense)
		{}
	}	
}

/***********************************************/
/*             MCS Lock Functions              */
/***********************************************/
void* lock_mcs_counter(void* arg)
{
	int tid = (long)arg; 
	printf("Thread[%d] in action \n", tid);
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&start);}
	for(int i = 0; i<ITERATIONS; i++)
	{
		mcs_lock(&lock,&node);
		lock_cntr++;
		mcs_unlock(&lock,&node);
	}
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&end);}
}


void mcs_lock(struct spinlock *lock, struct spinlock_node *my_node) {

	_Atomic struct spinlock_node *prev = atomic_load_explicit(&lock->tail, memory_order_seq_cst);
    atomic_store_explicit(&my_node->next, NULL, memory_order_relaxed);

	while( ! atomic_compare_exchange_weak_explicit(&lock->tail,&prev, my_node, memory_order_seq_cst,memory_order_seq_cst)){
		prev = atomic_load_explicit(&lock->tail,memory_order_seq_cst);
	}

	if (atomic_load_explicit(&prev,memory_order_seq_cst) != NULL)
	{
		atomic_store_explicit(&my_node->wait, true, memory_order_relaxed);
		atomic_store_explicit(&prev->next,my_node, memory_order_seq_cst);
		while(atomic_load_explicit(&my_node->wait,memory_order_seq_cst))  {
		} // wait
	}
}

void mcs_unlock(struct spinlock *lock, struct spinlock_node *my_node) {
	struct spinlock_node *m = atomic_load_explicit(&my_node,memory_order_seq_cst);
	if(atomic_compare_exchange_weak_explicit(&lock->tail, &m, NULL, memory_order_seq_cst, memory_order_seq_cst))
	{
	}
	else
	{
		while((   atomic_load_explicit(&my_node->next,memory_order_seq_cst) )== NULL) {}
		atomic_store_explicit(&my_node->wait,false,memory_order_seq_cst);
		atomic_store_explicit(&my_node->next,false,memory_order_seq_cst);
	}
}

/***********************************************/
/*          Peterson Sequenced                 */
/***********************************************/
void* lock_peterseq_counter(void* arg)
{
	int tid = (long)arg; 
	printf("Thread[%d] in action \n", tid);
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&start);}
	for(int i = 0; i<ITERATIONS; i++)
	{
		peterseq_lock(tid);
		lock_cntr++;
		peterseq_unlock(tid);
	}
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&end);}
}

void peterseq_lock_init(void) {
  atomic_store_explicit(&desires_seq[0],false,memory_order_seq_cst);
  atomic_store_explicit(&desires_seq[1],false,memory_order_seq_cst);
  atomic_store_explicit(&turn_seq,false,memory_order_seq_cst);
}

void peterseq_lock(int tid) {
	atomic_store_explicit(&desires_seq[tid],true,memory_order_seq_cst);
	atomic_store_explicit(&turn_seq,(!tid),memory_order_seq_cst);
	
	while(   ( atomic_load_explicit(&desires_seq[!tid],memory_order_seq_cst)  )&&(atomic_load_explicit(&turn_seq,memory_order_seq_cst) == (!tid))    ) {}
	
}
void peterseq_unlock(int tid) {
	atomic_store_explicit(&desires_seq[tid], 0, memory_order_seq_cst);
}

/***********************************************/
/*          Peterson Released                  */
/***********************************************/
void* lock_peterrel_counter(void* arg)
{
	int tid = (long)arg; 
	printf("Thread[%d] in action \n", tid);
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&start);}
	for(int i = 0; i<ITERATIONS; i++)
	{

		peterrel_lock(tid);
		lock_cntr++;
		peterrel_unlock(tid);
	}
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&end);}
}

void peterrel_lock_init(void) {
  atomic_store_explicit(&desires_rel[0],false,memory_order_release);
  atomic_store_explicit(&desires_rel[1],false,memory_order_release);
  atomic_store_explicit(&turn_rel,false,memory_order_release);
}

void peterrel_lock(int tid) {
	atomic_store_explicit(&desires_rel[tid],true,memory_order_acquire);
	atomic_store_explicit(&turn_rel,(!tid),memory_order_acquire);
	while(   ( atomic_load_explicit(&desires_rel[!tid],memory_order_acquire)  )&&(atomic_load_explicit(&turn_rel,memory_order_acquire) == (!tid))    ) {}
	
}
void peterrel_unlock(int tid) {
	atomic_store_explicit(&desires_rel[tid], 0, memory_order_acquire);
}