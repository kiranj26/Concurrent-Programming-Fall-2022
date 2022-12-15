/***********************************************/
/*     Lab 1  : Concurrent Programming         */
/*     Author : Kiran Jojare                   */
/***********************************************/

#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/time.h>
#include <time.h>
#include <limits.h>

/* Preprocessing Data */
#define MAX 10

/* Global Function Defination */
// for fork join merge sort algorithm
void merge_sort(int * arr, int low,int up);
void merge1(int * arr,int low,int mid, int up);
void* thread_merge_sort(void* arg);
void merge_sections_of_array(int arr[], int number, int aggregation);

// for bucket sort algorithm
void swap(int* xp, int* yp);
void selectionSort(int * arr, int n);
void *thread_bucket_sort(void * arg);
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

pthread_barrier_t bar,bar1;
pthread_mutex_t lk,lk1;

struct timespec start, end;

/***********************************************/
/*                  Main                       */
/***********************************************/
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
    if (thread_cnt_recieved != 1 || thread_count > 15)
    {
        thread_count = 4;
    }
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
	
    /* Run Selected Algorithm */
    /* 1. Run Fork Join Sort Algorithm */
    if (((strcmp(alg_selected,"forkjoin"))) == 0)
    {   
        /* Barrier & Lock Initialisation */
		pthread_barrier_init(&bar, NULL, thread_count);
		pthread_mutex_init(&lk,NULL);	
        /* thread creation */
        for(long i=0; i< thread_count;i++)
        {
            int pid = pthread_create(&threads[i],NULL,thread_merge_sort,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
        
        /* wait for threads to finish the jobs */
        for(long i=0; i< thread_count;i++)
        {
            pthread_join(threads[i],NULL);
        }
        merge_sections_of_array(arr, thread_count, 1);
        printf("Fork Join Sorted Using Merge Sort\n");
		
		/* deinitialise syncronisation */
		pthread_barrier_destroy(&bar);
		pthread_mutex_destroy(&lk);
  
		/* File IO : Opening the source file from command line argument */
		destination_file = fopen(destination_txt,"w+");
		for(int i=0;i<count;i++)
		{
			fprintf(destination_file,"%d\n",arr[i]);
		}

		fclose(destination_file);
		free(arr);
		/* Clock Time */
		unsigned long long elapsed_ns;
		elapsed_ns = (end.tv_sec-start.tv_sec)*1000000000 + (end.tv_nsec-start.tv_nsec);
		printf("Elapsed (ns): %llu\n",elapsed_ns);
		double elapsed_s = ((double)elapsed_ns)/1000000000.0;
		printf("Elapsed (s): %lf\n",elapsed_s);
    }
	/* 1. Run Fork Join Sort Algorithm */
    if (((strcmp(alg_selected,"lkbucket"))) == 0)
    {
		/* Initialise barrier */
		pthread_barrier_init(&bar, NULL, thread_count);
		pthread_mutex_init(&lk1,NULL);
		/* Calculate max value from the given unsorted data */
		int max = max_element(arr,count);
		bucket_count = count/thread_count;
		bucket_length = count;
		if(count > 30000)
		{
			bucket_length = 500;
		}
		printf("\n Bucket_Length = %d\n",bucket_length);
		
		/* Initialise buckets */
		buckets = (int**)malloc(bucket_count * sizeof(int*));
		for (int i=0;i<bucket_count;i++)
		{
			buckets[i] = (int*)malloc(bucket_length * sizeof(int));
		}
		/* Filling Bucket with Zero */
		for ( int i=0;i<bucket_count;i++)
		{
			for(int j=0;j<bucket_length;j++){
				buckets[i][j]= 0;
			}
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
				mins[i] = max+1;}// +1 coz in distribution it will leave the last value
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
		
		/* Thread creation */
		for(long i=0; i<thread_count;i++)
        {
            int pid = pthread_create(&threads[i],NULL,thread_bucket_sort,(void *)i);
            if(pid){ printf("Error creating thread\n");
                    exit(-1);
            }
        }
		/*  Waiting for thread operation */
		for(long i=0; i< thread_count;i++)
        {
            pthread_join(threads[i],NULL);
        }
		for (int i=0;i<bucket_count;i++)
		{	selectionSort(buckets[i],bucket_length);
		}
		/*  Writting buckets into array arr */
		int k=0;
		destination_file = fopen(destination_txt,"w+");
		
		/*  Sorted Buckets Aritten into arr */
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
		/* Destroy Mutex */
		pthread_mutex_destroy(&lk);
		
		/* Free dynamic memory allocated */
		free(counter);
		free(mins);
		for (int i=0;i<bucket_count;i++)
		{
			free(buckets[i]);
		}
		fclose(destination_file);
		
		/*  Clock Time */
		unsigned long long elapsed_ns;
		elapsed_ns = (end.tv_sec-start.tv_sec)*1000000000 + (end.tv_nsec-start.tv_nsec);
		printf("Elapsed (ns): %llu\n",elapsed_ns);
		double elapsed_s = ((double)elapsed_ns)/1000000000.0;
		printf("Elapsed (s): %lf\n",elapsed_s);
	}
free(destination_txt);
free(alg_selected);
}
/***********************************************/
/***********************************************/
/*               Functions Used                */
/***********************************************/
/***********************************************/

/***********************************************/
/*            Merge Sort Function              */
/***********************************************/

void merge_sort(int * arr, int low,int up)
{   
    if (low<up)
    {
        int mid = (low+up)/2;         // middle index
        merge_sort(arr,low,mid);      // sort left sublist of array
        merge_sort(arr,mid+1,up);     // sort right sublist of array
        merge1(arr,low,mid,up);       // merge two sorted part   
    }
}
/***********************************************/
/*              Merge Function                 */
/***********************************************/

void merge1(int * arr, int low, int mid, int up)
{
    int i, j, k;
    int n1 = mid - low + 1;
    int n2 =  up - mid;
    // Dynamically allocate two arrays
    int * Left = (int *)malloc(n1 * sizeof(int *));
    int * Right = (int *)malloc(n2 * sizeof(int *));
    // Copy data to temp arrays Left[] and Right[]
    for (i = 0; i < n1; i++)
        Left[i] = arr[low + i];
    for (j = 0; j < n2; j++)
        Right[j] = arr[mid + 1+ j];
     //Perform sorting based on indices
     i = 0;j = 0;k = low;      

     while (i < n1 && j < n2)
     {
         if (Left[i] <= Right[j])
         {
             arr[k] = Left[i];
             i++;
         }
         else
         {
             arr[k] = Right[j];
             j++;
         }
         k++;
     }
     // Store remaining elemnts from Left Array
     while (i < n1)
     {
         arr[k++] = Left[i++];
     }

     // Store remaining elemnts from Left Array
     while (j < n2)
     {
         arr[k++] = Right[j++];
     }
	free(Left);
	free(Right);
}
/***************************************************/
/*         Threaded Merge Function                 */
/***************************************************/
void *thread_merge_sort(void* arg)
{   
    int thread_id = (long)arg;           
	pthread_barrier_wait(&bar);
	
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	
	printf("Thread %u reporting for duty\n",thread_id);
	// load left and right index of array 
	// allocated to thread function
    int left = thread_id * (no_per_thread);
    int right = (thread_id + 1) * (no_per_thread) - 1;
    if (thread_id == thread_count - 1) {
        right += offset;
    }
    int middle = left + (right - left) / 2;
	
	
	// sort the individual array and merge back
	// to maiin arr
    if (left < right) {
        merge_sort(arr, left, right);
        merge_sort(arr, left + 1, right);
        merge1(arr, left, middle, right);
    }
	
	
	pthread_barrier_wait(&bar);
	
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
    return NULL;
}
/***********************************************/
/*         Merge Selection Function            */
/***********************************************/
void merge_sections_of_array(int arr[], int number, int aggregation) {
    
	for(int i = 0; i < number; i = i + 2) {
        int left = i * (no_per_thread * aggregation);
        int right = ((i + 2) * no_per_thread * aggregation) - 1;
        int middle = left + (no_per_thread * aggregation) - 1;
        if (right >= count) {
            right = count - 1;
        }
		
		// segments are merged pairwise 
		// recursively
        merge1(arr, left, middle, right);
    }
    if (number / 2 >= 1) {
        merge_sections_of_array(arr, number / 2, aggregation * 2);
    }
}

/************************************************/
/*        Threaded Bucket Sort Function         */
/************************************************/
void *thread_bucket_sort(void * args)
{  	
	int thread_id = (int)args;
	printf("Thread %u reporting for duty\n",thread_id);
	
	// loading thread data into local variable
	pthread_mutex_lock(&lk1);
	int head = td[thread_id].head;
	int tail = td[thread_id].tail;

	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	// lock aquired filling data into 
	// buckets to be sorted
	printf("Thread %u got the lock!\n",thread_id);
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
				buckets[j-1][index] = arr[i];				
				break; 
			}
		}	
	}
	
	pthread_mutex_unlock(&lk1);
	// lock released
	if(thread_id==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
	return 0;
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