# Final Project ECEN 5313
# Concurrent Containers and LIFO Lock

**Student Name**     : Kiran Narendra Jojare  
**Section**          : Graduate Student in ECEN  
**Final Project**    : Concurrent Containers for SGL Stack, SGL Queue, M&S Queue and Trieber Stack


*Note             : For extra credit both elimination and flat combining optamisation algorithm is used in the code. Also note the counter with LIFO lock is also implemented for extra credit.*

## **Summary**



This project revolves around implementing several concurrent stacks and queue algorithms.Project  implement different concurrent stack & queue using elimination & flat combining methodes

Usage : 
```
$./finalproject -t THREAD_COUNT -i ITRATIONS --DS=SGLStack/SGLQueue/MSQ/Treiber
```
I created and optimized my own concurrent data structures for this project. A Treiber Stack, Michael and Scott Queue, Single Global Lock Stack, and Single Global Lock Queue made up the entire set of four data structures. Epoch-based reclamation was used to ensure that none of the data structures leak memory. An elimination algorithm and flat combining were used to optimize the lock algorithms and stack data structures, respectively.

While the Single Global Lock Stack and Single Global Lock Queue are blocking algorithms, the Treiebr stack and M&S queue are both non-blocking. The non-blocking algorithms linearize the algorithms across threads by using atomic operations, more specifically compare and swap operations. The blocking algorithms merely mimic the conventional stack and queue algorithms, but in order to prevent data races, the algorithms are secured by a lock.

The project also uses Lab 2 counter micro benchmark to test and evaluate a performance of strictly LIFO lock and compare the performance with TAS and FIFO. 

Usage : 
```
$./counter      -t THREAD_COUNT -i ITRATIONS --lock=tas/mcs/LIFO
```

## **Testing Concurrent Data Structures** 

All the observations for both concurrent containers as well as for counter benchmarks is located in an excel file named `Concurrrent Final Project TestData.xlsx` avaiable inside repository.

### Performance (Experimental Results)
When testing the performance of each data structure compared to each other, I tested the structures with 10,000 operations with varying thread counts (5,000 pushes/queues and 5,000 pops/dequeues). I ran each data structure 5 times with 5, 10, and 25 threads. 

Below table shows the detailed results aftre each iterations for respective data structure. 

