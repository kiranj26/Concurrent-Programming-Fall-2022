#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/time.h>
#include <time.h>
#include <limits.h>
#define MAX 10
	
void local_init(){}
void global_init(){}
void local_cleanup(){}
void global_cleanup(){}

int no_per_thread;
int offset;
int thread_count;
int bucket_count;
int bucket_length;
int count;

pthread_mutex_t lk, lk1;
pthread_barrier_t bar;

struct timespec start, end;


int * arr;
int * mins;
int ** buckets;
int * counter;

struct tData {
	int *array;
	int head;
	int tail;
	int tid;
}td[MAX];

void swap(int* xp, int* yp);
void selectionSort(int * arr, int n);
void *thread_bucket_sort(void * arg);
void merge_sections_of_array(int arr[], int number, int aggregation);
void bucket_sort(int * arr,int no_threads);
void printArray(int * arr,int size);
int max_element(int * array, int size);


int main(int argc, char *argv[])
{
    int c = 0;
    
    int option_index = 0;
    int thread_cnt_recieved = 0;
    char * alg_selected;
    char * destination_txt;
    
    alg_selected = (char*)malloc(30*sizeof(char));
    destination_txt = (char*)malloc(50*sizeof(char));
    
    /* Parsing Command Line arguments */
    static struct option long_options[] ={
    {"name",0,0,'n'},              // long option 1
    {"destination",1,0,'o'},       // long option 2
    {"thread",1,0,'t'},            // long option 3
    {"alg",1,0,'b'},               // long option 4
    {0,0,0,0},
    };
	
	/* Fetch Command Line Argument Using Switch Case */
    while((c = getopt_long(argc,argv, "no:t:b:", long_options,&option_index)) != -1)
    {
        switch (c)
        {
        case 'n':
            printf("Student name : Kiran Narendra Jojare\n");
            exit(1); //print name and exit code
            break;
        case 'o':
            strcpy(destination_txt, optarg);
            printf("Destination file name : %s\n",destination_txt);
            break;
        case 't':
            thread_count = atoi(optarg);

            thread_cnt_recieved = 1;
            break;
        case 'b':
            strcpy(alg_selected, optarg);
            printf("Algortithm Selected : %s\n",alg_selected);
            break;
        default :
            printf("Default \n");
            break;
        }
    }
    /* Configure default thread counts */
    if (thread_cnt_recieved != 1)
    {
        thread_count = 4;
    }
	//td[thread_count];
    printf("No of threads selected : %d\n",thread_count);
    
	/* Take soure file name from command line argument */
    if (thread_cnt_recieved != 1)
    {
		printf("Source file name : %s\n",argv[4]); 
    }else
    {
		printf("Source file name : %s\n",argv[6]); 
    }
    
    /*  Print selected sorting algorithm */
    printf("Sorting Algortitm (alg_selected) Selected : %s\n",alg_selected);
    
    /* Take Data From Source File */
    arr = (int *)malloc(7000001*sizeof(int *));
	
    //printf("Address of array = %ld\t at main\n", &arr[0]);
    
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
    /* Run Selected Algorithm */
    
    /* 1. Run Fork Join Sort Algorithm */
    if (((strcmp(alg_selected,"lkbucket"))) == 0)
    {   
		pthread_barrier_init(&bar, NULL, thread_count);
		/* Initialise Buckets */
		int max = max_element(arr,count);
		bucket_count = count/thread_count;
		bucket_length = count/2;
		buckets = (int**)malloc(bucket_count * sizeof(int*));
		for (int i=0;i<bucket_count;i++)
		{
			buckets[i] = (int*)malloc(bucket_length * sizeof(int));
		}
		// Filling Bucket with Zero
		for ( int i=0;i<bucket_count;i++)
		{
			for(int j=0;j<bucket_length;j++){
				buckets[i][j]= 0;
			}
		}
		mins = (int *)malloc((bucket_count+1)*sizeof(int *)); // int mins[thread_count+1]
		counter = (int*)malloc(bucket_count * sizeof(int));  
		for ( int i=0;i<bucket_count;i++)
		{
			counter[i] = 0;
		}
		printf("\n");
		for(int i=0; i<=bucket_count; i++){
			if(i==bucket_count)
				mins[i] = max+1;// +1 coz in distribution it will leave the last value
			else
				mins[i] = i * (max/bucket_count);
		}
        /* Barrier Initialisation */
		pthread_mutex_init(&lk,NULL);	
		pthread_mutex_init(&lk1,NULL);
        /* thread creation */
        int fragsSize = count / thread_count;
		int remElements = count - (fragsSize * thread_count);
   
		int temp=0;
		for(size_t i=0; i<thread_count;i++){
			td[i].array = arr;
			td[i].head = temp;
			td[i].tail = td[i].head + fragsSize -1;
       
			if(remElements){
				td[i].tail++;
				remElements--;
			}
			temp = td[i].tail+1;
			td[i].tid = i+1;
		}
		for(long i=0; i<thread_count;i++)
        {
            int pid = pthread_create(&threads[i],NULL,thread_bucket_sort,(void *)&i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }		
		for(long i=0; i< thread_count;i++)
        {
            pthread_join(threads[i],NULL);
        }
		printf("Data sorted using Bucket Sort with %d Threads\n",thread_count);
		
			// sorting buckets
		for (int i=0;i<bucket_count;i++)
		{	selectionSort(buckets[i],bucket_length);
		}
		int k=0;
		destination_file = fopen(destination_txt,"w+");
		// Sorted Buckets Aritten into arr
		for ( int i=0;i<bucket_count;i++)
		{
			for(int j=0;j<bucket_length;j++){
			if ((buckets[i][j]) != 0)
			{
				fprintf(destination_file,"%d\n",buckets[i][j]);
				arr[k++] = buckets[i][j];
			}
			}
		}
		printArray(arr,count);
		/* deinitialise syncronisation */
		pthread_mutex_destroy(&lk);
		unsigned long long elapsed_ns;
		elapsed_ns = (end.tv_sec-start.tv_sec)*1000000000 + (end.tv_nsec-start.tv_nsec);
		printf("Elapsed (ns): %llu\n",elapsed_ns);
		double elapsed_s = ((double)elapsed_ns)/1000000000.0;
		printf("Elapsed (s): %lf\n",elapsed_s);
    }
	//printArray(arr,count);
    free(alg_selected);
    fclose(destination_file);
    free(destination_txt);
	free(arr);
	free(buckets);
	free(mins);
	free(counter);
	
}

void bucket_sort(int * arr,int no_threads){
    printf("Inside Bucket Sort\n");
}

void *thread_bucket_sort(void * args)
{  	
	//pthread_barrier_wait(&bar);
	int thread_id = (*(int *)args);
	thread_id -= 1;
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;

	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	
	// inserting elements into buckets
	pthread_mutex_lock(&lk1);
	for( int i=head;i<=tail;i++)
	{
		for (int j=0;j<=bucket_count;j++)
		{
			if (arr[i] <= mins[j])
			{   // Insert elements into bucket
				int index = counter[j-1];
				counter[j-1] = counter[j-1] +1;
				buckets[j-1][index] = arr[i];
				break;
			}
		}	
	}
	printf("Thread %zu reporting for duty\n",thread_id);
    printf("Thread %zu got the lock!\n",thread_id);
	pthread_mutex_unlock(&lk1);

	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
	return 0;
}
void printArray(int * arr,int size)
{
	for(int i=0;i<size;i++)
	{
		printf("%d ",arr[i]);
	}
	printf("\n");
}
int max_element(int * array, int size) 
{  
    // Initializing max variable to minimum value so that it can be updated
    // when we encounter any element which is greater than it.
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
void swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
 
// Function to perform Selection Sort
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