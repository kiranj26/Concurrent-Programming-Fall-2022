/***********************************************************************/
/***********************************************************************/
/****     Final Project  : Concurrent Programming         **************/
/****     Author         : Kiran Jojare                   **************/
/***********************************************************************/
/***********************************************************************/


/* Include */
#include "SGL_stack.hpp"

/*
 *  This is the constructor for the SGL_Stack Class
 *  Initialize the global lock  and top node. If flat combining optimization is turned on,
 *  initialize the flat combining array. If elimination opimizaation is turned on, initialize
 *  the eliminations array. 
 */
SGL_stack::SGL_stack(int no_of_threads, int iterations) {
	/* Initialising top as NULL */
    stacknode * initialized_stacknode = NULL;
    top_stacknode  = initialized_stacknode;

    #ifdef ELIMINATION_OPTIMIZATION_ON
		/* Initialising Variables and Array */
		ELIMINATION_ARR_SIZE = (no_of_threads * iterations)/2;
		elimination_arr      = new atomic<SGL_Stack_functions*>[ELIMINATION_ARR_SIZE];
		for(int i = 0; i < ELIMINATION_ARR_SIZE; i++) {
			elimination_arr[i].store(NULL);
		}

    #endif

    #ifdef FLAT_COMBINING_OPTIMIZATION_ON
		/* Initialising Variables and Array */
		thread_count       = no_of_threads;
		flat_combining_arr = new atomic<SGL_Stack_functions*>[no_of_threads];
		for(int i = 0; i < no_of_threads; i++) {
			flat_combining_arr[i].store(NULL);
		}

    #endif

}

/*
 *  This is the destructor for the SGL_Stack class
 *  Deletes the flat combining array if it was created. Deletes the eliminations array 
 *  if it was created
 */
SGL_stack::~SGL_stack() {

    top_stacknode = NULL;

    #ifdef ELIMINATION_OPTIMIZATION_ON
		delete [] elimination_arr;
    #endif

    #ifdef FLAT_COMBINING_OPTIMIZATION_ON
		delete [] flat_combining_arr;
    #endif

}

/***************************************************************************//**
 *  This is a time delay function. 
 *  @param[in] milliseconds
 *  Length of delay in milliseconds
 ******************************************************************************/
static void time_delay(int milliseconds) {

    clock_t start_time = clock();

    while(clock() < start_time + milliseconds) {};

}


/* Pop method for the SGL_Stack class
 * Function pops the top node of the stack and returns its value. The array's operations are all carried out by 
 * the lock holder thanks to flat combining optimization. Place your operations in the array if you don't hold a
 * lock. After inserting an operation into the array, each thread competes for the lock. Similar to elimination
 * optimization, global lock contention is relieved by elimination optimization. Non-lock holders choose a random
 * index in the elimination array and check what is in the index after the lock holder completes their operation
 * and returns. The thread will execute both operations and then return if the complementary operation is present
 * in the index. The operation will place its operation in the index and wait for a delay if the index is NULL. 
 * It then determines whether another thread has already completed the operation. If not, it engages in a new 
 * lock-on conflict. If the thread fails to obtain the lock, the procedure is repeated.
 */
int SGL_stack::pop(Locks* lock, int tid) {

    int value;

    while(true) {

        // Elimination optimization turned on
        #ifdef ELIMINATION_OPTIMIZATION_ON

			while(lock->try_acquire() == EBUSY) {

				int index = rand() % ELIMINATION_ARR_SIZE;
				SGL_Stack_functions* elimination_index = elimination_arr[index].load(ACQ);
				/* Vacant Positions in an array */
				if(elimination_index == NULL) {
					SGL_Stack_functions* operation = new SGL_Stack_functions;
					operation->pop = true;
					operation->push = false;
					/* Load Operations */
					if(elimination_arr[index].compare_exchange_strong(elimination_index, operation, ACQREL)) {
						/* Input delay */
						time_delay(DELAY);
						SGL_Stack_functions* temp = elimination_arr[index].load(ACQ);
						/* Wait and dont allow any combining */
						if(temp != NULL) {
							/* Take operations ou and retry*/
							elimination_arr[index].compare_exchange_strong(temp, NULL);
						}
					}
				}

				/* Push Operation */
				else if(elimination_index->push) {
					/* Performe operations and remove from array once done*/
					if(elimination_arr[index].compare_exchange_strong(elimination_index, NULL, ACQREL)) {
						int return_value = elimination_index->push_value;
						delete(elimination_index);
						return return_value;
					}
				}
			}
		
			/* If top stack is null */
			if(top_stacknode != NULL) {
				stacknode* temp = top_stacknode;
				top_stacknode = temp->next;
				int value = temp->val;
				delete(temp);

				lock->release();
				return value;
			}

			else {
				/* If stack is empty then 
				   return -1 and release lock */
				lock->release();
				return -1;
			}

        #endif


        #ifdef FLAT_COMBINING_OPTIMIZATION_ON

			/* For non lock holders */
			while(lock->try_acquire() == EBUSY) {
				/* If you are not a lock holder place operation inside array */
				SGL_Stack_functions* operation = new SGL_Stack_functions;
				operation->pop = true;
				operation->push = false;
				while(true) {
					SGL_Stack_functions* current_index = flat_combining_arr[tid].load(ACQ);
					if(flat_combining_arr[tid].compare_exchange_strong(current_index,operation,ACQREL)) {
						while(lock->try_acquire() == EBUSY) {}
						break;
					}
				}
				break;
			}

			/* For lock holders */

			for(int i = 0; i < thread_count; i++) {
				SGL_Stack_functions* current_operation = flat_combining_arr[i].load(ACQ);
				if(current_operation != NULL) {
					if(flat_combining_arr[i].compare_exchange_strong(current_operation,NULL)) {
						if(current_operation->push) {
							stacknode* updated_top_stacknode = new stacknode;
							updated_top_stacknode->val = current_operation->push_value;
							updated_top_stacknode->next = top_stacknode;
							top_stacknode = updated_top_stacknode;
							delete(current_operation);

						}
						else if(current_operation->pop) {

							if(top_stacknode != NULL) {
								stacknode* temp = top_stacknode;
								int value = temp->val;
								top_stacknode = temp->next;
								delete(temp);
							}
							delete(current_operation);
						}   
					}
				}
			}

			if(top_stacknode != NULL) {

				stacknode* temp = top_stacknode;
				top_stacknode = temp->next;
				int value = temp->val;
				delete(temp);
				lock->release();

				return value;
			}

			else {
				/* If stack is empty then 
				   return -1 and release lock */
				lock->release();
				return -1;
			}

        #endif


        /* No Optimisation Provided  */
        #ifdef NO_OPTIMIZATION

			lock->acquire();

			if(top_stacknode != NULL) {
				/* If stack is not empty */
				stacknode* temp = top_stacknode;
				top_stacknode = temp->next;
				int value = temp->val;
				delete(temp);

				lock->release();

				return value;
			}

			else {
				/* If stack is empty then 
				   return -1 and release lock */
				lock->release();
				return -1;

			}

        #endif

    }   


}


