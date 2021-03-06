/* xeroskernel.h - disable, enable, halt, restore, isodd, min, max */

#ifndef XEROSKERNEL_H
#define XEROSKERNEL_H

/* Symbolic constants used throughout Xinu */

typedef	char    Bool;        /* Boolean type                  */
typedef unsigned int size_t; /* Something that can hold the value of
                              * theoretical maximum number of bytes 
                              * addressable in this architecture.
                              */
#define	FALSE   0       /* Boolean constants             */
#define	TRUE    1
#define	EMPTY   (-1)    /* an illegal gpq                */
#define	NULL    0       /* Null pointer for linked lists */
#define	NULLCH '\0'     /* The null character            */

#define CREATE_FAILURE -1  /* Process creation failed     */

#define ENTER_KEY "\n"
#define EOT 0x04
#define KINT 1
/* Universal return constants */

#define	OK            1         /* system call ok               */
#define	SYSERR       -1         /* system call failed           */
#define	EOF          -2         /* End-of-file (usu. from read)	*/
#define	TIMEOUT      -3         /* time out  (usu. recvtim)     */
#define	INTRMSG      -4         /* keyboard "intr" key pressed	*/
                                /*  (usu. defined as ^B)        */
#define	BLOCKERR     -5         /* non-blocking op would block  */

/* Functions defined by startup code */


void           bzero(void *base, int cnt);
void           bcopy(const void *src, void *dest, unsigned int n);
void           disable(void);
unsigned short getCS(void);
unsigned char  inb(unsigned int);
void           init8259(void);
int            kprintf(char * fmt, ...);
void           lidt(void);
void           outb(unsigned int, unsigned char);


/* Some constants involved with process creation and managment */ 
   /* Maximum number of processes */      
#define MAX_PROC        64           
   /* Kernel trap number          */
#define KERNEL_INT      80
   /* Interrupt number for the timer */
#define TIMER_INT      (TIMER_IRQ + 32)
   /* Minimum size of a stack when a process is created */
#define PROC_STACK      (4096 * 4)    
              
   /* Number of milliseconds in a tick */
#define MILLISECONDS_TICK 10        


/* Constants to track states that a process is in */
#define STATE_STOPPED   0
#define STATE_READY     1
#define STATE_SLEEP     22
#define STATE_RUNNING   23
#define STATE_RECEIVE_SIGNAL 43
/* System call identifiers */
#define SYS_STOP        10
#define SYS_YIELD       11
#define SYS_CREATE      22
#define SYS_TIMER       33
#define SYS_GETPID      144
#define SYS_PUTS        155
#define SYS_SLEEP       166
#define SYS_CPUTIMES    178
#define SYS_SIGNAL      200
#define SYS_SIGRETURN   300
#define SYS_SIGKILL		350
#define SYS_WAIT		400
#define SYS_OPEN	    777
#define SYS_CLOSE		778
#define SYS_WRITE		779
#define SYS_READ		780
#define SYS_IOCTL		781
// for Keyboards
#define ECHO 1
#define NONECHO 0
// 
#define IGNORE 3
#define ACTUPON 4
#define DEVICE_OPEN 5
#define DEVICE_CLOSED 6
#define KINT 1
/* Structure to track the information associated with a single process */
// NEW Signal table for process (psigt)

typedef struct struct_psigt psigt;
struct struct_psigt {
	void* handler;
	void **oldhandler;
};
typedef struct signal_frame {
	int ret;
	unsigned long esp;
	unsigned long oldsp;
	int mask;		 // 1 - ignore, 0 - act 
	void* handler;
} signal_frame;

typedef struct fdt_entry {
	int dvnum;
	char *dvname;
	//int(*dvinit)();
	int	(*dvopen)(void*,...);
	int(*dvclose)(void*, ...);
	int(*dvread)(void*, ...);
	int(*dvwrite)(void*, ...);
	//int(*dvseek)();
	//int(*dvgetc)();
	//int(*dvputc)();
	int(*dvcntl)(void*, ...);
	//void *dvcsr;
	// void *dvivec;
	//void *dvovec;
	//int(*dviint)();
	//int(*dvoint)();
	//void *dvioblk;
	//int dvminor;
}fdt_entry;





typedef struct file_descriptor{
	int major;
	int minor;
	char* name;
	int status;
	fdt_entry* entry;
}file_descriptor;



