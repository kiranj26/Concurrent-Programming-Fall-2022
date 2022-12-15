/**********************************************************/
/**********************************************************/
/*     Final Project  : Concurrent Programming            */
/*     Author         : Kiran Jojare                      */
/**********************************************************/
/**********************************************************/

#include "MyTester.hpp"

struct timespec start, finish;

/* 
 * Constructor for the Concurrent_DS_Tester Class
 */
Concurrent_DS_Tester::Concurrent_DS_Tester(int thread_count) {
	/* Allocates Memory To Thread Pointer to create Thread Array */
    threads = (pthread_t*)(malloc(thread_count*sizeof(pthread_t)));
}

/*
 * Destructor for the Concurrent_DS_Tester class
 */
Concurrent_DS_Tester::~Concurrent_DS_Tester() {
	/* deallocate dynamic memory */
    free(threads);
}

/* Function : extermination_treiber 
 * Garbage collection method for Triber Stack
 * finds the smallest epoch at which nodes can be released, then searches the list of retired nodes 
 * to release the appropriate nodes. Each thread cleans up their own garbage.
 */
static void extermination_treiber(multimap<int,treiberstack_node*> &retired_nodes, atomic<int>* reservations, int number_of_threads) {

    int min = INT_MAX; 
    for(int i = 0; i < number_of_threads; i++) {
        if(min > reservations[i].load(ACQ)) {
            min = reservations[i].load(ACQ);
        }
    }
    
    auto itr = retired_nodes.begin();
    itr++;

    for(itr; itr != retired_nodes.end(); itr++) {
        if(min > itr->first) {
            auto itcopy = itr;
            itcopy--;
            delete(itr->second);
            retired_nodes.erase(itr);
            itr = itcopy;
        }
    }
}


/* Function : extermination_ms 
 * Garbage collection method for M&S Queue
 * finds the smallest epoch at which nodes can be released, then searches the list of retired nodes 
 * to release the appropriate nodes. Each thread cleans up their own garbage.
 */
static void extermination_ms(multimap<int,msqueue_node*> retired_nodes, atomic<int>* reservations, int number_of_threads) {

    int min = INT_MAX; 

    for(int i = 0; i < number_of_threads; i++) {
        if(min > reservations[i].load(ACQ)) {
            min = reservations[i].load(ACQ);
        }
    }

    for(auto itr = retired_nodes.begin(); itr != retired_nodes.end(); itr++) {
        if(min > itr->first) {
            auto itcopy = itr;
            itcopy--;
            delete(itr->second);
            retired_nodes.erase(itr);
            itr = itcopy;
        }
    }
}

/* Function ot invoke parallism inside treiber stack
 * For the specified number of iterations, pushes numbers from a counter. After that, pops immediately for the
 * specified number of iterations. For garbage collection, this method uses epoch-based reclamation. In this 
 * methodology, there are two counters. One is in charge of when trash is collected, and the other is in charge 
 * of when the global epoch is raised. This method will be carried out individually by each thread.
 */
