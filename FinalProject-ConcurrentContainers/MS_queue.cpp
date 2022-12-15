/**********************************************************/
/**********************************************************/
/*     Final Project  : Concurrent Programming            */
/*     Author         : Kiran Jojare                      */
/**********************************************************/
/**********************************************************/


#include "MS_queue.hpp"

/* Constructor for the MS Queue Class
 * Function initialises the dummy msqueue node 
 * and sets head and tail to dummy
*/
MS_queue::MS_queue() {
    msqueue_node* dummy_node = new msqueue_node;
    dummy_node->val = 21;
    dummy_node->next = NULL;
    head.store(dummy_node, RELAXED);
    tail.store(dummy_node, RELAXED);
}

/* Deconstructor for MS queue class
 */
MS_queue::~MS_queue() {

    msqueue_node* garbage_node = head.load();
    delete(garbage_node);

}

/*  Enqueue function for MS queue class
 *  Functon places msqueue node in the queue and uses CAS operation to liniarise
 * Function returs value of the new node
 */
void MS_queue::enqueue(int val) {
    msqueue_node *imposter_tail, *true_end, *new_msqueue_node;
    new_msqueue_node = new msqueue_node;
    new_msqueue_node->val = val;
    while(true) {
        imposter_tail = tail.load(ACQ);
        true_end = imposter_tail->next.load(ACQ);

        if(imposter_tail == tail.load(ACQ)) {
            if(true_end == NULL && imposter_tail->next.compare_exchange_strong(true_end, new_msqueue_node)) {       
                break;
            }    
            else if(true_end != NULL) {
                tail.compare_exchange_strong(imposter_tail, true_end);
            }
        }
    }
    tail.compare_exchange_strong(imposter_tail, new_msqueue_node);
}


/* Dequeue function method for MS Queue Class 
 * Function dequeues and returns value of the dequed msqueue_node.
 * Uses CAS to liniriase with threads. Returns -1 if queue is empty
 */

msqueue_node* MS_queue::dequeue() {
    msqueue_node *dummy_node, *new_dummy, *imposter_tail;
    while(true) {
        dummy_node = head.load(ACQ);
        imposter_tail = tail.load(ACQ);
        new_dummy = dummy_node->next.load(ACQ);

        if(dummy_node == head.load(ACQ)) {
            if(dummy_node == imposter_tail) {
                if(new_dummy == NULL) {
                    dummy_node->val = -1;
                    return dummy_node;
                }
                else {
                    tail.compare_exchange_strong(imposter_tail, new_dummy, ACQREL);
                }
            }
            else {
                int ret = new_dummy->val;
                if(head.compare_exchange_strong(dummy_node, new_dummy, ACQREL)) {
                    return dummy_node;
                }
            }
        }
    } 
}