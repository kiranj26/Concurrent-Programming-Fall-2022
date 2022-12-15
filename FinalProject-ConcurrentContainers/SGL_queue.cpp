/**********************************************************/
/**********************************************************/
/*     Final Project  : Concurrent Programming            */
/*     Author         : Kiran Jojare                      */
/**********************************************************/
/**********************************************************/


/* include header file for SQL Queue */
#include "SGL_queue.hpp"

/* Constructor for the SQL Queue Class */
SGL_queue::SGL_queue(int number_of_threads) {

    head = NULL;
    tail = NULL;
    #ifdef FLAT_COMBINING_OPTIMIZATION_ON
		NUM_THREADS = number_of_threads;
		flat_combining_array = new atomic<SGL_Queue_operations*>[number_of_threads];

		for(int i = 0; i < number_of_threads; i++) {
			flat_combining_array[i].store(NULL);
		}
    #endif
}

/* Deconstructor for the SQL Queue Class */
SGL_queue::~SGL_queue() {

    #ifdef FLAT_COMBINING_OPTIMIZATION_ON
		delete [] flat_combining_array;
    #endif
}

/* Function to dequeue any variable from SGL Queue Class 
 * Main output of the function is the queue node whos value is 
 * dequeued. The algorithm uses a flat combining method where 
 * lock holders performes all operation on main array while
 * non lock holders place the required operation in array
 * Function will return -1 if queue is empty
 */
int SGL_queue::dequeue(Locks* lock, int tid) {

    #ifdef FLAT_COMBINING_OPTIMIZATION_ON	
		/* For non lock holders */
		while(lock->try_acquire() == EBUSY) {
			/* If you are a non lock holder place your operation in flat combiniing array and wait */
			SGL_Queue_operations* operation = new SGL_Queue_operations;
			operation->dequeue = true;
			operation->enqueue = false;
			while(true) {
				/* Load flat combining array and compare and swap with requested operation */
				SGL_Queue_operations* current_index = flat_combining_array[tid].load(ACQ);
				if(flat_combining_array[tid].compare_exchange_strong(current_index,operation,ACQREL)) {
					while(lock->try_acquire() == EBUSY) {}
					break;
				}
			}
			break;
		}
	
		/* Processing operations inside flat combining array */
		for(int i = 0; i < NUM_THREADS; i++) {
			SGL_Queue_operations* current_operation = flat_combining_array[i].load(ACQ);
			if(current_operation != NULL) {
				/* Keep in mind to not allow other threads to change SQL operation so making index NULL */
				if(flat_combining_array[i].compare_exchange_strong(current_operation,NULL)) {
					/* Process enqueue operations if current process is enqueue */
					if(current_operation->enqueue) {
						queue_node* new_queue_node = new queue_node;
						new_queue_node->val = current_operation->enqueue_value;
						new_queue_node->next = NULL;

						if(!head || !tail) {
							head = new_queue_node;
							tail = new_queue_node;
						}
						else {
							tail->next = new_queue_node;
							tail = new_queue_node;
						}
						delete(current_operation);
					}
					/* Process dequeue operations if current process is dequeue */
					else if(current_operation->dequeue) {
						// Checking if there are contents in the queue
						/* In case when there is no element in the queue */
						if(!(!head || !tail)) {
							if(head == tail) {
								int value = head->val;
								head = NULL;
								tail = NULL;
							}
							/* If there are elements in queue */
							else {
								queue_node* temp = head;
								head = head->next;
								int value = temp->val;
								delete(temp);
							}
						}
						delete(current_operation);
					}
				}
			}
		}

		/* Processing lock holder's operation */
		/* If there are elements in the queue */
		if(!(!head || !tail)) {
			if(head == tail) {
				int value = head->val;
				delete(head);
				head = NULL;
				tail = NULL;
				lock->release();
				return value;
			}
			else {
				queue_node* temp = head;
				head = head->next;
				int value = temp->val;
				delete(temp);
				lock->release();
				return value;
			}
		}
		/* No elemenets in queue release and return -1 */
		else {
			lock->release();
			return -1;
		}
		lock->release();
	
    #else
	/* Dequeue logic without flat combining technique */
		lock->acquire();
		/* If there are elements in queue */
		if(!(!head || !tail)) {
			if(head == tail) {
				int value = head->val;
				delete(head);
				head = NULL;
				tail = NULL;
				lock->release();
				return value;
			}
			else {
				queue_node* temp = head;
				head = head->next;
				int value = temp->val;
				delete(temp);
				lock->release();
				return value;
			}
		}
		/* No elemenets in queue release and return -1 */
		else {
			lock->release();
			return -1;
		}
		lock->release();
    #endif

}

/* Function to dequeue any variable from SGL Queue Class. New value 
 * node to be added is the input to the function Functions loads 
 * new node into the latest queue. Using flat combining technique
 * lock holders perform all operations on array while non lock
 * holders only places the operations to be perform on to the array
*/
void SGL_queue::enqueue(int val, Locks* lock, int tid) {

    queue_node *new_queue_node;
    new_queue_node = new queue_node;
    new_queue_node->val = val;
    new_queue_node->next = NULL;

    #ifdef FLAT_COMBINING_OPTIMIZATION_ON
	/* Enqueue logic with flat combining technique */
	/* No Lock Holder */
	while(lock->try_acquire() == EBUSY) {
		/* No Lock Holder so place operations in falt combining array */
        SGL_Queue_operations* operation = new SGL_Queue_operations;
        operation->dequeue = false;
        operation->enqueue = true; /* Setting enqueue as true */
        operation->enqueue_value = val;

        while(true) {
            SGL_Queue_operations* current_index = flat_combining_array[tid].load(ACQ);
            if(flat_combining_array[tid].compare_exchange_strong(current_index,operation,ACQREL)) {
                while(lock->try_acquire() == EBUSY) {}
                break;
            }
        }
        break;
    }
	/* Processing operations inside flat combining array */
    for(int i = 0; i < NUM_THREADS; i++) {

        SGL_Queue_operations* current_operation = flat_combining_array[i].load(ACQ);
        if(current_operation != NULL) {
            /* Make sure no other thread changes operation by making index NULL */
            if(flat_combining_array[i].compare_exchange_strong(current_operation,NULL)) {
                if(current_operation->enqueue) {
                    if(!head || !tail) {
                        head = new_queue_node;
                        tail = new_queue_node;
                    }
                    else {
                        tail->next = new_queue_node;
                        tail = new_queue_node;
                    }
                    delete(current_operation);
                }
                else if(current_operation->dequeue) {
					/* Checking if there are elements in queue */
                    if(!(!head || !tail)) {
                        if(head == tail) {
                            int value = head->val;
                            head = NULL;
                            tail = NULL;
                        }
                        else {
                            queue_node* temp = head;
                            head = head->next;
                            int value = temp->val;
                            delete(temp);
                        }
                    }
                    delete(current_operation);
                }
            }
        }
    }
	/* Executing lock holders operation */
    if(!head || !tail) {
        head = new_queue_node;
        tail = new_queue_node;
    }
    else {
        tail->next = new_queue_node;
        tail = new_queue_node;
    }

    lock->release();
    return;

    #else 
	/* Dequeue logic without flat combining technique */
		lock->acquire();
		if(!head || !tail) {
			head = new_queue_node;
			tail = new_queue_node;
		}
		else {
			tail->next = new_queue_node;
			tail = new_queue_node;
		}
		lock->release();

    #endif
}
