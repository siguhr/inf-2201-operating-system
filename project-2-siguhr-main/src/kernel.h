/* kernel.h
 *
 * Various definitions used by the kernel and related code.
 */
#ifndef KERNEL_H
#define KERNEL_H

#include "common.h"

/* Cast 0xf00 into pointer to pointer to function returning void
 * ENTRY_POINT is used in syslib.c to declare 'entry_point'
 */
#define ENTRY_POINT ((void (**)())0xf00)

// Constants
enum
{
	/* Number of threads and processes initially started by the kernel. Change
	 * this when adding to or removing elements from the start_addr array.
	 */
	NUM_THREADS = 7,
	NUM_PROCS = 2,
	NUM_TOTAL = (NUM_PROCS + NUM_THREADS),

	SYSCALL_YIELD = 0,
	SYSCALL_EXIT,
	SYSCALL_COUNT,

	//  Stack constants
	STACK_MIN = 0x10000,
	STACK_MAX = 0x20000,
	STACK_OFFSET = 0x0ffc,
	STACK_SIZE = 0x1000,

	STACK = (STACK_MIN + STACK_OFFSET), 	// start of stack location
};

// Typedefs

/* The process control block is used for storing various information about
 *  a thread or process
 */
typedef struct pcb_t {
	struct pcb_t *next; 		// Used to point to next task in ready queue
	struct pcb_t *previous;		// Used to point to previus task in ready queue

	void *address;				// memory location
	int pid;					// process/thread id
	int state; 					// state of process/thread
	uint32_t ker_p;				// kernel pointer
	uint32_t use_p;				// user pointer
		
	struct pcb_t *queue; 		// pointer to next pcb in waiting queue
	int *save_FPU[27];			// array to store floating point unit (minimum 27 byte)
} pcb_t;

// Variables

// The currently running process, and also a pointer to the ready queue
extern pcb_t *current_running;
extern uint64_t time1;
extern uint64_t time2;

// Prototypes
void init_pcb();
void set_address();
void kernel_entry(int fn);
void kernel_entry_helper(int fn);

#endif