static void* invoke_Treiber(void* args){

	// Extracting arguments from the passed struct
	DS_Thread_args*     Args      = (DS_Thread_args*)args;
	Locks*              lock      = Args->lock;
    Barriers*           bar       = Args->barrier;
    T_stack*            t_stack   = Args->t_stack;
    SGL_stack*          s_stack   = Args->s_stack;
    MS_queue*           m_queue   = Args->m_queue;
    SGL_queue*          s_queue   = Args->s_queue;
    atomic<int>*        epoch     = Args->epoch;
    atomic<int>*        reservations = Args->reservations;
	int                 tid       = Args->tid;
	int                 number_of_threads = Args->number_of_threads;
    int                 iterations = Args->iterations;

    int value                      = tid;
    int garbage_genocide_countdown = 500;
    int time_shift_countdown       = 100;
	
	thread_local multimap<int,treiberstack_node*> retired_nodes; 


	bar->wait();
    
	/* Start Timer */
	if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&start);
    }
    
	/* Push value int trieber stack till no of iterations */
    for(int i = 0; i < iterations; i ++) {
        t_stack->push(value);
        value += 1;
    }

    while(true){
        int time = epoch[0].load(ACQ);
        reservations[tid].store(time,ACQREL);

        treiberstack_node* to_retire = t_stack->pop();

		/* Making sure value is not the end signal */
        if(to_retire->val != -1) {
            bool flag = false;
            for(auto itr = retired_nodes.begin(); itr != retired_nodes.end(); itr++) {
                if((treiberstack_node*)itr->second == to_retire) {
                    flag = true;
                    break;   
                } 
            }
            if(flag == false) {
                retired_nodes.insert({time, to_retire});
            }  /* End of If */
        } /* End of If */

        garbage_genocide_countdown += -1;
        time_shift_countdown += -1;
        
		/* Perform Garbage Collection */
        if(garbage_genocide_countdown == 0 || to_retire->val == -1) {
            if(retired_nodes.size() > 0) {
                extermination_treiber(retired_nodes, reservations, number_of_threads);
            }
            garbage_genocide_countdown = 500;
            if(to_retire->val == -1) {
                delete(to_retire);
                break;
            }
        }
        
		/* Increment global epoch when time shift countdown is zero */
        if(tid == 0 && time_shift_countdown == 0) {
            
            epoch[0].store(time + 1, ACQREL);
            time_shift_countdown = 100;
        }
    } /* End of while loop */
    
    
	/* Free Data for all retired nodes */
    while(retired_nodes.begin() != retired_nodes.end()) {
        auto itr = retired_nodes.begin();
        delete(itr->second);
        retired_nodes.erase(itr); 
    } /* End of while loop */
    
	bar->wait();

	/* Stop Clock*/
	
	if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&finish);
    }

}

 
/* Function to invoke parallism inside M&S Queue
 * Numbers from a counter are enqueued for the specified number of iterations. then immediately releases the
 * queue after the specified number of iterations. For garbage collection, this method uses epoch-based 
 * reclamation. In this methodology, there are two counters. One is in charge of when trash is collected, and 
 * the other is in charge of when the global epoch is raised. This method will be carried out independently by
 * each thread. To determine when all pops have occurred, look for an end signal of -1.
 */
static void* invoke_MSQueue(void* args){

	// Extracting arguments from the passed struct
	DS_Thread_args* Args           = (DS_Thread_args*)args;
	Locks*          lock           = Args->lock;
    Barriers*       bar            = Args->barrier;
    T_stack*        t_stack        = Args->t_stack;
    SGL_stack*      s_stack        = Args->s_stack;
    MS_queue*       m_queue        = Args->m_queue;
    SGL_queue*      s_queue        = Args->s_queue;
    atomic<int>*    epoch          = Args->epoch;
    atomic<int>*    reservations   = Args->reservations;
	int tid                        = Args->tid;
	int number_of_threads          = Args->number_of_threads;
    int iterations                 = Args->iterations;

    int value                      = tid;
    int garbage_genocide_countdown = 500;
    int time_shift_countdown       = 100;
	
    thread_local multimap<int,msqueue_node*> retired_nodes; 

	bar->wait();
    
	/* Start Timer */
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&start);
    }
    
	/* Enqueue elements from queue */
    for(int i = 0; i < iterations; i ++) {
    
        m_queue->enqueue(value);
        value += 1;
    }
	
    while(true){

        int time = epoch[0].load(ACQ);
        reservations[tid].store(time,ACQREL);

        msqueue_node* to_retire = m_queue->dequeue();

		/* Making sure value is not the end signal */
        if(to_retire->val != -1) {

            bool flag = false;
            for(auto itr = retired_nodes.begin(); itr != retired_nodes.end(); itr++) {
                if((msqueue_node*)itr->second == to_retire) {
                    flag = true;
                    break;   
                }   
            }
            if(flag == false) {
                retired_nodes.insert({time, to_retire});  
            } 
        }

        garbage_genocide_countdown += -1;
        time_shift_countdown += -1;
        
		/* Perform Garbage Collection */
        if(garbage_genocide_countdown == 0 || to_retire->val == -1) {

            if(retired_nodes.size() > 0) {
                extermination_ms(retired_nodes, reservations, number_of_threads);
            }
            garbage_genocide_countdown = 500;
            if(to_retire->val == -1) {
                break;
            }
        }
        
		/* Increment global epoch when time shift countdown is zero */
        if(tid == 0 && time_shift_countdown == 0) {
            epoch[0].store(time + 1, ACQREL);
            time_shift_countdown = 100;
        }
    } /* End of while loop */
    
	/* Free Data for all retired nodes */
    while(retired_nodes.begin() != retired_nodes.end()) {
        auto itr = retired_nodes.begin();
        delete(itr->second);
        retired_nodes.erase(itr);
    }
	
	bar->wait();

	/* End Timer */
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&finish);
    }
}

 
/* Funcion to invoke paralllelsim inside SGL Stack
 * Pushes numbers for the specified number of iterations from a counter. After that, 
 * pops immediately for the specified number of iterations. This method will be carried 
 * out independently by each thread. To determine when all pops have occurred, look for an end signal of -1.
 */
