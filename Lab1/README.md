# Lab 1

This lab should familiarize you with fork/join parallelism, barriers, and locks.  

## Summary:
Write, in C or C++, two sorting algorithms. The first algorithm should use fork/join parallelism to parallelize either Quicksort or Mergesort (you may use your code from Lab 0). The second algorithm should use locks to implement BucketSort across multiple sorted data structures (e.g. STL's map). 

As with Lab 0, you are emulating the performance of the UNIX `sort -n` command.  This lab is an individual assignment. Unlike Lab 0, your code for Lab 1 should time itself.  

## Code Requirements / Restrictions

### Fork/Join Quick/Mergesort
Your fork/join algorithm should be an implementation of either Mergesort or Quicksort.  Your parallelization strategy should use fork/join parallelism, implemented using fork, join, and barriers.  Locks should not be necessary. You are allowed to use pre-written unsorted data structures (e.g. STL's vector), and your Lab 0 code, but you may not use any pre-written sorted data structure or sorting algorithm (e.g. STL's map) for this algorithm.

### Locking BucketSort
Your second algorithm should be an implementation of BucketSort using sorted data structures (e.g. STL's map - note that sorted data structures are allowed in this algorithm, but not the other).  BucketSort normally uses unsorted buckets, using sorted buckets makes the task significantly easier - you need not recurse to sort each bucket after the first insertion.  Pseudocode for this algorithm is as follows:

```
function bucketSort(input_array, k) :
  buckets[] : array of k empty sorted lists
  mins[] : array of k+1 minimum values for each list in buckets
  // bucket[m] holds (sorted) values from mins[m] to mins[m+1], so
  // mins[0]=0; mins[k]=INT_MAX;
  for i = 0 to length(input_array) do
    for j = 0 to k do
		if(input_array[i]<mins[j]) // find bucket to insert 
			insert input_array[i] into buckets[j-1]
  return the concatenation of buckets[0], ...., buckets[k-1]
```

Your implementation of Locking BucketSort should avoid idling threads if at all possible. For instance, a problem decomposition that assigns the bucket ranges evenly across threads will result in idle threads when the source distribution is highly skewed e.g. (1,2,3,4,5,6,7,1001,1000,1002).  Instead, you should work to ensure that all threads are busy at all times, even at the cost of synchronizing across buckets.  The `skew` workloads in `autograde_tests` have a highly skewed distribution.

### Lab write-up:
Your lab write-up, called `WRITEUP.md` should include:
* A description of your two parallelization strategies
* A brief discussion of performance - what algorithm is faster? on what cases? why? 
* A brief description of your code organization
* A description of every file submitted
* Any extant bugs

I expect your lab write-up for this project will be around a page or two.

### Code style:
Your code should be readable and commented so that the grader can understand what's going on.

### Submission:
You should submit a link to your final commit to the canvas assignment, following the instructions on the class website.  This mechanism allows us to be sure of exactly what commit you intend to be graded, and when you completed it.  Do not forget this step!

### Compilation and Execution:
Your submission should contain a Makefile and the project should build using a single `make` command.  The generated executable should be called `mysort`.  The `mysort` command should have the following syntax (pay attention, it is slightly
different from Lab 0):

`mysort [--name] [source.txt] [-o out.txt] [-t NUMTHREADS] [--alg=<forkjoin,lkbucket>] `

Using the `--name` option should print your name.  Otherwise, the program should sort the source file.  The source file is a text file with a single integer on each line.  The `mysort` command should then sort all integers in the source file and print them sorted one integer per line to an output file (specified by the `-o` option). The time taken to sort the file (excluding the time for file I/O and to initially launch / finally join threads) should be printed to standard out in nanoseconds (see `text.c` for examples).  The `-t` option specifies the number of threads that should be used, including the master thread, to accomplish the sorting. 

See below for `mysort` syntax examples.  As before, you can assume that all input values are non-negative, less than or equal to `INT_MAX`, and that there are no duplicates.

See the included `exampleC.c` , `exampleCPP.cpp` and `Makefile` for boilerplate thread launching and timing code using the high resolution `CLOCK_MONOTONIC` timer and the pthread and C++ threading interface.  You are welcome to use any/all of this code for building your applications. Note that the example code does not appropriately handle the `mysort` command line syntax. 

### Testing:
We include a `autograde.sh` script and seven test cases, located in `autograde_tests` for you to test your code.  Your submission will be autograded every time you push using the autograder.  To run the autograder on the provided tests, use:

`./autograde.sh`

You can add additional test cases in an additional directory.  You can generate test input files using the UNIX `shuf` (shuffle) command (see below for examples using a bash shell).  Using this input file you can compare your results with `sort` using the `cmp` command, which checks if files are equivalent. To run the autograder on your own directory, use:

`./autograde.sh ./my_own_test_directory`

I _highly_ recommend you test your code using this methodology... it's how we'll grade you!



## Grading:
Your assignment will be graded as follows:
* *Unit tests (70%):*
We will check your code using fourteen randomly generated input files (generated by `shuf` and checked by `cmp`.  Correctly sorting a file is worth five points.  Half of these test cases are in the `autograde_tests` directory.
* *Lab write-up, code readability, and coding interview (30%):* 
Lab write-ups and readable code that meet the requirements will get full marks. Incomplete write-ups or unreadable code will be docked points.

Recall that late submissions will be penalized 5% for every day late, and will not be accepted after the final day of class.

## Examples

Examples of your `mysort` program's syntax
```
### print your name
./mysort --name
# prints:
Your Full Name

### Consider an unsorted file
printf "3\n2\n1\n" > 321.txt
cat 321.txt
# prints
3
2
1

### Sort the text file and print to file
./mysort 321.txt -o out.txt -t5 --alg=forkjoin
# prints time taken in nanoseconds for 5 threads on fork/join sort:
294759

cat out.txt
# prints:
1
2
3
```

Running the boilerplate `example.c` code
```
# build the executable
make example
# prints:
gcc example.c -pthread -O3 -g -o example
# run the executable
./example
# prints:
creating thread 2
creating thread 3
creating thread 4
creating thread 5
Thread 1 reporting for duty
Thread 2 reporting for duty
Thread 3 reporting for duty
Thread 5 reporting for duty
Thread 4 reporting for duty
joined thread 2
joined thread 3
joined thread 4
joined thread 5
Elapsed (ns): 260993
Elapsed (s): 0.000261
```

Several useful shell commands for this assignment:
```
### To generate a random test file ###
# -i1-10 is the range (1 to 10)
# -n5 is the length (chose 5 numbers from the range)
# testcase.txt is the output shuffled file
shuf -i1-10 -n5 > testcase.txt

### To sort a text file with linux ###
# -n is cast each line to integers
# testcase.txt is the shuffled file
# testsoln.txt is the sorted file
sort -n testcase.txt > testsoln.txt

### To compare two text files ###
# e.g. to verify your program's correctness
# Note that line endings matter!
cmp --silent myoutput.txt testsoln.txt && echo "Same!" || echo "Different!"
```


How we will grade your code:
```
### Generate a test file
### (of unspecified range and size)
shuf -i0-2147483643 -n382 > case1.txt

### Sort it using sort
sort -n case1.txt > soln1.txt

### Run your mysort program to also sort the test file
./mysort case1.txt -o your1.txt --alg=merge

### Compare test results
cmp --silent your1.txt soln1.txt && echo "Pass (5pts)" || echo "Fail (0pts)"
```
