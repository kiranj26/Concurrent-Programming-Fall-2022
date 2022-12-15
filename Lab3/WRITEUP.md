# Lab 3 : Sorting Open MP 
Student Name : Kiran Narendra Jojare 
Section      : Graduate Student in ECEN

## Summary:
The outcome of Lab 3 is to use Open MP parrallel programming environment to parallelise the sorting algoritm for both merge sort or quick sort. 

Note : For additional credits both Merge Sort and Quick Sort are employed in this code. If an algorithm is not provided by the user, the quick sort will be used by default.

The program passes the command line arguments to the main function in the below format.

`mysort [--name] [source.txt] [-o out.txt] [-t NUMTHREADS]  [--alg=<merge,quick>]`

On successful compilation, the program will sort the elements in the txt file at source and write it onto the destination file mentioned in command line arguments.

For example:  $./mysort source_test.txt -o destination.ans --alg=merge -t no_thread

## Ease of parallelization between pthreads and OpenMP

In comparison to pthread, OpenMP makes it easier to parallelize code, debug it, and tune it, which speeds up the development of the code. It prevents the programmer from having to rewrite the challenging thread synchronizations, making compute-bound applications more precisely tuned for speed. The development of thread-safe data structures is made easier by OpenMP. By assigning priorities to threads as they are being executed, the highly tuned OpenMP runtime scheduling mechanisms reduce the burden of writing one's own scheduling mechanism. Compared to the OpenMP version, the pthread code proves to be more complex.The directives can be gradually added, resulting in some parts of the code running serially and others gradually parallelizing. As a result, the code is simpler to comprehend, easier to maintain, and easier to parallelize than the pthread version. 

## Parallelisation Stratergy

The serial recursive merge and quick sort could be used with Open MP parallel directive #pragma omp parallel. Each thread will recieve a task where they will work on a shared input array. As seen below the omp parallel will invoke the compiler about parallel behaviour implemented in the program. The first thread will start working on merge sort using #pragma omp single. The purpose here is to Create pool of threads and start with one of them. The recurive work will be taken cared by the task group. 

Finally merge function will work on araay sorted by each thread and will load values back into destination text file.

```
#pragma omp parallel num_threads(thread_cnt)
{
	#pragma omp single    // invoke first thread to work on sorting algorithm
	merge_sort_parallel / quick_sort_parallel 
}
```
```
void merge_sort_parallel(int * arr, int low,int up)
{   
    if (low<up)
    {
        int mid = (low+up)/2;        
		#pragma omp taskgroup     
		{   // array arr will be shared among the threads
			#pragma omp task shared(arr)
			merge_sort_parallel(arr,low,mid);      
			
			#pragma omp task shared(arr)
			merge_sort_parallel(arr,mid+1,up);     
		}
		// waiting for all innner threads
		merge1(arr,low,mid,up);      
	}
}
```
```
void quick_sort_parallel(int * arr, int low, int up)
{
    int pivloc;
    if(low>up)  
    {
        return;   
    }
    pivloc = partition(arr,low,up);   
    #pragma omp taskgroup
	{	
		#pragma omp task shared(arr)
		quick_sort_parallel(arr,low,pivloc -1);     
		#pragma omp task shared(arr)
		quick_sort_parallel(arr,pivloc+1,up);  
	}
	// waiting for all innner threads
}
```

## Code Perfromance 

The average code performance for the serial sort and parallel sort is given in the table. The time taken is the average time it took for all the test cases.

| Sorting Algorithm    | Serial Runtime (s) | Parallel Runtime (s) |
|--------------|-------------|-------------|
| Quick Sort           | 0.6899 sec  | 1.213 sec  |
| Merge Sort           | 0.9865 sec  | 1.657 sec  |

The open MP was able to reduce the speed of program by almost 1.35X faster as seen above in the table

## Code Description
The code flow is as follows :

  * Fetch the integers from surce text file obtained from command line argument using File IO operation
  * Based on the command line argument value of variable --alg, sort the data using merge sort or quick sort.
  * In the case of merge sort, the functions merge_sort() and merge1() will sort an unsorted list of integers into a sorted array.
  * In the case of quick sort, functions like quick_sort() and partition() will sort the list of unsorted integers into a sorted array.
  * The sorted list of integers will be stored in the destination file using File IO operations. The name of the destination file and format are taken from command line arguments.
  
The code uses the C file "mysort.c" and the "Makefile" for compilation. 
mysort.c contains the following functions:
1. merge_sort
2. merge1
2. quick_sort
3. partition


### merge_sort() function : 
void merge_sort(int * arr, int low,int up)

The merge_sort function performs the merge sort algorithm over the selected array of unsorted data.

The merge sort function is responsible for breaking the source array into two separate arrays. Sorting these two separate arrays and merging them back together to create a new sorted array.

When merge_sort() is called for the first time, it calculates the midpoint to sagrigate the array into two sublists. Later, the same function is called to sort the left sublist from 0 to mid and the right sublist from mid +1 to 1. After two sublists are sorted using recursion, they are merged and stored using the function merge1().


### merge1() function
void merge1(int * arr,int low,int mid, int up)

The merge1 function, as the name suggests, will merge the two unsorted arrays created by the merg_sort function. The function compares each element from both the lists and stores the comparison in the third array. Since the two arrays at the input of this function are already sorted, only one pass is needed to sort them.

Initially, i,j and k point to the beginning of three arrays. Left[i] and Right[j] are compared, and the smallest element is stored at arr[k].Variable k is incremented and one of the variable i or j is incremented.

### quick_sort function
void merge1(int * arr,int low,int mid, int up)

A quick sort takes a pivot element from the list of unsorted elements. The list is broken based on pivot position using function partition. After partition, the function will make sure all elements to the left of the pivot are less than or equal to the pivot. Similarly, all elements to the right of the pivot are greater than or equal to the pivot. The quick_sort () function uses recursion to sort the partitions till only one element is left in the list.

The function initially takes the first element from an unsorted list and makes it a pivot by placing it in a location where all elements to its left are less than the pivot and elements to its right are greater than the pivot. This is done using the partition() function. Once that's done, two sublists to the left and right of the pivot are generated. Both are sorted using quick sort recursively until there is only one element left in the sublist. At the end, all individual positions will have been sorted. 


### partition function
int partition(int arr[], int low, int up)

Function partition() will place pivot at the proper place nd then return the location of pivot so that we can form sublists again besed on pivot.Function return th pivot elemnts for merge_sort()


## File IO operations
Code uses two file IO operations for reading and writting into files before and after sorting is performed. The file IO functios used are fopen(),fscaf() and fprint(). Also fclose() function is also used to free memeory used by function oeprations.

## Command line parsing
The command line arguments are parsed using a modified version of getopt() function called getopt_long(). After program invocation, command line arguments [--name] , [--alg=<merge/quick>], [-o destination_file] are passed into function. the extended long options [--name], [--destination], [--alg] and [--source_file] is used by the main algorithm.

## Extent Bugs
No extent bug to be reported. Code works fine for all the test cases given as per assignment requirements.

