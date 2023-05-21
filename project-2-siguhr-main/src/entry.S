#	entry.S
#	This file contains the code for kernel_entry, as well as
#	scheduler_entry, which saves the context of the current process
#	before calling the scheduler. The file is pre-processed before it
#	is assembled, allowing (among other things) the use of macros
#	and C-style comments.

.data
	.align 4
.text
.code32
	.align 4

# Make symbols visible for ld
.globl	kernel_entry
.globl	scheduler_entry
.globl  thread
.globl  process

# void kernel_entry(int fn)
#
# SYSCALL(i) in syslib.c calls this function
# fn = system call number (defined in kernel.h)
#
ke_scratch:
	.long	0
ke_arg:
	.long	0
kernel_entry:
	call context_switch1
	
	# Save eax
	movl	%ebx, (ke_scratch)
	
	# Save argument
	movl 4(%esp), %ebx
	movl %ebx, (ke_arg)

	
	movl current_running, %ecx
	movl %esp, 24(%ecx)					# move (stack pointer) esp to user stack pointer
	movl 20(%ecx), %esp					# move kernel stack pointer to esp

	# push arguments (Yield, exit) to kernel_entry_helper
	pushl (ke_arg)						# push arguments
	call kernel_entry_helper            # call kernel_entry helper with argument
	addl $4, %esp						# rebuild esp

	movl current_running, %ecx			# moce current_running to edx
	movl %esp, 20(%ecx)					# move (stack pointer) esp to kernel stack pointer
	movl 24(%ecx), %esp					# move user stack pointer to esp

	call context_switch2

	ret


#	This function gets called to enter the scheduler, saving registers before doing so.
scheduler_entry:
	# Fill in the blanks!
	pushfl								# push EFLAGS
	pushal								# save GPRs

	movl current_running, %ebx			# move current_running to ebx
	fsave 32(%ebx)						# save FPU state
	movl %esp, 20(%ebx)					# move (stack pointer) esp to kernel stack pointer
	call scheduler						# call scheduler to fetch new task
	movl current_running, %ebx			# move current_running to ebx
	movl 20(%ebx), %esp					# move kernel stack pointer to esp
	frstor 32(%ebx)						# rebuild FPU state

	popal 								# rebuild GPRs 
	popfl 								# restore EFLAGS
	ret

thread:
	# jump to current_running thread address
	movl 20(%eax), %esp					# move kernel stack pointer to esp
	pushl 8(%eax)						# push current_running address
	ret

process:
	# jump to current_running procecc address
	movl 24(%eax), %esp					# move user stack pointer to esp
	pushl 8(%eax)						# push current_running address
	ret
