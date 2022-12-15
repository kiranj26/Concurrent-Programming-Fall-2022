/**********************************************************/
/**********************************************************/
/*     Final Project  : Concurrent Programming            */
/*     Author         : Kiran Jojare                      */
/**********************************************************/
/**********************************************************/

#ifndef SGL_S_HPP
#define SGL_S_HPP

#include <iostream>
#include <atomic>
#include <vector>
#include "Locks.hpp"

using std::atomic;

#define ACQREL std::memory_order_acq_rel
#define ACQ std::memory_order_acquire
#define DELAY 200


/* Select required optamisation methode */

#define ELIMINATION_OPTIMIZATION_ON
// #define FLAT_COMBINING_OPTIMIZATION_ON
// #define NO_OPTIMIZATION

/* Structure Definations for SGL Stack*/
struct stacknode {
    int  val;
    stacknode * next;
};

struct SGL_Stack_functions {
    bool pop;
    bool push;
    int  push_value;
};

/* Class Defination for SGL Stack */
class SGL_stack {

    private:
    Locks     *  lock;
    stacknode *  top_stacknode;
    int          ELIMINATION_ARR_SIZE;
    int          thread_count;
    atomic<SGL_Stack_functions*>* flat_combining_arr;
    atomic<SGL_Stack_functions*>* elimination_arr;

    public:
    SGL_stack(int no_of_threads, int iterations);  /* Default Constructer */
    ~SGL_stack();                                  /* Default Deconstructer */
    void push(int val, Locks* lock, int tid);
    int pop(Locks* lock, int tid);

};

#endif

