#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<string.h>

/* Preprocessing data */
#define MAX 300

/* Function prototype */
void merge_sort(int * arr, int low,int up);
void quick_sort(int * arr, int low, int up);
void merge1(int * arr,int low,int mid, int up);
int partition(int * arr, int low, int up);


/* Main Function */
int main(int argc, char *argv[])
{
    int count = 0;

    char alg_selected[MAX];
    char destination_txt[MAX];

    /* Parsing Command Line arguments */
    int c = 0;int option_index=0;
    /* Long Option Structure
	Three long options are
	--name        -> reflects Studnet ID i.e ./mysort --name $ <Student Name>
	--destination -> reflects final object file name i.e -o <out.txt> 
	--alg         -> refelect the algorithm to be used i.e --alg==merge or --alg==quick
	*/
    static struct option long_options[] ={
    {"name",0,0,'n'},              // long option 1
    {"destination",1,0,'o'},       // long option 2
    {"alg",1,0,'b'},               // long option 3
    {0,0,0,0},
    };

	/* Fetch Command Line Argument Using Switch Case */
    while((c = getopt_long(argc,argv, "no:b:", long_options,&option_index)) != -1)
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
        case 'b':
            strcpy(alg_selected, optarg);
            printf("Algortithm Selected : %s\n",alg_selected);
            break;
        default :
            printf("Default \n");
            break;
        }
    }

	/* Take soure file name from command line argument */
    printf("Source file name : %s\n",argv[4]); 

    /*  Print selected sorting algorithm */
    printf("Sorting Algortitm (alg_selected) Selected : %s\n",alg_selected);

    /* Take Data From Source File */
    int * arr;
    arr = (int *)malloc(10000001*sizeof(int *));
                       
    FILE* source_file;
    FILE* destination_file;

    /* File IO : Opening the source file from command line argument */
    int i=0;
    source_file = fopen(argv[4],"r");
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

    /* Run Selected Algorithm */
    /* 1. Run Merge Sort Algorithm */
    if (((strcmp(alg_selected,"merge"))) == 0)
    {
        merge_sort(arr,0,n-1); // call merge sort function with data from 0 to n-1
        printf("Merge Sorted\n");
        printf("\n");
    }
    /* 2. Run Quick Sort Algorithm */
     if (((strcmp(alg_selected,"quick"))) == 0 )
    {
        quick_sort(arr,0,n-1);// call quick sort function with data from 0 to n-1
        printf("Quick Sorted\n");
        printf("\n");
    }

    /* File IO : Opening the source file from command line argument */
    /* Print Data into destination text file */
    destination_file = fopen(destination_txt,"w+");
    for(int i=0;i<count;i++)
    {
        fprintf(destination_file,"%d\n",arr[i]);
    }
    fclose(destination_file);

    free(arr);

}

/* -------------- Function Definations ------------------*/

/* Function to call merge sort unsorted array of numbers */
/*
Function takes the array to be sorted called arr and lower and upper limit of the same array
Sorts the two part of array from 0 to 'middle index' and 'middle index +1' to 'highest index'
Once these two parts are sorted it merges them using merge1 function into seperate array called temp
Finally it copied the sorted array to temp array
When merge_sort() called for first time it calculates mid. Later the same function is called to sort left sublist from 0 to mid and mid +1 to 1. After two sublist are sorted using recursion, they are merged and stored suing function merge1().
*/
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

/* Function to merge two sorted array together */
/*
Merge Function merges the two different parts of sorted array done after merge_sort() function
Merge takes one element from each of the sorted array compares them and then stores it to the new sorted  array.
In other words it merges the two sorted array and creates a combined new array which contains sorted elemnts only.
Initially, i,j and k point to the beginning of three arrays. Left[i] and Right[j] are compared, and the smallest element is stored at arr[k].Variable k is incremented and one of the variable i or j is incremented.
*/
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
 }

/* Function for quick sort algorithm */
/* 
A quick sort takes a pivot element from the list of unsorted elements. The list is broken based on pivot position using function partition. After partition, the function will make sure all elements to the left of the pivot are less than or equal to the pivot. Similarly, all elements to the right of the pivot are greater than or equal to the pivot. The quick_sort () function uses recursion to sort the partitions till only one element is left in the list.
The function initially takes the first element from an unsorted list and makes it a pivot by placing it in a location where all elements to its left are less than the pivot and elements to its right are greater than the pivot. This is done using the partition() function. Once that's done, two sublists to the left and right of the pivot are generated. Both are sorted using quick sort recursively until there is only one element left in the sublist. At the end, all individual positions will have been sorted. 
*/
void quick_sort(int * arr, int low, int up)
{
    int pivloc;
    if(low>up)  
    {
        return;   // only one element in the list no need to sublist
    }
    pivloc = partition(arr,low,up);    // Calculate pivot point index using partition function
    quick_sort(arr,low,pivloc -1);     // sort array to left of the pivot point
    quick_sort(arr,pivloc+1,up);       // sort array to the right of the pivot
}

/* Function to do partitionaing of array for quick sorting */
/*
Function partition() will place pivot at the proper place nd then return the location of pivot so that we can form sublists again besed on pivot.Function return th pivot elemnts for merge_sort()
*/
int partition(int * arr, int low, int up)
{
    int temp,i,j,pivot;
    i = low+1;
    j = up;
    pivot = arr[low];
    while(i<=j)
    {
        while((arr[i] < pivot) && (i < up))
            i++;
        while (arr[j] > pivot)
            j--;
        if(i < j)
        {
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;j--;
        }else
        {
            i++;
        }
    }
    arr[low] = arr[j];
    arr[j] = pivot;
    return j;
}