|Threads = 5 Iterations = 10000   | | | | | | | | |
|:----|:----|:----|:----|:----|:----|:----|:----|:----|
|Iterations|SGL Stack| | |SGL Queue| |MS Queue| |Trieber Stack|
| |Normal|With Elimination|With Flat Combining|Normal|With Flat Combining|With Flat Combining|No Elimination|With Elimination|
|1|18010863|5655853|175264417|18222572|62534316|3497680857|3228721981|3128721981|
|2|18261785|6070977|176633887|17920125|70206744|3436053088|3250827280|3150827280|
|3|18849993|5433491|135008318|16241497|102902381|3427330467|3160215122|3060215122|
|4|18524522|5728644|140618930|19172589|109007745|3515860880|3208905324|3108905324|
|5|19336935|5943641|80113864|16916874|121477671|3505124625|3247109002|3147109002|
|Total|92984098|28832606|707639416|88473657|466128857|17382049917|16095778709|15595778709|
|Average(ns)|18596819.6|5766521.2|141527883.2|17694731.4|93225771.4|3476409983|3219155742|3119155742|
|Average(sec)|0.0185968196 Sec|0.0057665212 Sec|0.1415278832 Sec|0.017694731 Sec|0.093225771 Sec|3.476409983 Sec|3.219155742 Sec|3.119155742 Sec|
| | | | | | | | | |
| | | | | | | | | |
|Threads = 10 Iterations = 10000   | | | | | | | | |
|Iterations|SGL Stack| | |SGL Queue| |MS Queue| |Trieber Stack|
| |Normal|With Elimination|With Flat Combining|Normal|Flat Combining|Flat Combining|No Elimination|With Elimination|
|1|42179485|15969306|1203602105|136400002|731427379|7544294223|6985276581|6885276581|
|2|134440841|15146498|1347172954|208795953|1354425964|7765414489|6711223593|6611223593|
|3|203035358|14073813|1171762592|110578412|1748182884|7729689030|6792899509|6692899509|
|4|178533056|16146683|1210428773|166947811|593275986|7552768682|6968921740|6868921740|
|5|126163008|14194299|1301014928|175905204|1177279948|7860465129|6769712700|6669712700|
|Total|684351748|75530599|6233981352|798627382|5604592161|38452631553|34228034123|33728034123|
|Average(ns)|136870349.6|15106119.8|1246796270|159725476.4|1120918432|7690526311|6845606825|6745606825|
|Average(sec)|0.1368703496 Sec|0.0151061198 Sec|1.24679627 Sec|0.159725476 Sec|1.120918432 Sec|7.690526311 Sec|6.845606825 Sec|6.745606825 Sec|
| | | | | | | | | |
| | | | | | | | | |
|Threads = 25 Iterations = 10000   | | | | | | | | |
|Iterations|SGL Stack| | |SGL Queue| |MS Queue|Trieber Stack| |
| |Normal|With Elimination|With Flat Combining|Normal|Flat Combining|Flat Combining|No Elimination|With Elimination|
|1|723070477|371833673|12081516179|1324197723|14447087846|23905510558|21110680980|21010680980|
|2|662500540|510881851|11633912166|899976504|13356778218|23761048795|20964691312|20864691312|
|3|698838661|340958683|22400143832|1950018283|12730946594|23645995611|21304883805|21204883805|
|4|838608996|336093310|17168157635|900360648|8716577859|23457520839|20975328186|20875328186|
|5|1057033363|304708386|9718131368|959834369|12446794906|23462586769|21095874499|20995874499|
|Total|3980052037|1864475903|73001861180|6034387527|61698185423|1.18233E+11|1.05451E+11|1.04951E+11|
|Average(ns)|796010407.4|372895180.6|14600372236|1206877505|12339637085|23646532514|21090291756|20990291756|
|Average(sec)|3.980052037 Sec|0.372895181 Sec|14.600372236 Sec|1.206877505 Sec|12.33963708 Sec|23.64653251 Sec|21.09029176 Sec|20.99029176 Sec|



### Perf Analysis  
The per analysis for the concurrent data structures explained and noted down with observed values as follows: 

|Data Structure|L1 Cache Hit Rate | | | |
|:----|:----|:----|:----|:----|
| | | | | |
| |Optamisation|5 Threads|10 Threads|25 Threads|
|SGL Stack|No Optamisation|89.9|89.91|89.9|
| |Elimination|90.01|90.23|90.24|
| |Flat Combining|91.26|91.67|91.82|
|SGL Queue|No Optamisation|98.23|98.24|98.23|
| |Flat Combining|99.01|99.01|99.01|
|M&S Queue|Flat Combining|98.11|98.11|98.11|
|Treiber Stack|No Elimiation|98.11|98.1|98.11|
| |Flat Combining|98.11|98.1|98.11|

  

|Data Structure|Branch Pred Hit Rate (%)| | | |
|:----|:----|:----|:----|:----|
| | | | | |
| |Optamisation|5 Threads|10 Threads|25 Threads|
|SGL Stack|No Optamisation|96.75%|96.75%|96.75%|
| |Elimination|94.95%|94.99%|94.95%|
| |Flat Combining|92.73%|92.73%|92.73%|
|SGL Queue|No Optamisation|89.40%|89.40%|89.40%|
| |Flat Combining|91.03%|91.03%|91.03%|
|M&S Queue|Flat Combining|92.10%|93.33%|94.56%|
|Treiber Stack|No Elimiation|92.17%|92.18%|92.18%|
| |Elimination|92.40%|92.50%|92.50%|
 

