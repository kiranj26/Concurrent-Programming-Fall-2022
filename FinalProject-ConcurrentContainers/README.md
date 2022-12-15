# Final Project

In this project you will implement one or a few parallel
algorithms (you have a choice) and analyze their performance.

## Summary

You have the choice to implement a selection of the following
parallel algorithms, each worth a set of points.
Overall, the final project is worth 250 points.
You can implement up to 350 points for 100 points extra credit.
Your options are:

### Concurrent Ordered Map (250 pts)
Implement a key-value store implemented as either a sorted tree or skip list.  Your structure should use fine-grained synchronization (no global lock, the recommendation is to use hand-over-hand locking), and be linearizable. The structure should support `get`, `put`, and `delete`, as well range queries that take two keys and return the key/value pairs between them (range queries need not be linearizable, but you should be able to describe what values might be seen).  Your data structure should not leak memory.  Your write-up should include experimental explorations across high contention (many threads accessing the same key) and low contention (uniform access pattern) cases.  *For an additional 50 pts*, implement the same tree using reader-writer locks such that readers can execute in parallel, and compare the performance.

### Concurrent Containers (250 pts)
Implement several concurrent stack and queue algorithms, including a SGL stack and queue, the Treiber stack, and the M&S queue. You must also implement one of the following (or, *for an additional 50 points*, both): (1) all the above stacks with an [elimination](https://people.csail.mit.edu/shanir/publications/Lock_Free.pdf), (2) a [flat-combining](http://mcg.cs.tau.ac.il/papers/spaa2010-fc.pdf) stack and queue. Your implementations are allowed to leak memory. Your write-up should include experimental explorations of data structure throughput across varying thread counts.

### LIFO locks (extra credit only, 50 pts)
Using your Lab 2 counter micro-benchmark, implement a strictly LIFO lock --- the recommendation is to combine the MCS lock with a stack.  Analyze its performance relative to TAS and FIFO locks across thread count.  


## Lab write-up:
Your lab write-up, called `WRITEUP.md` should include:
* Experimental results as required by the prompt
* Analysis of results using `perf` as necessary to support explanations
* A description of your code organization
* A description of every file submitted
* Compilation instructions
* Execution instructions, particulary for any results presented in the write-up
* Any extant bugs

## Compilation and Execution:
Your submission should contain a Makefile and the project should build using a single `make` command.  Executables generated should provide execution instructions when given a `-h` flag.

## Code Requirements:
This is an open project --- you are welcome to use whatever supplemental libraries you would like.  However, you need to entirely implement the main algorithm(s).  Your code should be readable and commented so that the grader can understand what's going on.

## Submission:
You should submit a link to your final commit to the canvas assignment, following the instructions on the class website.  This mechanism allows us to be sure of exactly what commit you intend to be graded, and when you completed it.  Do not forget this step!

## Grading
Your assignment will be graded as follows:
* *Implementation (70%):*
Your code should work and meet the project criteria. Incomplete/failing code will be docked points.  Your submission should include sufficient unit test cases that we can verify your code is correct.
* *Lab write-up and interview (30\%):*
Lab write-ups and interviews that meet the requirements will get full marks. Incomplete write-ups or unreadable code will be docked points.
