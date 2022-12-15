# Lab 1 : Assignemnt Submittion 
Program to sort the unsorted integers from source file and write it into destination text file using fork join or bucket sort algorithm
#### Submitted By : Kiran Narendra Jojare

## Summery
The program sorts the integers stored in the text file using a fork join using merge sort or bucket sort algorithm and stores it in the other text file. The program passes the command line arguments to the main function in the below format.

$ .mysort [--name] [source_file] -o [destination_file] [--alg==<forkjoin/lkbucket>]

On successful compilation, the program will sort the elements in the txt file at source and write it onto the destination file mentioned in command line arguments.

For example:  $./mysort source_test.txt -o destination.ans --alg=forkjoin 
									OR
			  $./mysort source_test.txt -o destination.ans --alg=lkbucket 

## Code Description
The code flow is as follows :

  -- Fetch the integers from surce text file obtained from command line argument using File IO operation
  -- Based on the command line argument value of variable --alg, sort the data using fork join algorithm or bucket sort.
  -- In the case of fork join , the functions merge_sort() and  thread function named thread_merge_sort() will sort an unsorted list of integers into a sorted array.
  -- In the case of lkbucket sort, functions like bucketsort() and thread function thread_bucket_sort() will sort the list of unsorted integers into a sorted array.
  -- The sorted list of integers will be stored in the destination file using File IO operations. The name of the destination file and format are taken from command line arguments.
  
The code uses the C file "mysort_lab1.c" and the "Makefile" for compilation. 
mysort.c contains the following functions:


## Code Observation
The code observation sand parallelization stratergies are discussed as below :  
### Code Performance 
The code performance in parallel was significantly faster and improved when compared to sequencial. However, according to the output of the atomic clock withing program, bucket sort was found to be less time consuming than both fork join merge sort and bucket sort.

### Code Parallelism 
For Bucket Sort : 
The difference between the serial and parallel versions of merge sort is that we have divided the unsorted array into groups of arrays, and each thread works on these groups of arrays. The threads are in charge of filling the buckets with the element. When all threads have finished filling buckets for array they recieved, the final merge is performed at the main function.

For Fork Join Merge Sort : 
The difference between the serial version and the parallel version of merge sort is that we have used parallel task to figure out the bounds of the array to operate on, during the initial function call and later they are sorted locally. These locally sorted array by threads are then given to main function where local array's are merged back to one final sorted array.

## Functions Used 

### Bucket Sort :
1. thread_bucket_sort
2. selection_sort
3. max_element

#### thread_bucket_sort() Function 
v
oid *thread_bucket_sort(void * arg)

The thread bucket sort() function is a pthread function call that allows the thread to function. The function receives the head and tail of the group of elements it is responsible for loading into buckets. Each thread will receive a subset of the elements from the main unsorted array. The thread function will load buckets from these elements and pass them to the main function. After each thread has finished filling the buckets, the main function will sort them.

#### selection_sort() function

void selectionSort(int * arr, int n);

The selection sort() function is in charge of sorting buckets after the array of buckets has been filled by the thread functions. The selection sort() function performs the final sorting before data is loaded into the destination text file. It takes array Buckets() and sortes each 1D array in it.

#### max_element() function

int max_element(int * array, int size);

The max elemnt() function is in charge of returning the maximum value from an array. The function is used here to find the maximum value in the array. This maximum value is used to calculate the elements of the mins[] array. 


### Fork Join Merge Sort :
1. thread_merge_sort
2. merge1
3. merge_sections_of_array

#### thread_merge_sort()

void* thread_merge_sort(void* arg);

For the thread to work recursively, the above function is passed to the pthread call. It first obtains the thread id before locating the array's left and right bounds. Following that, it runs the standard merge sort. The difference between the serial and parallel versions is that during the initial function call, we must determine the bounds of the array on which we will operate.

#### merge_sort() function : 

void merge_sort(int * arr, int low,int up)

The merge_sort function performs the merge sort algorithm over the selected array of unsorted data.

The merge sort function is responsible for breaking the source array into two separate arrays. Sorting these two separate arrays and merging them back together to create a new sorted array.

When merge_sort() is called for the first time, it calculates the midpoint to sagrigate the array into two sublists. Later, the same function is called to sort the left sublist from 0 to mid and the right sublist from mid +1 to 1. After two sublists are sorted using recursion, they are merged and stored using the function merge1().


#### merge1() function

void merge1(int * arr,int low,int mid, int up)

The merge1 function, as the name suggests, will merge the two unsorted arrays created by the merg_sort function. The function compares each element from both the lists and stores the comparison in the third array. Since the two arrays at the input of this function are already sorted, only one pass is needed to sort them.

Initially, i,j and k point to the beginning of three arrays. Left[i] and Right[j] are compared, and the smallest element is stored at arr[k].Variable k is incremented and one of the variable i or j is incremented.










