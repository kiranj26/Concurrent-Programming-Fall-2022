/***********************************************/
/***********************************************/
/*     Lab 1  : Concurrent Programming         */
/*     Author : Kiran Jojare                   */
/***********************************************/
/***********************************************/

/* Includes */
#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/time.h>
#include <time.h>
#include <limits.h>
#include<stdatomic.h>
#include "threads.h"
#include "assert.h"
#include <stdbool.h>

/* Preprocessing Data */
#define MAX 10
#define TRUE 1
#define FALSE 0

/* Structure definations for MCS lock */
struct spinlock_node {
   _Atomic struct spinlock_node *next;
   _Atomic _Bool wait;
}node;


struct spinlock {
   _Atomic struct spinlock_node * tail;
}lock;

/* Function Declaration */
void* thread_bucket_sort_tas(void* args);
void tas_lock(void);
void tas_unlock(void);

void* thread_bucket_sort_ttas(void* args);
void ttas_lock(void);
void ttas_unlock(void);

void* thread_bucket_sort_ticket(void* args);
void ticket_lock(void);
void ticket_unlock(void);

void* thread_bucket_sort_barsense(void* args);
void barrier_sense_wait(void);

void* lock_peterseq_counter(void* arg);
void peterseq_lock_init(void);
void peterseq_lock(int tid);
void peterseq_unlock(int tid);

void* lock_peterrel_counter(void* arg);
void peterrel_lock_init(void);
void peterrel_lock(int tid);
void peterrel_unlock(int tid);

void* lock_mcs_counter(void* arg);
void mcs_lock(struct spinlock *lock, struct spinlock_node *my_node);
void mcs_unlock(struct spinlock *lock, struct spinlock_node *my_node);
void MCS_lock(_Atomic struct spinlock *lock, _Atomic struct spinlock_node *node);
void MCS_unlock(_Atomic struct spinlock *lock, _Atomic struct spinlock_node *node);

void* thread_bucket_sort_pthread_bar(void* args);

void swap(int* xp, int* yp);
void selectionSort(int arr[], int n);
void* thread_bucket_sort_pthread(void* args);
void merge_sections_of_array(int arr[], int number, int aggregation);
void printArray(int * arr,int size);
int max_element(int * array, int size);
/* structure data for thread data for bucket sort */
struct tData {
	int head;
	int tail;
	int tid;
}td[MAX];



/* Global Data */
int no_per_thread;
int offset;
int thread_count;
int count;
int bucket_count;
int bucket_length;
int * arr;
int * mins;
int ** buckets;
int * counter;
int old_index;
int * bucket_length_counter;
int k=0;

/* Atomic variables */
_Atomic _Bool * tas_lock_flag      = 0;
_Atomic _Bool * ttas_lock_flag     = 0;
_Atomic int   * ticket_next_num    = 0;
_Atomic int   * ticket_now_serving = 0;
_Atomic int   * bar_sense_cnt      = 0;
_Atomic int   * bar_sense_sense    = 0;

_Atomic _Bool * desires_seq;
_Atomic int     turn_seq;
_Atomic _Bool * desires_rel;
_Atomic int     turn_rel;

/* Pthread Vriables */
pthread_barrier_t bar1;
pthread_mutex_t lk1;

/* Timing Structures */
struct timespec start, end;

/***********************************************/
/***********************************************/
/*                  Main                       */
/***********************************************/
/***********************************************/