/* Push method for the SGL_Stack class
 * new node is pushed to the top of the stack. The array's operations are all carried out by the lock holder 
 * thanks to flat combining optimization. Place your operations in the array if you don't hold a lock. After
 * inserting an operation into the array, each thread competes for the lock. Similar to elimination optimization,
 * global lock contention is relieved by elimination optimization. Non-lock holders choose a random index in 
 * the elimination array and check what is in the index after the lock holder completes their operation 
 * and returns. The thread will execute both operations and then return if the complementary operation is 
 * present in the index.
 * The operation will place its operation in the index and wait for a delay if the index is NULL. It then 
 * determines whether another thread has already completed the operation. If not, it engages in a new 
 * lock-on conflict. If the thread fails to obtain the lock, the procedure is repeated.
 */
void SGL_stack::push(int val, Locks* lock, int tid) {

    
    stacknode* new_top = new stacknode;
    new_top->val = val;

    #ifdef ELIMINATION_OPTIMIZATION_ON

		while(lock->try_acquire() == EBUSY) {

			int index = rand() % ELIMINATION_ARR_SIZE;
			SGL_Stack_functions* elimination_index = elimination_arr[index].load(ACQ);

			/* Eliminations array slots emptied */
			if(elimination_index == NULL) {

				SGL_Stack_functions* operation = new SGL_Stack_functions;
				operation->pop = false;
				operation->push = true;
				operation->push_value = val;

				/* Loading Operations into Array*/
				if(elimination_arr[index].compare_exchange_strong(elimination_index, operation, ACQREL)) {
					/* Delay */
					time_delay(DELAY);
					SGL_Stack_functions* temp = elimination_arr[index].load(ACQ);
					if(temp != NULL) {
						elimination_arr[index].compare_exchange_strong(temp, NULL);
					}
				}
			}

			/* Push operation located */
			else if(elimination_index->pop) {

				/* Perform matching operations and remove operation from elimination array */
				if(elimination_arr[index].compare_exchange_strong(elimination_index, NULL, ACQREL)) {

					int return_value = elimination_index->push_value;
					delete(elimination_index);
					delete(new_top);
					return;
				}
			} /* End else if*/
		} /* End While Loop */

		new_top->next = top_stacknode;
		top_stacknode = new_top;

		lock->release();

		return;

    #endif


    #ifdef FLAT_COMBINING_OPTIMIZATION_ON

		/* Non Lock Holders Operations */
		while(lock->try_acquire() == EBUSY) {
			/* Put the operation into the combining array if the lock holder is not present. */
			SGL_Stack_functions* operation = new SGL_Stack_functions;
			operation->pop = false;
			operation->push = true;
			operation->push_value = val;

			while(true) {
				SGL_Stack_functions* current_index = flat_combining_arr[tid].load(ACQ);
				if(flat_combining_arr[tid].compare_exchange_strong(current_index,operation,ACQREL)) {
					while(lock->try_acquire() == EBUSY) {}
					break;
				}
			} /* End While Loop */
			break;
		} /* End While Loop */

		/* Lock Holders Operations */

		for(int i = 0; i < thread_count; i++) {

			SGL_Stack_functions* current_operation = flat_combining_arr[i].load(ACQ);
			if(current_operation != NULL) {
				if(flat_combining_arr[i].compare_exchange_strong(current_operation,NULL)) {
					if(current_operation->push) {
						stacknode* updated_top_stacknode = new stacknode;
						updated_top_stacknode->val = current_operation->push_value;
						updated_top_stacknode->next = top_stacknode;
						top_stacknode = updated_top_stacknode;
						delete(current_operation);
					}

					else if(current_operation->pop) {
						if(top_stacknode != NULL) {
							stacknode* temp = top_stacknode;
							int value = temp->val;
							top_stacknode = temp->next;
							delete(temp);
						}
						delete(current_operation);
					}   
				}
			}
		}

		new_top->next = top_stacknode;
		top_stacknode = new_top;
		lock->release();
	
		return;

    #endif

	/* No Optamisation Provided */
	#ifdef NO_OPTIMIZATION

		lock->acquire();

		new_top->next = top_stacknode;
		top_stacknode = new_top;

		lock->release();

    #endif

}