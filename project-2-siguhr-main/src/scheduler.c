/*  scheduler.c
 */
#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "util.h"

// Call scheduler to run the 'next' process
void yield(void) 
{
    
    if (current_running->pid >= 2) 
    { // timing of threads
       
        context_switch1();
        scheduler_entry();
        context_switch2();
    }
    else 
    { //timing of processes in entry.S
        scheduler_entry();
    }
}

/* The scheduler picks the next job to run, and removes blocked and exited
 * processes from the ready queue, before it calls dispatch to start the
 * picked process.
 */
void scheduler(void) 
{
    // if exited, remove current process form ready queue
    if (current_running->state == STATUS_EXITED) 
    {
        current_running->previous->next = current_running->next;        // put current running (exited) task's (*next address) as previous task's next address
        current_running->next->previous = current_running->previous;    // put current running (exited) task's (*previous address) as next task's previous address
    }  

    // remove current process from the ready queue
    if (current_running->state == STATUS_BLOCKED) 
    {
        current_running->previous->next = current_running->next;        // put current running (blocked) task's (*next address) as previous task's next address
        current_running->next->previous = current_running->previous;    // put current running (blocked) task's (*previous address) as next task's previous address
        current_running = current_running->next;                        // set the current running (blocked) task's (*next address) as the current running task
    } 

    else
    {
        current_running = current_running->next;                        // if no state call, current running is just next in queue
    }
    if (current_running->state == STATUS_FIRST_TIME)                    // start process/thread task
    {
        dispatch();
    }
    
   
}

void context_switch1() 
{
    time1 = get_timer();
}

void context_switch2() {
    time2 = get_timer();
    
    print_str(9, 72, "Time:");
    print_str(9, 54, ">-pid-switch->");
    print_int(current_running->pid+10, 53, current_running->previous->pid);
    print_int(current_running->pid+10, 68, current_running->pid);
    print_int(current_running->pid+10, 73, (time2-time1));
    

    
}

/* dispatch() does not restore gpr's it just pops down the kernel_stack,
 * and returns to whatever called scheduler (which happens to be scheduler_entry,
 * in entry.S).
 */

void dispatch(void) 
{
    if (current_running->pid < NUM_PROCS)           // if current_running is process 
    {
        current_running->state = STATUS_READY;      // and process is in ready queue
        process();                                  // move user pointer to %esp register

        current_running->state = STATUS_FIRST_TIME; // and process is first time
        process();
    }
    else                                            // if current_running is thread
    {
        current_running->state = STATUS_READY;      // and thread is in ready queue
        thread();                                   // move kernel pointer to %esp register
        
        current_running->state = STATUS_FIRST_TIME; // and thread is first time
        thread(); 
    }
}

/* Remove the current_running process from the linked list so it
 * will not be scheduled in the future
 */
void exit(void) 
{
    current_running->state = STATUS_EXITED;
    yield();
    
}

/* 'q_head' is a pointer to the waiting list where current_running should be
 * inserted
 */
void block(pcb_t **q_head) 
{

    current_running->state = STATUS_BLOCKED;    // block current running
    if(*q_head == NULL)
    {
        *q_head = current_running;              // add current running as queue head
    }
    else
    {
        pcb_t *q_tail;
        q_tail = *q_head;                       
        while (q_tail->queue != NULL)            // search queue to find tail
        {
            q_tail = q_tail->queue;
        }
        q_tail->queue = current_running;        // set current running at the end of queue
    }
    current_running->queue = NULL;              // current running should be last in queue
    yield();
}

/* Must be called within a critical section.
 * Unblocks the first process in the waiting queue (q_head), (*q_head) points to the
 * last process.
 */
void unblock(pcb_t **q_head) 
{
  (*q_head)->state = STATUS_READY;              // unblock q_head state in queue

  (*q_head)->next = current_running->next;      // set q_head as next in (ready queue)
  current_running->next = *q_head;
  (*q_head)->previous = current_running;        // set q_head previous as currently running in (ready queue)

  *q_head = (*q_head)->queue;                // set q_head to next in queue
}
