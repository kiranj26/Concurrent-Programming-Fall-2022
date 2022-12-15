# Lab 2 : Building Concurrency Premetives
Student Name : Kiran Narendra Jojare 
Section      : Graduate Student in ECEN

## Summary:
The outcome of Lab 2 after creating four locks and one barrier algorithm. TAS, TTAS, Ticket and MCS are the four locks that were created. A sense barrier has been created. The `perf` tool is used to evaluate the performance of each concurrency premetive. The complete analysis, code behavior, and code structure are all explained in the following sections. The lab 2 works on creating locks and barriers for counter `counter.c` and also for bucket sort algorithm in `mysort.c`. 

Note : For additional credits Peterson's lock for two threads under both sequential and release consistency has been implemented.

## Code Description

### Counter
Counter function uses different concurrency premetives to increment a counter variable for certain number of iterations. The counter will use command line arguments in below format. 

`counter [--name] [-t NUM_THREADS] [-i NUM_ITERATIONS] [--bar=<sense,pthread>] [--lock=<tas,ttas,ticket,mcs,pthread,petersonseq,petersonrel>] [-o out.txt]`

The program launches `NUM_THREADS` and each increments the counter `NUM_ITERATIONS` times.  The counter is synchronized using either the `bar` or `lock` argument (setting both is an invalid input). Counter.c contains the following functions. Note each concurrency premetive have seperate thread functions as follows.

Thread Functions for Barriers : 
* void* bar_thread_counter(void* arg);
* void* bar_sense_counter(void* arg);

Typical Thread function using barrier is as follows : 

```
void* barrier thread function(void* arg)
{
	int tid = (long)arg;  
	//tid--; // adjust to zero based tid's
	for(int i = 0; i<ITERATIONS*THREAD_CNT; i++){
		if(i%THREAD_CNT==tid){
		bar_cntr++;
		}
		pthread barrier wait() / barrier sense wait()
	if(tid==1){clock_gettime(CLOCK_MONOTONIC,&end);}
	}	
}
```
The code uses the C file "counter.c" and the "Makefile" for compilation. 
counter.c contains the following thread functions:

Thread Functions for Locking : 

* void* lock_thread_counter(void* arg);
* void* lock_tas_counter(void* arg);
* void* lock_ttas_counter(void* arg);
* void* lock_ticket_counter(void* arg);
* void* lock_mcs_counter(void* arg);
* void* lock_peterseq_counter(void* arg);
* void* lock_peterrel_counter(void* arg);

Typical Thread function using locks is as follows : 

```
void* lock thread function(void* arg)
{
	int tid = (long)arg; 
	for(int i = 1; i<=ITERATIONS; i++)
	{
		tas/ttas/ticket/mcs/peterseq/peterrel lock ()
		lock_cntr++;
		tas/ttas/ticket/mcs/peterseq/peterrel unlock ()
	}
}
```
Counter code will save the value of final count incremented into text file mentioned in command line argument. 


### My Sort

My sort code is responsible to sorting an unsorted data from source text file and storing sorted data back into another destination file. Sorting method used is a bucket sort. My sort will use bucket sort algorithm using custom builded concurrency premetives. The command line format for my sort function is as follows. 

`mysort [--name] [source.txt] [-o out.txt] [-t NUM_THREADS] [--bar=<sense,pthread>] [--lock=<tas,ttas,ticket,mcs,pthread,petersonseq,petersonrel>]`

The code flow is as follows :

* Fetch the integers from surce text file obtained from command line argument using File IO operation
* Based on the command line argument value of variable --lock or --bar , sort the data using either lock selected or barrier selected.
* In the case of lock , the respective thread functions will sort an unsorted list of integers into a sorted array using either tas,ttas,ticket,mcs,pthread,petersonseq,petersonrel locks.
* In the case of barriers , the respective thread functions will sort an unsorted list of integers into a sorted array using either sense,pthread barriers.
* The sorted list of integers will be stored in the destination file using File IO operations. The name of the destination file and format are taken from command line arguments.
  
The code uses the C file "mysort1.c" and the "Makefile" for compilation. 
mysort.c contains the following thread functions:


* void* thread_bucket_sort_pthread(void* args);
* void* thread_bucket_sort_tas(void* args);
* void* thread_bucket_sort_ttas(void* args);
* void* thread_bucket_sort_ticket(void* args);
* void* thread_bucket_sort_barsense(void* args);
* void* lock_peterseq_counter(void* arg);
* void* lock_peterrel_counter(void* arg);

Some additional functions for swaping data, doing selection sort on final sorted array, for merging arrays, for finding max value from array, for printing array is as follows.

* void swap(int* xp, int* yp);
* void selectionSort(int arr[], int n);
* void merge_sections_of_array(int arr[], int number, int aggregation);
* void printArray(int * arr,int size);
* int max_element(int * array, int size);

Typical thread function structure for bucket sort using barriers is as follows

