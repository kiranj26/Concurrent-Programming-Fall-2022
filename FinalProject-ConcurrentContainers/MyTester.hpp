/**********************************************************/
/**********************************************************/
/*     Final Project  : Concurrent Programming            */
/*     Author         : Kiran Jojare                      */
/**********************************************************/
/**********************************************************/

#ifndef DS_TEST_HPP
#define DS_TEST_HPP

#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <atomic>
#include <limits.h>
#include "Locks.hpp"
#include "Barriers.hpp"
#include "MS_queue.hpp"
#include "Treiber_stack.hpp"
#include "SGL_queue.hpp"
#include "SGL_stack.hpp"

using std::string;
using std::atomic;
using std::cout;
using std::endl;
using std::multimap;

/* Change this values to "sense/pthread"*/
#define SELECTED_BARRIER "sense"
/* Change this values to "tas/ttas/ticket/pthread" */
#define SELECTED_LOCK "pthread"

/* Concurrency Premetives Needed */
#define ACQREL std::memory_order_acq_rel
#define ACQ std::memory_order_acquire

/* Structure for Arguments to be pased 
 * to each thead used for Testing */
struct DS_Thread_args {
	Locks       * lock;
    Barriers    * barrier;
    T_stack     * t_stack;
    SGL_stack   * s_stack;
    MS_queue    * m_queue;
    SGL_queue   * s_queue;
    atomic<int> * epoch;
    atomic<int> * reservations;
	
	int tid;
	int number_of_threads;
    int iterations;
};

/* Concurrent_DS_Tester class with 
 * constructers and thread parameter */
class Concurrent_DS_Tester {
    private:
    pthread_t *threads;                           /* Class parameter */

    public:
    Concurrent_DS_Tester(int number_of_threads);  /* Default Constructer */
    ~Concurrent_DS_Tester();                      /* Default Deconstructer */
    void test(int iterations, int number_of_threads, string data_structure);
};

#endif