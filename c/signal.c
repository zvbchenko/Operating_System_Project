/* signal.c - support for signal handling
   This file is not used until Assignment 3
 */

#include <xeroskernel.h>
#include <xeroslib.h>

/* Your code goes here */

	  // Trampoline for signalling 
void sigtramp(void(*handler)(void*), void* ctnx) {
	context_frame* frame = (context_frame*)ctnx;
	handler(frame);
	syssigreturn(frame->esp);
}
// Signalling. Returns error codes or 0 
int signal(pcb* target, int signalNum, void(*newHandler)(void *)){
	
	if (target == NULL) {
		return -514;
	}
	
	if (target->state == STATE_RECEIVE_SIGNAL) {
		target->ret = -666;	 //summon the devil
	}
	if (signalNum >31 || signalNum < 0) {
		return -583;
	}
	// sets the handler for the signal, saves the previous handle
	signal_frame *signalf = target->esp - sizeof(signal_frame); // saving the signal stack on top of the target process stack
	unsigned long savestackp = target->esp;
	signalf->oldsp = savestackp;
	signalf->esp = savestackp;
	signalf->ret = target->ret;
	signalf->handler = newHandler;
	target->esp = savestackp - sizeof(signal_frame);
	ready(target);

	return 0;
}