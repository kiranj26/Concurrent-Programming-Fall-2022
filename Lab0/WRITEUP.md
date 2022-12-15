# Lab 0 : Assignemnt Submittion 
Program to sort the unsorted integers from source text file and write it into destination text file using merge or quick sort
#### Submitted By : Kiran Narendra Jojare

## Summery
The program sorts the integers stored in the text file using a quick sort or merge sort algorithm and stores it in the other text file. The program passes the command line arguments to the main function in the below format.

$ .mysort [--name] [source_file] -o [destination_file] [--alg==<merge/quick]

On successful compilation, the program will sort the elements in the txt file at source and write it onto the destination file mentioned in command line arguments.

For example:  $./mysort source_test.txt -o destination.ans --alg=merge

## Code Description
The code flow is as follows :

  -> Fetch the integers from surce text file obtained from command line argument using File IO operation
  -> Based on the command line argument value of variable --alg, sort the data using merge sort or quick sort.
  -> In the case of merge sort, the functions merge_sort() and merge1() will sort an unsorted list of integers into a sorted array.
  -> In the case of quick sort, functions like quick_sort() and partition() will sort the list of unsorted integers into a sorted array.
  -> The sorted list of integers will be stored in the destination file using File IO operations. The name of the destination file and format are taken from command line arguments.
  
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