```
void* thread function for barrier (void* args)
{
	int thread_id = (int)args;
	// loading thread data into local variable
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;
	// lock aquired filling data into 
	// buckets to be sorted
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
	barrier pthread/barrier sense()
	
}

```

Typical thread function structure for bucket sort using locks is as follows

```
void* thread function for lock (void* args)
{
	int thread_id = (int)args;	
	// loading thread data into local variable
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;
	// lock aquired filling data into 
	// buckets to be sorted
	tas/ttas/ticket/mcs/peterseq/peterrel _lock();
	// lock aquired
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
	tas/ttas/ticket/mcs/peterseq/peterrel _unlock();
	// lock released
}

```

## Code Performance

Code performance for both the codes `counter.c` and `mysort.c` is performed using `perf`.

Perf is a profiler tool for Linux 2.6+ based systems that abstracts away CPU hardware differences in Linux performance measurements and presents a simple command line interface. It covers hardware level (CPU/PMU, Performance Monitoring Unit) features and software features (software counters, tracepoints) as well.

### Perf Performance for Counter

* Performance analysis of the `counter.c` code with TAS, TTAS, Ticket and MCS lock is given below in the table.

| Primitive    | Runtime (s) | L1 cache hit (%) | Branch Pred Hit Rate (%) | Page Fault Count (#)|
|--------------|-------------|------------------|--------------------------|---------------------|
| TAS lock              |0.0051942  Sec | 8.21% | 3.59% | 78 |
| TTAS lock             |0.000409 Sec  | 3.76% | 0.69% | 76 |
| Ticket lock             |0.009395 Sec  | 0.41% | 0.33% | 78 |
| MCS lock           | 0.051423 Sec  | 0.18% | 0.71% | 76 |
| Pthread barrier           |1.106723 Sec  | 7.50% | 2.35% | 77 |
| Barrier Sense            |0.043916 Sec  | 0.40% | 1.24%  | 84 |

* Performance analysis of the my sort code with two threads using Peterson's lock is given below in the table.

| Primitive    | Runtime (s) | L1 cache hit (%) | Branch Pred Hit Rate (%) | Page Fault Count (#)|
|--------------|-------------|------------------|--------------------------|---------------------|
| Peterson's Release Consistancy lock              | 0.007148 Sec  | 4.45% | 2.37% | 77 |
| Peterson's Sequencial Consistancy lock           | 0.010423 Sec  | 1.52% | 2.34% | 76 |

According to the above table, the performance of counter.c with MCS lock is excellent, with an L1 cache hit percentage of only 0.18%. The MCS lock performed the best for counetr.c, followed by the ticket lock and Peterson's Sequencial Consistency lock. Barrier sensing outperformed conventional pthread barriers in terms of performance. Surprisingly 

TAS lock, on the other hand, was the worst performing concureency premetive in terms of counter.c with significantly high L1 cache hit percentage of around 8.21%. Conventional pthread barrier performed worst compared to barrier sense with almost double L1 cache miss of 4.45%.

### Perf Performance for My Sort

* Performance analysis of the `mysort.c` code with TAS, TTAS, Ticket and MCS lock is given below in the table.

| Primitive    | Runtime (s) | L1 cache hit (%) | Branch Pred Hit Rate (%) | Page Fault Count (#)|
|--------------|-------------|------------------|--------------------------|---------------------|
| TAS lock              | 25.036418 Sec  | 1.21% | 0.01% | 1327 |
| TTAS lock             | 19.073345 Sec  | 0.36% | 0% | 1328 |
| Ticket lock             | 13.383299 Sec  | 0.36% | 0% | 1328 |
| MCS lock           | 6.220664 Sec  | 0.24% | 0% | 1326 |
| Pthread barrier         | 13.383299 Sec  | 1.09% | 0.03% | 1337 |
| Barrier Sense           | 6.875626 Sec  | 0.94% | 0.01% | 1336 |

* Performance analysis of the my sort code with two threads using Peterson's lock is given below in the table.

| Primitive    | Runtime (s) | L1 cache hit (%) | Branch Pred Hit Rate (%) | Page Fault Count (#)|
|--------------|-------------|------------------|--------------------------|---------------------|
| Peterson's Release Consistancy lock              | 12.834787 Sec  | 0.56& | 0% | 1329 |
| Peterson's Sequencial Consistancy lock           | 12.794341 Sec  | 0.55% | 0% | 1328 |

According to the above table and the performance obtained from 'perf,' MCS lock provides the best results in terms of runtime, L1 cache hit, and branch pred hit rate, followed by ticket lock and TTAS lock. In terms of L1 cache hit percentage, Peterson's Sequencial Consistency lock also performs admirably. In terms of barriers, Barrier sense outperformed conventional pthread barriers significantly.

TAS lock, on the other hand, was the worst performer, with a significantly high cache hit percentage of 1.21%. Based on the above results, pthread barriers are the worst performing concurrency practices.

## Code Structure

### Counter Algorithm
Counter function uses different concurrency premetives to increment a counter variable for certain number of iterations. The counter will use command line arguments in below format. The concurrency premetives are build using atomic variables. The no of iterations to be worked upton by each thread is fetched using command line arguments. The final count variable is also stored inside a destination text file.

The code work flow is as follows 
* Input number of threads, barrier to be selected, lock to be selected, no of iterations, and destination file to store final count from command line argument using getopt_long
* Based on barrier selected or thread selected individual thread functions are invoked for each thread. 
* Either if its a lock or barrier each thread will use their respective concurrency premetives to inceremnet the counter from i to `ITERATIONS`. For example, thread function created for TAS lock will use TAS related concurrency premetives `TAS_Lock()` and`TAS_unlock` to perform the conter increment operation
* Finally the final count value i stored in destination file using File IO operation.

### My Sort Algorithm
The program sorts the integers stored in the text file using bucket sort algorithm and stores it in the other text file. To prevent data races the custom builded concurrency premetives are created using atomic varible and atomic operations. Each thread works on the respective buckets, sorts it and once all thread performs sorting on their respective buckets, contents are merged inside master thread. The sorted outpput is stored inside a seperate destination text file.

The code flow is as follows :

* Fetch the integers from surce text file obtained from command line argument using File IO operation
* Input number of threads, barrier to be selected, lock to be selected, no of iterations, and destination file to store final count from command line argument using getopt_long
* Either if its a lock or barrier each thread will use their respective concurrency premetives to to complete their sorting operation on respective buckets. For example, thread function created for TAS lock will use TAS related concurrency premetives `TAS_Lock()` and`TAS_unlock` to perform the conter increment operation
* Finally once thread operations are done, the buckets are merged and added back into final array and stored inside a seperate destination text file using File IO operation

Both `conter.c` an `mysort.c` uses the custom concurrency premetives as follows : 

#### TAS Lock 
```
_Atomic _Bool * tas_lock_flag      = 0;
void tas_lock(void)
{
	while( (atomic_flag_test_and_set_explicit(&tas_lock_flag,memory_order_seq_cst)) == 1){};
}
	
void tas_unlock(void)
{
	atomic_store_explicit(&tas_lock_flag,0,memory_order_seq_cst); // store zero in flag
}
```

#### TTAS Lock
```
_Atomic _Bool * ttas_lock_flag     = 0;
void ttas_lock(void)
{
	while(((atomic_load_explicit(&ttas_lock_flag,memory_order_seq_cst)) == 1 ) || ( (atomic_flag_test_and_set_explicit(&ttas_lock_flag,memory_order_seq_cst)) == 1 )) {};
}
	
void ttas_unlock(void)
{
	atomic_store_explicit(&ttas_lock_flag,0,memory_order_seq_cst); // store zero in flag

}
```
#### Ticket Lock
```
_Atomic int   * ticket_next_num    = 0;
_Atomic int   * ticket_now_serving = 0;
void ticket_lock(void)
{
	int my_num = atomic_fetch_add_explicit(&ticket_next_num,1,memory_order_seq_cst);
	while( (atomic_load_explicit(&ticket_now_serving,memory_order_seq_cst)) != my_num ){};
}
void ticket_unlock(void)
{
	atomic_fetch_add_explicit(&ticket_now_serving,1,memory_order_seq_cst);
}
```
#### MCS Lock
```
/* Structure definations for MCS lock */
struct spinlock_node {
   _Atomic struct spinlock_node *next;
   _Atomic _Bool wait;
}node;
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

struct spinlock {
   _Atomic struct spinlock_node * tail;
}lock;
```

#### Barrier Sense
```
_Atomic int   * bar_sense_cnt      = 0;
_Atomic int   * bar_sense_sense    = 0;

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
	while(atomic_load_explicit(&bar_sense_sense,memory_order_seq_cst) != my_sense)		{}
}	
```
#### Peterson's Sequencial Consistancy lock
```
_Atomic _Bool * desires_seq;
_Atomic int     turn_seq;
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
```
#### Peterson's Released Consistancy lock
```
_Atomic _Bool * desires_rel;
_Atomic int     turn_rel;
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
```

Some additional functions for bucket sort used are : 
1. thread_bucket_sort
2. selection_sort
3. max_element

##### thread_bucket_sort() Function 
void *thread_bucket_sort(void * arg)

The thread bucket sort() function is a pthread function call that allows the thread to function. The function receives the head and tail of the group of elements it is responsible for loading into buckets. Each thread will receive a subset of the elements from the main unsorted array. The thread function will load buckets from these elements and pass them to the main function. After each thread has finished filling the buckets, the main function will sort them.

##### selection_sort() function

void selectionSort(int * arr, int n);

The selection sort() function is in charge of sorting buckets after the array of buckets has been filled by the thread functions. The selection sort() function performs the final sorting before data is loaded into the destination text file. It takes array Buckets() and sortes each 1D array in it.

##### max_element() function

int max_element(int * array, int size);

The max elemnt() function is in charge of returning the maximum value from an array. The function is used here to find the maximum value in the array. This maximum value is used to calculate the elements of the mins[] array. 











