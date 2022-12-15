/**********************************************************/
/**********************************************************/
/*     Final Project  : Concurrent Programming            */
/*     Author         : Kiran Jojare                      */
/*     This is the barrier function for this project.     */
/**********************************************************/
/**********************************************************/

#include "Barriers.hpp"


/* Constructor for the Barriers Class
 * Decide which barrier algorithm to use using the passed argument. Make the 
 * required instantiations for the desired barrier.
 */
Barriers::Barriers(string barrierType, int number_of_threads) {

    NUM_THREADS = number_of_threads;

	/* Select which barrier to choose from */
    if(barrierType.compare("sense") == 0) {
		/* Sense Barrier */
        bar = sense_barrier;
        cnt.store(0,RELAXED);
        sense.store(0,RELAXED);
    }
    else if(barrierType.compare("pthread") == 0) {
		/* Pthread barrier wait */
        bar = pthread_barrier;
        pthread_barrier_init(&p_bar, NULL, number_of_threads);
    }
    else {
        assert(false);
    }
}

/* Destructor for the Barriers class
 * If the pthread barrier was used, it is destroyed.
 */
Barriers::~Barriers() {
    
	/* Destroy pthread attributes in decnstruction */
    if(bar == pthread_barrier) {
        pthread_barrier_destroy(&p_bar);
    }
}

/* Barrier Sense reversal Algorithm
 * Each thread has its own distinct local sense. All threads before the last thread 
 * will spin while they wait for the last thread to arrive, which causes the global 
 * sense to flip.
 */
void Barriers::sense_wait(){

    thread_local bool my_sense = 0;
    if(my_sense == 0) {
        /*Flipping local sense */
        my_sense = 1; 
    }
    else {
        my_sense = 0;
    }

    int cnt_cpy = cnt.fetch_add(1,SEQ_CST);

    /* Last thread to arrive */
    if(cnt_cpy == NUM_THREADS-1) { 
        cnt.store(0, RELAXED);
        sense.store(my_sense,SEQ_CST);
    }

    /* Not last thread to arrive */
    else { 
        while(sense.load(SEQ_CST) != my_sense) {}
    }

}

/* Barrier Handler
 */
void Barriers::wait() {

    switch(bar) {
        case(sense_barrier):{
            sense_wait();
            break;
        }
        case(pthread_barrier):{         
            pthread_barrier_wait(&p_bar);
            break;
        }
        default:{
            break;
        }
    }
}