int main(int argc, char *argv[])
{
	
	/* Character Initialisation */
	int c                   =  0;
	int option_index        =  0;
	int thread_cnt_recieved =  0;

	char * bar_selected;
	char * lock_selected;
	char * dest_txt_selected;
	char * source_txt_selected;
	//FILE * dest_file;
	
	bar_selected        =  (char*)malloc(10*sizeof(char));
	lock_selected       =  (char*)malloc(25*sizeof(char));
    dest_txt_selected   =  (char*)malloc(50*sizeof(char));
    source_txt_selected =  (char*)malloc(50*sizeof(char));
    desires_seq       =  (_Atomic _Bool *)malloc(2*sizeof(_Atomic _Bool *));
    desires_rel       =  (_Atomic _Bool *)malloc(2*sizeof(_Atomic _Bool *));

	/* Command Line Args Parsing */
	static struct option long_options[] ={
    {"name",0,0,'n'},              // long option 1
    {"thread",1,0,'t'},            // long option 2
	{"dest",1,0,'o'},              // long option 3
	{"bar",1,0,'b'},               // long option 4
	{"lock",1,0,'l'},              // long option 5
	{0,0,0,0},
    };
	
	/* Processing Command Line Args */
	while((c = getopt_long(argc,argv, "nt:b:l:o:", long_options,&option_index)) != -1)
    {
        switch (c)
        {
        case 'n':
            printf("Student name : Kiran Narendra Jojare\n");
            exit(1); //print name and exit code
            break;
       case 't':
            thread_count = atoi(optarg);
			printf("No of threads Selected : %d\n",thread_count);
            thread_cnt_recieved = 1;
            break;
        case 'b':
            strcpy(bar_selected, optarg);
            printf("Barrier Selected : %s\n",bar_selected);
            break;
		case 'l':
            strcpy(lock_selected, optarg);
            printf("Lock Selected : %s\n",lock_selected);
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
    /* Configure default thread counts */
    if (thread_cnt_recieved != 1 || thread_count > 15)
    {
        thread_count = 4;
    }
    printf("No of threads selected : %d\n",thread_count);
    printf("Thread count recieved: %d\n",thread_cnt_recieved);

	
	/* Take soure file name from command line argument */
    if (thread_cnt_recieved != 1)
    {
		printf("Source file name : %s\n",argv[4]); 
    }else
    {
		printf("Source file name : %s\n",argv[6]); 
    }
	
    /* Take Data From Source File */
    arr = (int *)malloc(7000001*sizeof(int *));
	
	for (int i=0;i<8;i++)
	{
		printf("Args[%d] = %s\n",i,argv[i]);
	}

    FILE* source_file;
    FILE* destination_file;
	
    /* File IO : Opening the source file from command line argument */
    int i=0;
    if (thread_cnt_recieved != 1)
    {
        source_file = fopen(argv[4],"r");
    }else
    {
        source_file = fopen(argv[6],"r");
    }
    
    if (source_file == NULL)
    {
        printf("Error Opening File \n");
    }
    if (source_file)
    {
        while(fscanf(source_file,"%d",&arr[i]) != EOF)
        {
            i++;
        }
    }
    fclose(source_file);
    arr[i] = '\0';
	
    /* To calculate number of elements to be sorted */
    for(int i=0;arr[i] != '\0';i++)
    {
        count++;  
    }
    int n;
    n = count; //count indicates total number of unsorted elements


    /* Thread Array Creation*/
    pthread_t threads[thread_count];
    no_per_thread = count/thread_count;
    offset = count%thread_count;
	
	/* Calculate max value from the given unsorted data */
	int max = max_element(arr,count);
	bucket_count = count/thread_count;
	bucket_length = count;
	/* If Count is large specify fixed size */
	if(count > 30000)
	{
	bucket_length = 100;
	}
	bucket_length_counter = (int *)calloc(bucket_count, sizeof(int));

	/* Initialise buckets */
	buckets = (int**)calloc(bucket_count,sizeof(int*));
	for (int i=0;i<bucket_count;i++)
	{
		buckets[i] = (int*)calloc(1,sizeof(int));
	}
		
	/* Initialised Mins[] */
	mins = (int *)malloc((bucket_count+1)*sizeof(int *)); // int mins[thread_count+1]
	/* Initialised Counter */
	counter = (int*)malloc(bucket_count * sizeof(int));  
	for ( int i=0;i<bucket_count;i++)
	{
		counter[i] = 0;
	}

	/* Fill min data based on input */
	for(int i=0; i<=bucket_count; i++){
		if(i==bucket_count){
			mins[i] = max+1;}// +1 coz in distribution it will leave the last valu
		else
		{mins[i] = i * (max/bucket_count);}
	}

	/* Initialise thread data as a array of structure*/
    int fragsSize = count / thread_count;
	int remElements = count - (fragsSize * thread_count);
   
	int temp=0;
	for(size_t i=0; i<thread_count;i++){
		td[i].head = temp;
		td[i].tail = td[i].head + fragsSize -1;
   			if(remElements){
			td[i].tail++;
			remElements--;
		}
		temp = td[i].tail+1;
		td[i].tid = i+1;
	}
	/* ****************************************** */
	/*      Run Algo using selected lock          */
	/* ****************************************** */
	
	/* Run using Pthread */
    if (((strcmp(lock_selected,"pthread"))) == 0)
	{
		/* Lock Initialisation */
		pthread_mutex_init(&lk1,NULL);	
        /* thread creation */
		for(long i=0; i<thread_count;i++)
        {
            int pid = pthread_create(&threads[i],NULL,thread_bucket_sort_pthread,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/*  Waiting for thread operation */
		for(long i=0; i< thread_count;i++)
        {
            pthread_join(threads[i],NULL);
        }
			
		pthread_mutex_destroy(&lk1);
	}
	/* Run using TAS */
    if (((strcmp(lock_selected,"tas"))) == 0)
	{
        /* thread creation */
		for(long i=0; i<thread_count;i++)
        {
            int pid = pthread_create(&threads[i],NULL,thread_bucket_sort_tas,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/*  Waiting for thread operation */
		for(long i=0; i< thread_count;i++)
        {
            pthread_join(threads[i],NULL);
        }			
	}
	/* Run using TTAS */
    if (((strcmp(lock_selected,"ttas"))) == 0)
	{
        /* thread creation */
		for(long i=0; i<thread_count;i++)
        {
            int pid = pthread_create(&threads[i],NULL,thread_bucket_sort_ttas,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/*  Waiting for thread operation */
		for(long i=0; i< thread_count;i++)
        {
            pthread_join(threads[i],NULL);
        }			
	}
	/* Run using Ticket */
    if (((strcmp(lock_selected,"ticket"))) == 0)
	{
        /* thread creation */
		for(long i=0; i<thread_count;i++)
        {
            int pid = pthread_create(&threads[i],NULL,thread_bucket_sort_ticket,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/*  Waiting for thread operation */
		for(long i=0; i< thread_count;i++)
        {
            pthread_join(threads[i],NULL);
        }			
	}
	/* Run using MCS */
    if (((strcmp(lock_selected,"mcs"))) == 0)
	{
        /* thread creation */
		for(long i=0; i<thread_count;i++)
        {
            int pid = pthread_create(&threads[i],NULL,thread_bucket_sort_ticket,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/*  Waiting for thread operation */
		for(long i=0; i< thread_count;i++)
        {
            pthread_join(threads[i],NULL);
        }			
	}
	/* Run using Peterson Seq CST  lock  */
    if (((strcmp(lock_selected,"peterseq"))) == 0)
	{
        /* thread creation */
		peterseq_lock_init();
        for(long i=0; i< thread_count;i++)
        {
            int pid = pthread_create(&threads[i],NULL,lock_peterseq_counter,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/* wait for threads to finish the jobs */
        for(long i=0; i< thread_count;i++)
        {
            pthread_join(threads[i],NULL);
        }
	}
	/* Run using Peterson Seq CST  lock  */
    if (((strcmp(lock_selected,"peterrel"))) == 0)
	{
        /* thread creation */
		peterrel_lock_init();
        for(long i=0; i< thread_count;i++)
        {
            int pid = pthread_create(&threads[i],NULL,lock_peterrel_counter,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/* wait for threads to finish the jobs */
        for(long i=0; i< thread_count;i++)
        {
            pthread_join(threads[i],NULL);
        }
	}
	/* ****************************************** */
	/*      Run Algo using selected barrier       */
	/* ****************************************** */
	
	/* Run using Pthread */
    if (((strcmp(bar_selected,"sense"))) == 0)
	{
        /* thread creation */		
		pthread_mutex_init(&lk1,NULL);	
		for(long i=0; i<thread_count;i++)
        {
            int pid = pthread_create(&threads[i],NULL,thread_bucket_sort_barsense,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/*  Waiting for thread operation */
		for(long i=0; i< thread_count;i++)
        {
            pthread_join(threads[i],NULL);
        }
		pthread_mutex_destroy(&lk1);
	}
	if (((strcmp(bar_selected,"pthread"))) == 0)
	{
        /* thread creation */		
		pthread_barrier_init(&bar1, NULL, thread_count);
		for(long i=0; i<thread_count;i++)
        {
            int pid = pthread_create(&threads[i],NULL,thread_bucket_sort_pthread_bar,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/*  Waiting for thread operation */
		for(long i=0; i< thread_count;i++)
        {
            pthread_join(threads[i],NULL);
        }
		pthread_mutex_destroy(&lk1);
	}

	/* Sorting BUckets Internally */
	for(int i=0;i<bucket_count;i++)
	{
			selectionSort(buckets[i],bucket_length_counter[i]);
	}
	
	/*  Writting buckets into array arr */
	int k=0;
	destination_file = fopen(dest_txt_selected,"w+");
	printf(":::::::::::::::Old index = %d\n",old_index);	
	/*  Sorted Buckets Aritten into arr */
	for ( int i=0;i<bucket_count;i++)
	{
			for (int j=0; j<bucket_length_counter[i]; j++){
			if ((buckets[i][j]) != 0)
			{
				fprintf(destination_file,"%d\n",buckets[i][j]);
				arr[k++] = buckets[i][j];
			}
		}
	}
	
	/* Clear Memory */
	//for (int i=0;i<bucket_length;i++)
	//{
	//	free(buckets[i]);
	//}
	//free(buckets);
	free(desires_seq);
	free(desires_rel);
	free(bar_selected);
	free(lock_selected);
	free(dest_txt_selected);
	free(source_txt_selected);
	free(counter);
	free(mins);
	free(bucket_length_counter);
	//fclose(destination_file);
	
	/*  Clock Time */
	unsigned long long elapsed_ns;
	elapsed_ns = (end.tv_sec-start.tv_sec)*1000000000 + (end.tv_nsec-start.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);

}
/* ****************************************** */
/* ****************************************** */
/*            Function Defination             */
/* ****************************************** */
/* ****************************************** */

/* Thread Main For Pthread */
void* thread_bucket_sort_pthread(void* args)
{
	int thread_id = (int)args;
	printf("Thread %u reporting for duty\n",thread_id);
	// loading thread data into local variable
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;

	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	// lock aquired filling data into 
	// buckets to be sorted
	pthread_mutex_lock(&lk1);
	for( int i=head;i<=tail;i++)
	{
		for (int j=0;j<=bucket_count;j++)
		{
			if (arr[i] <= mins[j])
			{  	
				// if given data is within bucket 
				// mins thn fill bucket
				int index = counter[j-1];
				counter[j-1] = counter[j-1] +1;
				buckets[j-1] = realloc(buckets[j-1],(index+1)*sizeof(int));
				buckets[j-1][index] = arr[i];
				bucket_length_counter[j-1] = index+1;
				old_index = index;
				break;
			}
		}	
	}
	pthread_mutex_unlock(&lk1);
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
	
}
/* ****************************************** */
/*                TAS Functions               */
/* ****************************************** */

/* Thread Main For tas */
void* thread_bucket_sort_tas(void* args)
{
	int thread_id = (int)args;
	printf("Thread %u reporting for duty\n",thread_id);
	
	// loading thread data into local variable
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;

	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	// lock aquired filling data into 
	// buckets to be sorted
	tas_lock();
	for( int i=head;i<=tail;i++)
	{
		for (int j=0;j<=bucket_count;j++)
		{
			if (arr[i] <= mins[j])
			{  	
				// if given data is within bucket 
				// mins thn fill bucket
				int index = counter[j-1];
				counter[j-1] = counter[j-1] +1;
				buckets[j-1] = realloc(buckets[j-1],(index+1)*sizeof(int));
				buckets[j-1][index] = arr[i];
				bucket_length_counter[j-1] = index+1;
				old_index = index;
				break;
			}
		}	
	}
	tas_unlock();
	// lock released
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
	
}
/* TAS Lock */
void tas_lock(void)
{
	while( (atomic_flag_test_and_set_explicit(&tas_lock_flag,memory_order_seq_cst)) == 1){};
}
/* TAS unlock */
void tas_unlock(void)
{
	atomic_store_explicit(&tas_lock_flag,0,memory_order_seq_cst); // store zero in flag
}


/* ****************************************** */
/*               TTAS Functions               */
/* ****************************************** */

/* Thread Main For ttas */
void* thread_bucket_sort_ttas(void* args)
{
	int thread_id = (int)args;
	printf("Thread %u reporting for duty\n",thread_id);
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;

	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	// lock aquired filling data into 
	// buckets to be sorted
	ttas_lock();
	for( int i=head;i<=tail;i++)
	{
		for (int j=0;j<=bucket_count;j++)
		{
			if (arr[i] <= mins[j])
			{  	
				// if given data is within bucket 
				// mins thn fill bucket
				int index = counter[j-1];
				counter[j-1] = counter[j-1] +1;
				buckets[j-1] = realloc(buckets[j-1],(index+1)*sizeof(int));
				buckets[j-1][index] = arr[i];
				bucket_length_counter[j-1] = index+1;
				old_index = index;
				break;
			}
		}	
	}
	ttas_unlock();
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
	
}
/* TTAS Lock */
void ttas_lock(void)
{
	while(((atomic_load_explicit(&ttas_lock_flag,memory_order_seq_cst)) == 1 ) || ( (atomic_flag_test_and_set_explicit(&ttas_lock_flag,memory_order_seq_cst)) == 1 )) {};
}
/* TTAS Unlock */
void ttas_unlock(void)
{
	atomic_store_explicit(&ttas_lock_flag,0,memory_order_seq_cst); // store zero in flag

}

/* ****************************************** */
/*             Ticket Functions               */
/* ****************************************** */
/* Thread Main For ticket */
void* thread_bucket_sort_ticket(void* args)
{
	int thread_id = (int)args;
	printf("Thread %u reporting for duty\n",thread_id);
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;

	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	// lock aquired filling data into 
	// buckets to be sorted
	ticket_lock();
	for( int i=head;i<=tail;i++)
	{
		for (int j=0;j<=bucket_count;j++)
		{
			if (arr[i] <= mins[j])
			{  	
				// if given data is within bucket 
				// mins thn fill bucket
				int index = counter[j-1];
				counter[j-1] = counter[j-1] +1;
				buckets[j-1] = realloc(buckets[j-1],(index+1)*sizeof(int));
				buckets[j-1][index] = arr[i];
				bucket_length_counter[j-1] = index+1;
				old_index = index;
				break;
			}
		}	
	}
	ticket_unlock();
	// lock released
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
}
/* Ticket Lock */
void ticket_lock(void)
{
	int my_num = atomic_fetch_add_explicit(&ticket_next_num,1,memory_order_seq_cst);
	while( (atomic_load_explicit(&ticket_now_serving,memory_order_seq_cst)) != my_num ){};
}
/* ticket Unlock */
void ticket_unlock(void)
{
	atomic_fetch_add_explicit(&ticket_now_serving,1,memory_order_seq_cst);
}

/* ****************************************** */
/*          Barrier Sense Functions           */
/* ****************************************** */
/* Thread Main For Sense Barrier */
void* thread_bucket_sort_barsense(void* args)
{
	int thread_id = (int)args;
	// loading thread data into local variable
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	// lock aquired filling data into 
	// buckets to be sorted
	// printf("Head = %d \t Tail = %d\n",head,tail);
	// printf("Thread %u got the lock!\n",thread_id);
	barrier_sense_wait();
	
	for( int i=head;i<=tail;i++)
	{
		for (int j=0;j<=bucket_count;j++)
		{
			if (arr[i] <= mins[j])
			{  	
				// if given data is within bucket 
				// mins thn fill bucket
				pthread_mutex_lock(&lk1);
				int index = counter[j-1];
				counter[j-1] = counter[j-1] +1;
				buckets[j-1] = realloc(buckets[j-1],(index+1)*sizeof(int));
				buckets[j-1][index] = arr[i];
				bucket_length_counter[j-1] = index+1;
				old_index = index;
				pthread_mutex_unlock(&lk1);
				break;
			}
		}	
	}
	barrier_sense_wait();
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
	
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
	if (cnt_copy == thread_count-1)
	{
		atomic_store_explicit(&bar_sense_cnt,0,memory_order_release); // store zero in flag
		atomic_store_explicit(&bar_sense_sense,my_sense,memory_order_seq_cst); // store zero in flag
	}else
	{
		while(atomic_load_explicit(&bar_sense_sense,memory_order_seq_cst) != my_sense)
		{}
	}	
}

/* ****************************************** */
/*        Barrier Pthread  Functions          */
/* ****************************************** */
/* Function for barrier based on pthread */
void* thread_bucket_sort_pthread_bar(void* args)
{
	int thread_id = (int)args;
	// loading thread data into local variable
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	// lock aquired filling data into 
	// buckets to be sorted
	pthread_barrier_wait(&bar1);
	for( int i=head;i<=tail;i++)
	{
		for (int j=0;j<=bucket_count;j++)
		{
			if (arr[i] <= mins[j])
			{  	
				// if given data is within bucket 
				// mins thn fill bucket
				int index = counter[j-1];
				counter[j-1] = counter[j-1] +1;
				buckets[j-1] = realloc(buckets[j-1],(index+1)*sizeof(int));
				buckets[j-1][index] = arr[i];
				bucket_length_counter[j-1] = index+1;
				old_index = index;
				break;
			}
		}	
	}
	pthread_barrier_wait(&bar1);
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
	
}

/* ****************************************** */
/*           Peterson Seq  Functions          */
/* ****************************************** */
void* lock_peterseq_counter(void* arg)
{
	int thread_id = (int)arg;
	printf("Thread %u reporting for duty\n",thread_id);
	// loading thread data into local variable
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;

	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	// lock aquired filling data into 
	// buckets to be sorted
	peterseq_lock(thread_id);
	for( int i=head;i<=tail;i++)
	{
		for (int j=0;j<=bucket_count;j++)
		{
			if (arr[i] <= mins[j])
			{  	
				// if given data is within bucket 
				// mins thn fill bucket
				int index = counter[j-1];
				counter[j-1] = counter[j-1] +1;
				buckets[j-1] = realloc(buckets[j-1],(index+1)*sizeof(int));
				buckets[j-1][index] = arr[i];
				bucket_length_counter[j-1] = index+1;
				old_index = index;
				break;
			}
		}	
	}
	peterseq_unlock(thread_id);
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
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

/* ****************************************** */
/*           Peterson Rel  Functions          */
/* ****************************************** */
void* lock_peterrel_counter(void* arg)
{
	int thread_id = (int)arg;
	printf("Thread %u reporting for duty\n",thread_id);
	
	// loading thread data into local variable
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;

	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	// lock aquired filling data into 
	// buckets to be sorted
	// printf("Head = %d \t Tail = %d\n",head,tail);
	// printf("Thread %u got the lock!\n",thread_id);
	peterrel_lock(thread_id);
	for( int i=head;i<=tail;i++)
	{
		for (int j=0;j<=bucket_count;j++)
		{
			if (arr[i] <= mins[j])
			{  	
				// if given data is within bucket 
				// mins thn fill bucket
				int index = counter[j-1];
				counter[j-1] = counter[j-1] +1;
				buckets[j-1] = realloc(buckets[j-1],(index+1)*sizeof(int));
				buckets[j-1][index] = arr[i];
				bucket_length_counter[j-1] = index+1;
				old_index = index;
				break;
			}
		}	
	}
	peterrel_unlock(thread_id);
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
}

void peterrel_lock_init(void) {
  atomic_store_explicit(&desires_seq[0],false,memory_order_relaxed);
  atomic_store_explicit(&desires_seq[1],false,memory_order_relaxed);
  atomic_store_explicit(&turn_seq,false,memory_order_relaxed);
}

void peterrel_lock(int tid) {
	atomic_store_explicit(&desires_seq[tid],true,memory_order_relaxed);
	atomic_store_explicit(&turn_seq,(!tid),memory_order_relaxed);
	
	while(   ( atomic_load_explicit(&desires_seq[!tid],memory_order_relaxed)  )&&(atomic_load_explicit(&turn_seq,memory_order_relaxed) == (!tid))    ) {}
	
}
void peterrel_unlock(int tid) {
	atomic_store_explicit(&desires_seq[tid], 0, memory_order_relaxed);
}

/* ****************************************** */
/*              MCS Lock  Functions           */
/* ****************************************** */
void* lock_mcs_counter(void* arg)
{
	int thread_id = (int)arg;
	printf("Thread %u reporting for duty\n",thread_id);
	
	// loading thread data into local variable
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;

	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	// lock aquired filling data into 
	// buckets to be sorted
	// printf("Head = %d \t Tail = %d\n",head,tail);
	// printf("Thread %u got the lock!\n",thread_id);
	mcs_lock(&lock,&node);
	for( int i=head;i<=tail;i++)
	{
		for (int j=0;j<=bucket_count;j++)
		{
			if (arr[i] <= mins[j])
			{  	
				// if given data is within bucket 
				// mins thn fill bucket
				int index = counter[j-1];
				counter[j-1] = counter[j-1] +1;
				buckets[j-1] = realloc(buckets[j-1],(index+1)*sizeof(int));
				buckets[j-1][index] = arr[i];
				bucket_length_counter[j-1] = index+1;
				old_index = index;
				break;
			}
		}	
	}
	mcs_lock(&lock,&node);
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
}

void mcs_lock(struct spinlock *lock, struct spinlock_node *my_node) {
	//my_node->next = NULL;
	//my_node->wait = 0;
	_Atomic struct spinlock_node *prev = atomic_load_explicit(&lock->tail, memory_order_seq_cst);
    atomic_store_explicit(&my_node->next, NULL, memory_order_relaxed);
	printf("Inside Lock :: Before while\n");

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


/***************************************************/
/*        Function for Selection Sort             */
/***************************************************/
void selectionSort(int arr[], int n)
{
    int i, j, min_idx;
    // One by one move boundary of unsorted subarray
    for (i = 0; i < n - 1; i++) {
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;
        // Swap the found minimum element
        // with the first element
        swap(&arr[min_idx], &arr[i]);
    }
}
void swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
/***********************************************/
/*           Max Element Function              */
/***********************************************/

int max_element(int * array, int size) 
{  
    // Initializing max variable 
	// to minimum value so that it can be updated
    // when we encounter any element 
	// which is greater than it.
    int max = INT_MIN;  
    for (int i = 0; i < size; i++)
    {
        //Updating max when array[i] is greater than max
        if (array[i] > max)  
        max = array[i];  
    }
    //return the max element
    return max;  
}
/***********************************************/
/*                 Print Array                 */
/***********************************************/
void printArray(int * arr,int size)
{
	for(int i=0;i<size;i++)
	{
		printf("%d ",arr[i]);
	}
	printf("\n");
}

/***************************************************/
/*                     END                         */
/***************************************************/