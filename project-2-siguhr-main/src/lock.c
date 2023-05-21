/* lock.c
 *
 * Implementation of locks.
 */

#include "common.h"
#include "lock.h"
#include "scheduler.h"


/*
 *  Set lock status to UNLOCKED
 */
void lock_init(lock_t *l) {
    l->lock_status = UNLOCKED;
    l->q_head = NULL;
}


/*
 *  If UNLOCKED aquire lock for process, else block
 */
void lock_acquire(lock_t *l) {
    if (l->lock_status == UNLOCKED) {
        l->lock_status = LOCKED;
    }
    else if (l->lock_status == LOCKED) {
        block(&(l->q_head));
    }
}

/*
 *  Release lock and unblock first in waiting list
 */
void lock_release(lock_t *l) {
    if (l->q_head != NULL) {    //unblock first in waiting queue
        unblock(&(l->q_head));
    }
    else { //no entries in waiting queue, release the lock
        l->lock_status = UNLOCKED;
    }
}