typedef struct struct_pcb pcb;
struct struct_pcb {
  void        *esp;    /* Pointer to top of saved stack           */
  pcb         *next;   /* Next process in the list, if applicable */
  pcb         *prev;   /* Previous proccess in list, if applicable*/
  int          state;  /* State the process is in, see above      */
  unsigned int pid;    /* The process's ID                        */
  int          ret;    /* Return value of system call             */
                       /* if process interrupted because of system*/
                       /* call                                    */
  long         args;   
  unsigned int otherpid;
  void        *buffer;
  int          bufferlen;
  int          sleepdiff;
  long         cpuTime;  /* CPU time consumed                     */
  psigt	   signaltable[32];// one psigt per signals
  file_descriptor files[4];
};


//  Kernel's device table
extern fdt_entry DEVICE_TABLE[2];

typedef struct struct_ps processStatuses;
struct struct_ps {
  int  entries;            // Last entry used in the table
  int  pid[MAX_PROC];      // The process ID
  int  status[MAX_PROC];   // The process status
  long  cpuTime[MAX_PROC]; // CPU time used in milliseconds
};


/* The actual space is set aside in create.c */
extern pcb     proctab[MAX_PROC];

#pragma pack(1)

/* What the set of pushed registers looks like on the stack */
typedef struct context_frame {
  unsigned long        edi;
  unsigned long        esi;
  unsigned long        ebp;
  unsigned long        esp;
  unsigned long        ebx;
  unsigned long        edx;
  unsigned long        ecx;
  unsigned long        eax;
  unsigned long        iret_eip;
  unsigned long        iret_cs;
  unsigned long        eflags;
  unsigned long        stackSlots[];
} context_frame;


/* Memory mangement system functions, it is OK for user level   */
/* processes to call these.                                     */

int      kfree(void *ptr);
void     kmeminit( void );
void     *kmalloc( size_t );


/* A typedef for the signature of the function passed to syscreate */
typedef void    (*funcptr)(void);


/* Internal functions for the kernel, applications must never  */
/* call these.                                                 */
void     dispatch( void );
void     dispatchinit( void );
void     ready( pcb *p );
pcb      *next( void );
void     contextinit( void );
int      contextswitch( pcb *p );
int      create( funcptr fp, size_t stack );
void     set_evec(unsigned int xnum, unsigned long handler);
void     printCF (void * stack);  /* print the call frame */
int      syscall(int call, ...);  /* Used in the system call stub */
void     sleep(pcb *, unsigned int);
void     tick( void );
void removeFromSleep(pcb * p);



/* Function prototypes for system calls as called by the application */
int          syscreate( funcptr fp, size_t stack );
void         sysyield( void );
void         sysstop( void );
unsigned int sysgetpid( void );
unsigned int syssleep(unsigned int);
void     sysputs(char *str);
//int syskill(int);
int sysgetcputimes(processStatuses *ps);
int syssighandler(int signal, void(*newhandler)(void *), void(** oldHandler)(void*));
void syssigreturn(void *old_sp);
int syswait(int PID);
void sigtramp(void(*handler)(void*), void* ctnx);
int signal(pcb* target, int signalNum, void(*newHandler)(void *));

int syskill(int PID, int signalNumber);

// DI_CALLS
void di_init(void);
int di_open(pcb* p, int device_no);
int di_close(pcb* p, int fdes);
int di_write(pcb* p, int fdes, unsigned char *buf, int size);
int di_read(pcb* p, int fdes, unsigned char *buf, int size);
int di_iotcl(pcb* p, int fdes, unsigned long command, ...);
// keyboard stuff
void enable_irq(unsigned int, int); // that's for kbd.c
unsigned int kbopen(pcb* p, int device_no, int major);
unsigned int kbclose(pcb* p, int major);
unsigned int kbread(pcb* p, int fdes, unsigned char *buf, int size);
unsigned int kbwrite(pcb* p);
unsigned int kbioctl(unsigned long command);
unsigned int kbreadint(void);
unsigned int kbtoa(unsigned char code);


/* The initial process that the system creates and schedules */
void     root( void );




void           set_evec(unsigned int xnum, unsigned long handler);


/* Anything you add must be between the #define and this comment */
#endif

