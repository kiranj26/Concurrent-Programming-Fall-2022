/**********************************************************/
/**********************************************************/
/*     Final Project  : Concurrent Programming            */
/*     Author         : Kiran Jojare                      */
/**********************************************************/
/**********************************************************/

#ifndef SGL_Q_HPP
#define SGL_Q_HPP

#include <iostream>
#include <atomic>
#include "Locks.hpp"

using std::atomic;
using std::cout;
using std::endl;

#define ACQREL std::memory_order_acq_rel
#define ACQ std::memory_order_acquire

/* Selected required optamisation methode */
#define FLAT_COMBINING_OPTIMIZATION_ON


/* Structure Definations for SGL Stack*/
struct queue_node {
    int val;
    queue_node* next;
};

struct SGL_Queue_operations {
    bool dequeue;
    bool enqueue;
    int  enqueue_value;
};

/* Class Defination for SGL Stack */
class SGL_queue {

    private:
    Locks *        lock;
    queue_node *   head;
    queue_node *   tail;
    int     NUM_THREADS;
    atomic<SGL_Queue_operations*>* flat_combining_array;

    public:
    SGL_queue(int number_of_threads);            /* Default Constructer */
    ~SGL_queue();                                /* Default Deconstructer */
    void enqueue(int val, Locks* lock, int tid);
    int  dequeue(Locks* lock, int tid);
};

#endif