static void* invoke_SGLStack(void* args){

	// Extracting arguments from the passed struct
	DS_Thread_args* Args           = (DS_Thread_args*)args;
	Locks*          lock           = Args->lock;
    Barriers*       barrier        = Args->barrier;
    T_stack* t_stack             = Args->t_stack;
    SGL_stack*      s_stack        = Args->s_stack;
    MS_queue* m_queue            = Args->m_queue;
    SGL_queue* s_queue             = Args->s_queue;
    atomic<int>*    epoch          = Args->epoch;
    atomic<int>*    reservations   = Args->reservations;
	
	int tid                        = Args->tid;
	int number_of_threads          = Args->number_of_threads;
    int iterations                 = Args->iterations;
	
    int value                      = tid;
    int garbage_genocide_countdown = 500;
    int time_shift_countdown       = 100;
	
    thread_local multimap<int,stacknode*> retired_nodes; 
	
	barrier->wait();
	
    /* start clock */
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&start);
    }
	
	/* Push items till no of iterations */
    for(int i = 0; i < iterations; i ++) {
        s_stack->push(value, lock, tid);
        value += 1;
    }
	/* Pop Continuesly */
    while(s_stack->pop(lock, tid) != -1) {
		
	}
    
	barrier->wait();

    /* end clock */
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&finish);
	}
}

/* Function to onvoke paralleism for SGL Queue
 * Numbers from a counter are enqueued for the specified number of iterations. 
 * then immediately releases the queue after the specified number of iterations.
 * This method will be carried out independently by each thread. To determine 
 * when all pops have occurred, look for an end signal of -1.
 */
static void* invoke_SGLQueue(void* args){
	
	// Extracting arguments from the passed struct
	DS_Thread_args* Args = (DS_Thread_args*)args;
	Locks* lock            = Args->lock;
    Barriers* bar          = Args->barrier;
    T_stack* t_stack       = Args->t_stack;
    SGL_stack* s_stack     = Args->s_stack;
    MS_queue* m_queue      = Args->m_queue;
    SGL_queue* s_queue     = Args->s_queue;
    atomic<int>* epoch     = Args->epoch;
    atomic<int>* reservations = Args->reservations;
	int tid                = Args->tid;
	int number_of_threads  = Args->number_of_threads;
    int iterations         = Args->iterations;

    thread_local multimap<int,queue_node*> retired_nodes; 
    int value = tid;
    int garbage_genocide_countdown = 500;
    int time_shift_countdown = 100;

	bar->wait();
    
    /* Timer starts */
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&start);
    }
    
    for(int i = 0; i < iterations; i ++) {
    
        s_queue->enqueue(value, lock, tid);
        value += 1;

    }

    while(s_queue->dequeue(lock, tid) != -1) {}
    

	bar->wait();

    /* Timer Ends */
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&finish);
    }

}

/* Method to test the concurrent data strctures using Concurrent_DS_Tester class
 * Methode tests and analyses the paseed data structures. Takes no of iterations, 
 * no of threads and data structure to be tested
 */
