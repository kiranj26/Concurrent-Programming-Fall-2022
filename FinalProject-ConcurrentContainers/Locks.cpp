/**********************************************************/
/**********************************************************/
/*     Final Project  : Concurrent Programming            */
/*     Author         : Kiran Jojare                      */
/**********************************************************/
/**********************************************************/

#include "Locks.hpp"

/* Functions */

/* Constructor for the Locks Class 
 * Select the lock algorithm to use using the passed argument. Make the necessary
 * instantiations for the desired lock.
 */
Locks::Locks(string lockType) {

   /* Check whichc lock we have to use */
    if(lockType.compare("tas") == 0) {
        lock = tas;
        flag.store(false,RELAXED);
    }
    else if(lockType.compare("ttas") == 0) {
        lock = ttas;
        flag.store(false,RELAXED);
    }
    else if(lockType.compare("ticket") == 0) {
        lock = ticket;
        next_num.store(0,RELAXED);
        now_serving.store(0,RELAXED);
    }
    else if(lockType.compare("pthread") == 0) {
        lock = pthread_lock;
        pthread_mutex_init(&p_lock, NULL);
    }
    else {
        assert(false);
    }

}


/* destructor for the Locks class  
 * destroys the pthread lock if it was activated.
 */
Locks::~Locks() {

    if(lock == pthread_lock) {

        pthread_mutex_destroy(&p_lock);
        
    }

}


/* The tas and ttas locks employ a test-and-set procedure.
 * sets flag to true if flag is false otherwise checks true. if the flag is held,
 * returns flase.
*/
bool Locks::testAndSet() {

    bool expected = false;
    return flag.compare_exchange_strong(expected, true, SEQ_CST);

}


/* TAS Lock
*/
void Locks::tas_lock() {

    while(testAndSet() == false){}

}

void Locks::tas_unlock() {

    flag.store(false,SEQ_CST);

}


/* TTAS Lock
 * uses the test and set method to contend on the flag while continuously checking
 * the value of the flag to see if it has been released. Value checking relieves 
 * congestion on the flag cache line.
 */
void Locks::ttas_lock() {

    while(flag.load(SEQ_CST) == true || testAndSet() == false){}

}

/* TTAS Unlock
*/
void Locks::ttas_unlock() {

    flag.store(false,SEQ_CST); 

}

/* Ticket lock
*/
void Locks::ticket_lock() {

    int my_num = next_num.fetch_add(1,SEQ_CST);
    while(now_serving.load(SEQ_CST) != my_num){}

}
/* Ticket Unlock
*/

void Locks::ticket_unlock() {

    now_serving.fetch_add(1,SEQ_CST);

}

/* try_acquire method rfor the pthread lock
 * tries to obtain lock and returns an integer indicating success or various types 
 * of failure.
 */
int Locks::try_acquire() {

    pthread_mutex_trylock(&p_lock);

}


/* Lock Aquire Handler Methode */
void Locks::acquire() {

    switch(lock) {
        case(tas):{
            tas_lock();
            break;
        }
        case(ttas):{         
            ttas_lock();
            break;
        }
        case(ticket):{           
            ticket_lock();
            break;
        }
        case(pthread_lock):{           
            pthread_mutex_lock(&p_lock);
            break;
        }
        default:{
            assert(false);
            break;
        }
    }
}

/* Lock Release Handler Methode */
void Locks::release() {

    switch(lock) {
        case(tas):{
            tas_unlock();
            break;
        }
        case(ttas):{
            ttas_unlock();
            break;
        }
        case(ticket):{           
            ticket_unlock();
            break;
        }
        case(pthread_lock):{
            pthread_mutex_unlock(&p_lock);
            break;
        }
        default:{
            assert(false);
            break;
        }
    }
}





