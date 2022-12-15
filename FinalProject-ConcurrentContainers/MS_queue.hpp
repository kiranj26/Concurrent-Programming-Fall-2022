/**********************************************************/
/**********************************************************/
/*     Final Project  : Concurrent Programming            */
/*     Author         : Kiran Jojare                      */
/**********************************************************/
/**********************************************************/

#ifndef MS_HPP
#define MS_HPP

/* Include files */

#include <iostream>
#include <atomic>


#define ACQREL std::memory_order_acq_rel
#define ACQ std::memory_order_acquire
#define RELAXED std::memory_order_relaxed

using std::string;
using std::atomic;
using std::cout;
using std::endl;

/* Structure Definations for MandS Queue */
struct msqueue_node {
    int val;
    atomic<msqueue_node*> next;
};

/* Class Defination for MS Queue */
class MS_queue{

    private:
    atomic<msqueue_node*> head, tail;

    public:
    MS_queue();           /* Default Constructer */
    ~MS_queue();          /* Default Deconstructer */
    void enqueue(int val);
    msqueue_node* dequeue();

};

#endif                   