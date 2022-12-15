# Lab 2

This lab should familiarize you with building concurrency primitives
and the C/C++ memory model.

## Summary:
In this lab you'll write three locking algorithms (four if you're in a grad section) and one barrier algorithm.   You'll both use these primitives in your BucketSort algorithm from Lab 1 and also in a stand-alone "counter" micro-benchmark.  Your lab write-up will use `perf` to investigate the performance of your code.

The algorithms you'll write are:
* Test-and-set lock
* Test-and-test-and-set lock
* Ticket lock
* MCS Lock (grads only)
* Sense reversal barrier

For up to 15% extra credit, you may also implement Peterson's lock for two threads under both sequential and release consistency.

## Code Requirements / Restrictions
Your primitives should be written using C/C++ atomics and should not rely on external concurrency primitives (e.g. `pthread_mutex_t`). You should apply the code performance lessons you've learned in class --- avoid contention when possible.

### Microbenchmark:
You will write a small stand-alone program, called `counter`. The program increments a counter, using your new primitives.  Either the counter is protected by a lock, or the threads use a barrier to rotate turns incrementing the counter (see below for pseudocode). At program end it will print the time the test took. Your counter program will have options to use all locks/barriers you wrote, plus the pthread ones.  Take note of the performance differences.

```
int cntr=0;
void thread_main_lock(int my_tid){
	for(int i = 0; i<NUM_ITERATIONS; i++){
		lock.lock();
		cntr++;
		lock.unlock();
	}
}
void thread_main_barrier(int my_tid){
	my_tid--; // adjust to zero based tid's
	for(int i = 0; i<NUM_ITERATIONS*NUM_THREADS; i++){
		if(i%NUM_THREADS==my_tid){
			cntr++;
		}
		bar.arrive_and_wait();
	}
}
```

### BucketSort:
You should also use your primitives within your BucketSort code, replacing
all pthreads primitives.  Take note of how the performance changes.

## Lab write-up:
Your lab write-up, , called `WRITEUP.md`, will be longer this time.  In addition to the normal requirements, you should describe how changing the lock and barrier types in the two programs changes performance. In particular, your write-up should include:
* A table of all different locks (for both programs), which includes run time, L1 cache hit rate, branch-prediction hit rate, and page-fault count with four threads.
* The same table for all barriers.
* A discussion explaining why the best and worst primitives have these results.

Example; for markdown syntax see [link](https://markdown.land/markdown-table):
| Primitive    | Runtime (s) | L1 cache hit (%) | Branch Pred Hit Rate (%) | Page Fault Count (#)|
|--------------|-------------|------------------|--------------------------|---------------------|
| Some lock              | 2  | 50 | 50 | 50 |
| Another lock           | 2  | 50 | 50 | 50 |

Your write-up should also include the normal requirements:
* A description of your algorithms and challenges overcome
* A brief discussion of performance - what algorithm is faster? on what cases? why? 
* A brief description of your code organization
* A description of every file submitted
* Any extant bugs

I expect your lab write-up for this project will be longer, around 4 pages.

## Code style:
Your code should be readable and commented so that the grader can understand what's going on.

## Submission:
You should submit a link to your final commit to the canvas assignment, following the instructions on the class website.  This mechanism allows us to be sure of exactly what commit you intend to be graded, and when you completed it.  Do not forget this step!


## Compilation and Execution:
Your submission should contain a Makefile and the project should build using a single `make` command.  Your makefile
will generate two executables.

### Counter

`counter [--name] [-t NUM_THREADS] [-i NUM_ITERATIONS] [--bar=<sense,pthread>] [--lock=<tas,ttas,ticket,mcs,pthread,petersonseq,petersonrel>] [-o out.txt]`

The program launches `NUM_THREADS` and each increments the counter `NUM_ITERATIONS` times.  The counter is synchronized using either the `bar` or `lock` argument (setting both is an invalid input).  The time taken to increment the counter to its total (excluding the time for file I/O and to initially launch / finally join threads) should be printed to standard out in nanoseconds, and the final counter value should be written to the output file designated by the `-o` flag. See below for `counter` syntax examples. The `pthread` argument refers to the default primitive in your language (e.g `pthread_mutex_t` or `mutex`).

### BucketSort
As before, the generated sorting executable should be called `mysort`.  The `mysort` command should have the following syntax (it is slightly different from Lab 1 as it adds two arguments, and drops the algorithm selection):

`mysort [--name] [source.txt] [-o out.txt] [-t NUM_THREADS] [--bar=<sense,pthread>] [--lock=<tas,ttas,ticket,mcs,pthread,petersonseq,petersonrel>]`

The new `bar` and `lock` arguments that chose which barrier and lock to use respectively when executing the algorithm (all locks should use the indicated lock and all barriers should use the indicated barrier). If this argument is excluded, you should use the default pthread primitives.  Using the `--name` option should print your name.  Otherwise, the program should sort the source file.  The `mysort` command should then sort all integers in the source file and print them sorted one integer per line to an output file (specified by the `-o` option). The time taken to sort the file (excluding the time for file I/O and to initially launch / finally join threads) should be printed to standard out in nanoseconds.  The `-t` option specifies the number of threads that should be used, including the master thread, to accomplish the sorting.   The `getopt` and `getopt_long` method calls are helpful for parsing the command line.

See the included `exampleC.c` , `exampleCPP.cpp` and `Makefile` for boilerplate thread launching and timing code using the high resolution `CLOCK_MONOTONIC` timer and the pthread and C++ threading interface.  You are welcome to use any/all of this code for building your applications. Note that the example code does not appropriately handle the `mysort` command line syntax. 

## Testing:
We include a `autograde.sh` script and several test cases, located in `autograde_tests` for you to test your code.  Your submission will be autograded every time you push using the autograder.  To run the autograder on the provided tests, use:

`./autograde.sh`

You can add additional test cases in an additional directory.  

For counting, the test file must end in a `.cnt` suffix and contain the number of iterations you want to run.

For sorting, you can generate test input files using the UNIX `shuf` (shuffle) command.  Using this input file you can compare your results with `sort` using the `cmp` command, which checks if files are equivalent. To run the autograder on your own directory, use:

`./autograde.sh ./my_own_test_directory`

I _highly_ recommend you test your code using this methodology... it's how we'll grade you!



## Grading:
Your assignment will be graded as follows:

* *Sort unit tests (40%)*
We will check your code using ten randomly generated
input files and using combinations of locks
and barriers. Correctly sorting a file is worth four points.
Half of these tests are included in the `./autograde_tests` directory.
* *Counter tests (10%)*
We will check your code for different locks
and barriers, verifying that counter is correctly
incremented. Half of these tests are included in the `./autograde_tests` directory. 
* *Lab write-up, code readability, and coding interview (50%):* 
Lab write-ups that meet the requirements and demonstrate understanding of the code's performance will be given full points. Incomplete experimentation and analysis will be docked points.

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
./mysort 321.txt -o out.txt -t5 --lock=tas
# prints time taken in nanoseconds for 5 threads:
294759

cat out.txt
# prints:
1
2
3
```

Examples of your `counter` program's syntax
```
### print your name
./counter --name
# prints:
Your Full Name

### Increment the counter
./counter -o out.txt -t5 -i10000 --lock=tas 
# prints time taken in seconds for 5 threads on tas lock,
# to increment 10000 times each and outputs final counter value to out.txt
294759

cat out.txt
# prints:
50000
```