void Concurrent_DS_Tester::test(int iterations, int number_of_threads, string data_structure) {

    Locks     lock(SELECTED_LOCK);
	Barriers  barrier(SELECTED_BARRIER, number_of_threads);    
    T_stack   t_stack(number_of_threads, iterations);
    SGL_stack s_stack(number_of_threads, iterations);
    MS_queue  m_queue;
    SGL_queue s_queue(number_of_threads);
    atomic<int>   epoch;
    atomic<int>*  reservations;

    epoch.store(0, ACQ);
    reservations = new atomic<int>[number_of_threads];

	/* Invoke threads for N number of threads */
	int ret; size_t i;
	for(i=1; i < number_of_threads; i++){

		DS_Thread_args* ThreadArgs    = new DS_Thread_args;
		ThreadArgs->barrier           = &barrier;
		ThreadArgs->lock              = &lock;
        ThreadArgs->t_stack           = &t_stack;
        ThreadArgs->s_stack           = &s_stack;
        ThreadArgs->m_queue           = &m_queue;
        ThreadArgs->s_queue           = &s_queue;
        ThreadArgs->epoch             = &epoch;
        ThreadArgs->reservations      = reservations;
		ThreadArgs->number_of_threads = number_of_threads;
        ThreadArgs->iterations        = iterations;
		ThreadArgs->tid               = i;
		
		/************* Testing Each DS ***********/

		/************* Treiber Stack *************/
        if(data_structure.compare("Treiber") == 0) {

            ret = pthread_create(&threads[i], NULL, &invoke_Treiber, (void*)ThreadArgs);

        }
		/************* M&S Queue ***********/
        else if(data_structure.compare("MSQ") == 0){

            ret = pthread_create(&threads[i], NULL, &invoke_MSQueue, (void*)ThreadArgs);

        }
		/************* SGL Stack ***********/
        else if(data_structure.compare("SGLStack") == 0){
			
            ret = pthread_create(&threads[i], NULL, &invoke_SGLStack, (void*)ThreadArgs);
			if(ret){
				printf("ERROR; pthread_create: %d\n", ret);
				exit(-1);
			}
			
        }
		/************* SGL Queue ***********/
        else if(data_structure.compare("SGLQueue") == 0){
            ret = pthread_create(&threads[i], NULL, &invoke_SGLQueue, (void*)ThreadArgs);
        }
        else {
			printf("\n**incorrect concurrent data structure name provided from user**\n\n");
            assert(false);
        }
        delete ThreadArgs;
	}

	DS_Thread_args* MasterThrdArgs     = new DS_Thread_args;
	MasterThrdArgs->barrier            = &barrier;
	MasterThrdArgs->lock               = &lock;
    MasterThrdArgs->t_stack            = &t_stack;
    MasterThrdArgs->s_stack            = &s_stack;
    MasterThrdArgs->m_queue            = &m_queue;
    MasterThrdArgs->s_queue            = &s_queue;
    MasterThrdArgs->epoch              = &epoch;
    MasterThrdArgs->reservations       = reservations;
	MasterThrdArgs->number_of_threads  = number_of_threads;
    MasterThrdArgs->iterations         = iterations;
	MasterThrdArgs->tid                = 0;

	// master also calls thread_main

	/************* Treiber Stack *************/
    if(data_structure.compare("Treiber") == 0) {
        invoke_Treiber((void*)MasterThrdArgs);
    }

	/************* M&S Queue ***********/
    else if(data_structure.compare("MSQ") == 0){

        invoke_MSQueue((void*)MasterThrdArgs);

    }
	/************* SGL Stack ***********/
    else if(data_structure.compare("SGLStack") == 0){
		/* Invoke SGL operations with thread arguments */
        invoke_SGLStack((void*)MasterThrdArgs);
		
    }
	/************* SGL Queue ***********/
    else if(data_structure.compare("SGLQueue") == 0){
		/* Invoke SGL operations with thread arguments */
        invoke_SGLQueue((void*)MasterThrdArgs);
    }

    else {
        assert(false);
    }

	/* Join threads at end */
	for(size_t i=1; i<number_of_threads; i++){
		ret = pthread_join(threads[i],NULL);
		if(ret){
			printf("Failed to join threads\n");
			exit(-1);
		}
	}

	/* Free memory */
    delete [] reservations;
    delete MasterThrdArgs;

	/* Report time consumed to terminal */
    unsigned long long elapsed_ns;
	elapsed_ns = (finish.tv_sec-start.tv_sec)*1000000000 + (finish.tv_nsec-start.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);

}	