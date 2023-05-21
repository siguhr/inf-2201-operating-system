/*  kernel.c
 */
#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "th.h"
#include "util.h"

// Statically allocate some storage for the pcb's
pcb_t pcb[NUM_TOTAL];
// Ready queue and pointer to currently running process
pcb_t *current_running;

/* This is the entry point for the kernel.
 * - Initialize pcb entries
 * - set up stacks
 * - start first thread/process
 */

uint64_t time1;
uint64_t time2;

void _start(void) {
	/* Declare entry_point as pointer to pointer to function returning void
	 * ENTRY_POINT is defined in kernel h as (void(**)()0xf00)
	 */

	//clear_screen(0, 0, 80, 25);
	// while (1)
	// 	;

	void (**entry_point)() = ENTRY_POINT;
	
	// print_int(10,10,100);
	// HALT("Hello") ;

	// load address of kernel_entry into memory location 0xf00
	*entry_point = kernel_entry;

	set_address();
	init_pcb();
	current_running = &pcb[0];
	context_switch1();
	dispatch();

	
	
}

/*  Helper function for kernel_entry, in entry.S. Does the actual work
 *  of executing the specified syscall.
 */

void kernel_entry_helper(int fn) 
{
	if (fn == 0) // yield syscall
	{
		yield();
	}
	if (fn == 1) // exit syscall
	{
		exit();
	}

}

// Function for address array
void set_address()
	{
		pcb[0].address = PROC1_ADDR;
		pcb[1].address = PROC2_ADDR;
		pcb[2].address = &clock_thread;
		pcb[3].address = &thread2;
		pcb[4].address = &thread3;
		pcb[5].address = &mcpi_thread0;
		pcb[6].address = &mcpi_thread1;
		pcb[7].address = &mcpi_thread2;
		pcb[8].address = &mcpi_thread3;
 	}


/*setting up PCB */
void init_pcb()
{	
	for (int i = 0; i < NUM_TOTAL; i++) 
	{	
		// properties given to both process and threads
		pcb[i].pid = i;										// set id
		pcb[i].state = STATUS_FIRST_TIME;					// set state 

		if (i < 2) 											// if process
		{
			pcb[i].ker_p = (STACK + 2*i*STACK_SIZE);		// setup kernel stack pointer 
			pcb[i].use_p = (STACK + (2*i+1)*STACK_SIZE); 	// setup user stack pointer
		}
		else												// if thread
		{
			pcb[i].ker_p = (STACK + (2+i)*STACK_SIZE);		// setup kernel stack pointer
		}
		// connect the end to start (next) of address 
		if (i==(8)) 										// if pid is end of list
		{
			pcb[i].next = &pcb[0];							// set next pcb as first pcb
		} 
		else 
		{
			pcb[i].next = &pcb[i+1];						
		}
		// connect start to end (previous) of address 
		if(i == 0)
		{
			pcb[i].previous = &pcb[8];						// connet first pcb's (previous) 
		}													// to last address
		else
		{
			pcb[i].previous = &pcb[i-1];
		}
	}
}





