/**********************************************************/
/**********************************************************/
/*     Final Project  : Concurrent Programming            */
/*     Author         : Kiran Jojare                      */
/**********************************************************/
/**********************************************************/


#include "Treiber_stack.hpp"


/* Constructor for the Treiber_Stack Class
 * Set top to a fresh instance of the treiberstack node struct after initializing the global lock. 
 * Additionally initializes the elimination array and fills it with NULL values if elimination 
 * optimization is enabled.
 */

T_stack::T_stack(int number_of_threads, int iterations) {

    treiberstack_node* n = NULL;
    top.store(n, ACQREL);

    #ifdef ELIMINATION_OPTIMIZATION_ON
		ELIM_ARRAY_SIZE = (number_of_threads * iterations)/2;
		elimination_arr = new atomic<Treiber_Stack_operations*>[ELIM_ARRAY_SIZE];
		for(int i = 0; i < ELIM_ARRAY_SIZE; i++) {
			elimination_arr[i].store(NULL);
		}
    #endif
}  


/* Destructor for the Treiber_Stack class
 * if the optimization is enabled, deletes the elimination array.
 */
T_stack::~T_stack() {

    top.store(NULL, ACQREL);
    #ifdef ELIMINATION_OPTIMIZATION_ON
		delete [] elimination_arr;
    #endif

}

/*  This is a time delay function. */

static void time_delay(int milliseconds) {

    clock_t start_time = clock();
    while(clock() < start_time + milliseconds) {};

}


/* Pop method for the Treiber_Stack class
 * The function returns the value of the popped treiberstack node after popping the top of the stack. Contention on
 * the global lock is reduced by elimination optimization. Non-lock holders choose a random index in the elimination
 * array and check what is in the index while lock holders complete their operation and return. The thread will 
 * execute both operations and then return if the complementary operation is present in the index. The operation 
 * will place its operation in the index and wait for a delay if the index is NULL. It then determines 
 * whether another thread has already completed the operation. If not, it then competes while locked. 
 * If the thread fails to obtain the lock, the procedure is repeated.
 */
treiberstack_node* T_stack::pop() {

    while(true){

        treiberstack_node* to_pop = top.load(ACQ);
        if(to_pop == NULL) {
            treiberstack_node* end_signal = new treiberstack_node;
            end_signal->val = -1;
            return end_signal;   
        }
        
        treiberstack_node* new_top = to_pop->next;
        bool success = top.compare_exchange_strong(to_pop, new_top, ACQREL);
        if(success) {
            return to_pop;
        }

        #ifdef ELIMINATION_OPTIMIZATION_ON
        else {

            while(true) {
                int index = rand() % ELIM_ARRAY_SIZE;
                Treiber_Stack_operations* elimination_index = elimination_arr[index].load(ACQ);
                /* Empty slot in elimination array */
                if(elimination_index == NULL) {
                    Treiber_Stack_operations* operation = new Treiber_Stack_operations;
                    operation->pop = true;
                    operation->push = false;
                    /* Load operations */
                    if(elimination_arr[index].compare_exchange_strong(elimination_index, operation, ACQREL)) {
                        /* Input Delay */
                        time_delay(200);
                        Treiber_Stack_operations* temp = elimination_arr[index].load(ACQ);
                        /* Wait and dont allow any sort of combining*/
                        if(temp != NULL) {
                            /* Pull out operation and retry */
                            elimination_arr[index].compare_exchange_strong(temp, NULL);
                            treiberstack_node* return_node = pop();
                            return return_node;
                        }
                    }
                }
                /* Push operation Slotted */
                else if(elimination_index->push) {
                    /* PMatching operations should be run, followed by an elimination array operation. */
                    if(elimination_arr[index].compare_exchange_strong(elimination_index, NULL, ACQREL)) {
                        int value = elimination_index->push_value;
                        treiberstack_node* return_node = new treiberstack_node;
                        return_node->val = value;  
                        delete(elimination_index);
                        return return_node;
                    }
                }
            }
        }
        #endif
    }
}

/* Push method for the Treiber_Stack class
 * new treiberstack node is pushed to the top of the stack. Contention on the global lock is reduced by 
 * elimination optimization. Non-lock holders choose a random index in the elimination array and check 
 * what is in the index while lock holders complete their operation and return. The thread will execute 
 * both operations and then return if the complementary operation is present in the index. The operation 
 * will place its operation in the index and wait for a delay if the index is NULL. It then determines whether
 * another thread has already completed the operation. If not, it engages in a new lock-on conflict. 
 * If the thread fails to obtain the lock, the procedure is repeated.
 */
void T_stack::push(int val) {

    treiberstack_node* new_top = new treiberstack_node;
    new_top->val = val;

    while(true) {
        treiberstack_node* old_top = top.load(ACQ);
        new_top->next = old_top;
        bool success = top.compare_exchange_strong(old_top, new_top, ACQREL); 
        if(success){
            break;
        }

        #ifdef ELIMINATION_OPTIMIZATION_ON
        else {

            while(true) {
                int index = rand() % ELIM_ARRAY_SIZE;
                Treiber_Stack_operations* elimination_index = elimination_arr[index].load(ACQ);
                /* Empty slot in elimination array */
                if(elimination_index == NULL) {
                    Treiber_Stack_operations* operation = new Treiber_Stack_operations;
                    operation->pop = false;
                    operation->push = true;
                    operation->push_value = val;
                    /* Load operations */
                    if(elimination_arr[index].compare_exchange_strong(elimination_index, operation, ACQREL)) {
                        /* delay */
                        time_delay(200);
                        Treiber_Stack_operations* temp = elimination_arr[index].load(ACQ);
                        /* Wait and dont allow any sort of combining*/
                        if(temp != NULL) {
                            /* Pull out operation and retry */
                            elimination_arr[index].compare_exchange_strong(temp, NULL);
                            push(val);
                            return;
                        }
                    }
                }
                /* Pop operation Slotted */
                else if(elimination_index->pop) {
                    // Matching operations should be run, followed by an elimination array operation.
                    if(elimination_arr[index].compare_exchange_strong(elimination_index, NULL, ACQREL)) {      
                        delete(new_top);
                        delete(elimination_index);
                        return;
                    }
                }
            }
        }
        #endif
    }
}