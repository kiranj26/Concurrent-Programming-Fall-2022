/**********************************************************/
/**********************************************************/
/*     Final Project  : Concurrent Programming            */
/*     Author         : Kiran Jojare                      */
/**********************************************************/
/**********************************************************/


#ifndef TS_HPP
#define TS_HPP
#include <iostream>
#include <atomic>

#define ACQREL std::memory_order_acq_rel
#define ACQ std::memory_order_acquire

using std::atomic;

/* Selected required optamisation methode */
#define ELIMINATION_OPTIMIZATION_ON

/* Structure Definations for Treiber Stack */
struct treiberstack_node {
    int val;
    treiberstack_node* next;
};

/* Structure for treiber stack operations for elimination methode */
struct Treiber_Stack_operations {
    bool pop;
    bool push;
    int push_value;
};

/* Class Defination for Treiber Stack */
class T_stack{
	
    private:    
    atomic<treiberstack_node*> top;
    atomic<Treiber_Stack_operations*>* elimination_arr;
    int ELIM_ARRAY_SIZE;

    public:
    T_stack(int number_of_threads, int iterations); /* Default Constructer */
    ~T_stack();                                     /* Default Deconstructer */
    void push(int val);
    treiberstack_node* pop();

};

#endif