|Data Structure|Page Fault Count (#)| | | |
|:----|:----|:----|:----|:----|
| | | | | |
| |Optamisation|5 Threads|10 Threads|25 Threads|
|SGL Stack|No Optamisation|150|150|150|
| |Elimination|157|157|157|
| |Flat Combining|159|159|159|
|SGL Queue|No Optamisation|159|159|159|
| |Flat Combining|158|158|158|
|M&S Queue|Flat Combining|159|159|159|
|Treiber Stack|No Elimiation|157|157|157|
| |Flat Combining|159|159|159|



The Treiber stack with the elimination optimization appears to have been the best-performing data structure overall after the data was gathered. Only under conditions of low thread density did the single global lock data structures outperform the Treiber stack with elimination. Due to the way I implemented garbage collection, I think this is the case. The non-locking algorithms used epoch-based reclamation for garbage collection, but the locking algorithms collected garbage more effectively because deletes were only executed after the lock was held. However, as the number of threads increased, the garbage collection efficiency decreased.

The performance of the locking algorithms significantly decreased as the number of threads rose because the lock became more congested. The data also show that each optimization enhanced the performance of its corresponding non-optimized counterpart. Overall, it is evident that as the number of threads increased, the non-blocking algorithms outperformed the locking algorithms. My flat combining optimization outperformed my elimination optimization as well. Given that flat combining had a higher cache hit rate, this makes sense.

## **Testing LIFO with Counter Benchmark** 



### Experimental Results And Perf Analysis

The result and analysis of running the counter benchmark with TAS, FIFO and LIFO locks is as follows. 

Locks Performance Using Counter BenchMark
Thread = 5 Iterations = 100000		

|Data Structure|Locks Performance Using Counter BenchMark| | |
|:----|:----|:----|:----|
| |Thread = 10 Iterations = 100000| | |
| |TAS|FIFO|LIFO|
|Time Consumed|0.288614 Sec|0.597675 Sec|0.120026 Sec|
|L1 Cache Hit Rate |98.50%|99.60%|97.10%|
|Branch Prediction Rate|97.50%|98.90%|97.02%|
|Page Fault Count (#)|123|122|124|



Locks Performance Using Counter BenchMark
Thread = 10 Iterations = 100000	

|Data Structure|Locks Performance Using Counter BenchMark| | |
|:----|:----|:----|:----|
| |Thread = 5 Iterations = 100000| | |
| |TAS|FIFO|LIFO|
|Time Consumed|1.01425 Sec|1.67415 Sec|1.203167 Sec|
|L1 Cache Hit Rate |98.49%|99.57%|99.70%|
|Branch Prediction Rate|96.32%|98.99%|99.20%|
|Page Fault Count (#)|123|122|124|


Above observations clearly shows that LIFO locks are more effective compared to FIFO locks. But they are not as efficient as TAS locks. 


## **Code Organisation**

### Concurrency Containers
The repository contains main driver code named `main.cpp` and `main.hpp` which loads the command line arguments using `getoptlong()`.  

Once command line arguments are evaluated, tester file `MyTester.cpp` & `MyTester.hpp` will test and invoke each concurrent data structure. Comtains individual functions for testing each concurrent data structure. 

The respective files which are built for concurrent data structures is as follows :   
* SGL Stack : `SGL_stack.cpp` & `SGL_stack.hpp`  
					Containes required classes and function declarations to implement SGL Stack using 
					Flat Combining and Elimination based Optamisation 
* SGL Queue : `SGL_queue.cpp` & `SGL_queue.hpp`
					Containes required classes and function declarations to implement SGL Stack using 
					Flat Combining Optamisation   
* Trieber Stack : `Treiber_stack.cpp` & `Treiber_stack.hpp`
					Containes required classes and function declarations to implement SGL Stack using 
					Flat Combining and Elimination based Optamisation   
* M&S Queue:  `MS_queue.cpp` & `MS_queue.hpp`
					Containes required classes and function declarations to implement SGL Stack using 
					Flat Combining Optamisation   
	
		
For each individual data structure seperate `Locks.c & Locks.h` and 1`Barrier.c & Barrier.h` file are used. These file loads the required concurrency premetive required and initialises the concurrent variables to use tas/ttas/ticket lock and barrier sense/pthread locks.

To test the files a auto test script is generated named `auto_test_concurrent_DS_exc.sh`. This script runs the output file of concurrent data structures named `finalproject` with three different scenrios involving running concurrent data structures with 5 , 10 and 20 threads each with 10000 iterations. 

### Counter Benchmark
The repository contains a c file named `counter.c` which processes the command line arguments and invokes respective thread functions for different locks such as TAS, MCS(FIFO) and stack based LIFO lock. 

The final generated opbject file for the counter benchmark is `counter`. The same script `auto_test_concurrent_DS_exc.sh` provides some execution test scenario where we are running counter with TAS, FIFO and LIFO premetives using 2 different scenraios. One with 5 Threads and 10000 Iterations and other being 10 threads and 10000 iterations.  

## **Files Submitted**



* main.cpp main.hpp  
The main command line parsing function which invokes a concurrent data structure testing methode

* MyTester.cpp Mytester.hpp  
The main testing file for testing each concurrent data structure in detail.

* Barriers.cpp barriers.hpp  
The file which process which type of barrier between barrier sense reversal and pthread barrier needs to be used. Peforms necesary initialisation to use thses barriers inside data structures

* Locks.cpp Locks.hpp  
The file which process which type of lock between tas/ttas/ticket/pthread needs to be used. Peforms necesary initialisation to use thses locking premetives inside data structures

* SGL_stack.cpp SGL_Stack.hpp  
These files initialises and implements all the Single Global Lock based Stacking. Functioon pops and pushes value for 0 to ITERATIONS. ITERATION/2 Pushes followed by ITERATIONS/2 Pops. Function provides user option to use either eminiation or flat combining methode.

* SGL_queue.cpp SGL_queue.hpp  
These files initialises and implements all the Single Global Lock based Queue. Functioon equeues and dequeues value for 0 to ITERATIONS. ITERATION/2 Pushes followed by ITERATIONS/2 Pops. Function provides user option to use either no optimisation or flat combining methode.

* MS_queue.cpp MS_queue.hpp  
These files initialises and implements all the M&S based Queue Algorithm wih flat combining optamisation.

* Treiber.cpp Treiber.hpp  
These files initialises and implements treiber stack algorithm using Elimination based optamisation.

* counter.c  
These file helps to implement and evaluate a FIFO based lock. Lab 2's code has been reused here.

## **Compilation and Execution**



### Compilation 

#### Concurrent Containers  


The code can be compiles using single `make` which creates a executable named `finalproject`. The usage to compile final executable is as follows: 
```
`$./finalproject -t THREAD_COUNT -i ITRATIONS --DS==SGLStack/SGLQueue/MSQ/Treiber`  
```  

For example,  

```
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLStack 
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLQueue 
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=MSQ 
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=Treiber 

```

#### Strictly FIFO Lock

The code can be compiles using single `make` which creates a executable named `counter`. The usage to compile final executable is as follows:   

```
$./counter      -t THREAD_COUNT -i ITRATIONS --lock=tas/mcs/LIFO
```

For example,  

```
./counter -t 5 -i 100000 --lock=tas -o dest.txt
./counter -t 5 -i 100000 --lock=mcs -o dest.txt
./counter -t 5 -i 100000 --lock=LIFO -o dest.txt

```


### Execution 

#### Concurrent Containers

The files `auto_test_concurrent_DS_exc.sh` provides a shell script to run the program with different scenarios. The output after running auto test script is like as follows: 

```
---Running With Concurrent Data Structure---

Running With Scenrio 1

No of threads selected : 5
No of iterations selected : 100000
Concurrent data structure Selected : SGLStack
Elapsed (ns): 5722945
Elapsed (s): 0.005723
No of threads selected : 5
No of iterations selected : 100000
Concurrent data structure Selected : SGLQueue
Elapsed (ns): 124707060
Elapsed (s): 0.124707
No of threads selected : 5
No of iterations selected : 100000
Concurrent data structure Selected : MSQ
Elapsed (ns): 3591266111
Elapsed (s): 3.591266
No of threads selected : 5
No of iterations selected : 100000
Concurrent data structure Selected : Treiber
Elapsed (ns): 3300638935
Elapsed (s): 3.300639

Running With Scenrio 2

No of threads selected : 10
No of iterations selected : 100000
Concurrent data structure Selected : SGLStack
Elapsed (ns): 71346481
Elapsed (s): 0.071346
No of threads selected : 10
No of iterations selected : 100000
Concurrent data structure Selected : SGLQueue
Elapsed (ns): 1104689735
Elapsed (s): 1.104690
No of threads selected : 10
No of iterations selected : 100000
Concurrent data structure Selected : MSQ
Elapsed (ns): 7805088662
Elapsed (s): 7.805089
No of threads selected : 10
No of iterations selected : 100000
Concurrent data structure Selected : Treiber
Elapsed (ns): 7195258617
Elapsed (s): 7.195259

Running With Scenrio 3

No of threads selected : 20
No of iterations selected : 100000
Concurrent data structure Selected : SGLStack
Elapsed (ns): 286730885
Elapsed (s): 0.286731
No of threads selected : 20
No of iterations selected : 100000
Concurrent data structure Selected : SGLQueue
Elapsed (ns): 10387142596
Elapsed (s): 10.387143
No of threads selected : 20
No of iterations selected : 100000
Concurrent data structure Selected : MSQ
Elapsed (ns): 17392711055
Elapsed (s): 17.392711
No of threads selected : 20
No of iterations selected : 100000
Concurrent data structure Selected : Treiber
Elapsed (ns): 15888337159
Elapsed (s): 15.888337

jovyan@jupyter-kijo7257:~/Concurrent_Prog/Final Project 2/finalproject-kiranj26$


```


#### Strict FIFO Lock

The files `auto_test_concurrent_DS_exc.sh` provides a shell script to run the program with different scenarios. The output after running auto test script is like as follows: 

```

--------Running For Counter--------------- 

--------Running For Scenario 1--------------- 


Running For Counter With TAS 

No of threads Selected : 5
No of iterations Selected : 100000
Lock Selected : tas
Destination file name : dest.txt
Final lock Count :: 500000 :: 
Elapsed (ns): 299596051
Elapsed (s): 0.299596

Running For Counter With FIFO 

No of threads Selected : 5
No of iterations Selected : 100000
Lock Selected : mcs
Destination file name : dest.txt
Final lock Count :: 500000 :: 
Elapsed (ns): 325094423
Elapsed (s): 0.325094

Running For Counter With LIFO 

No of threads Selected : 5
No of iterations Selected : 100000
Lock Selected : LIFO
Destination file name : dest.txt
Final lock Count :: 500000 :: 
Elapsed (ns): 578420972
Elapsed (s): 0.578421

--------Running For Scenario 2--------------- 


Running For Counter With TAS 

No of threads Selected : 10
No of iterations Selected : 100000
Lock Selected : tas
Destination file name : dest.txt
Final lock Count :: 1000000 :: 
Elapsed (ns): 1090627612
Elapsed (s): 1.090628

Running For Counter With FIFO 

No of threads Selected : 10
No of iterations Selected : 100000
Lock Selected : mcs
Destination file name : dest.txt
Final lock Count :: 1000000 :: 
Elapsed (ns): 2182879055
Elapsed (s): 2.182879

Running For Counter With LIFO 

No of threads Selected : 10
No of iterations Selected : 100000
Lock Selected : LIFO
Destination file name : dest.txt
Final lock Count :: 1000000 :: 
Elapsed (ns): 3091989931
Elapsed (s): 3.091990
jovyan@jupyter-kijo7257:~/Concurrent_Prog/Final Project 2/finalproject-kiranj26$ 
```

## Any extent Bugs



No extent bug to be reported. Code works fine for all the test cases given as per assignment requirements.





