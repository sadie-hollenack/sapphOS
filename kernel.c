#include "kernel.h"
#include "common.h"

// define types for unsigned ints
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

// bss: section of the kernel stack of data with an inital value of zero
// char type doesnt matter, we simply want the addresses of the symbols
extern char __bss[], __bss_end[], __stack_top[], __free_ram[], __free_ram_end[];

// makes a call to the sbi allowing the user to make elevated calls
// function will change depending on what values are passed into fid and eid
struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid) {
	register long a0 __asm__("a0") = arg0;
	register long a1 __asm__("a1") = arg1;
	register long a2 __asm__("a2") = arg2;
	register long a3 __asm__("a3") = arg3;
	register long a4 __asm__("a4") = arg4;
	register long a5 __asm__("a5") = arg5;
	register long a6 __asm__("a6") = fid;
	register long a7 __asm__("a7") = eid;

	__asm__ __volatile__("ecall"
						: "=r"(a0), "=r"(a1)
						: "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)  
						: "memory");
	return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char ch) {
	// the 1 calls the sbi extension sbi_console_putchar(int ch)
	sbi_call(ch, 0, 0, 0, 0, 0, 0, 1);
}

// ERROR HANDLING
// stores all general registers then calls handle_trap
// after handle_trap executes it restores all registers
__attribute__((naked))
__attribute__((aligned(4)))
void kernel_entry(void) {
	__asm__ __volatile__(
		"csrw sscratch, sp\n" // stores the stack pointer in a csr (control and status register) 
							// sscratch is a special purpose register for storing the stack pointer
		"addi sp, sp, -4 * 31\n" // subtract 4 * 31 from the stack pointer to store words above it in memory
		"sw ra, 4 * 0(sp)\n" // store every register to the stack to preserve the state of the cpu
		"sw gp, 4 * 1(sp)\n"
		"sw tp, 4 * 2(sp)\n"
		"sw t0, 4 * 3(sp)\n"
		"sw t1, 4 * 4(sp)\n"
		"sw t2, 4 * 5(sp)\n"
		"sw t3, 4 * 6(sp)\n"
		"sw t4, 4 * 7(sp)\n"
		"sw t5, 4 * 8(sp)\n"
		"sw t6, 4 * 9(sp)\n"
		"sw a0, 4 * 10(sp)\n"
		"sw a1, 4 * 11(sp)\n"
		"sw a2, 4 * 12(sp)\n"
		"sw a3, 4 * 13(sp)\n"
		"sw a4, 4 * 14(sp)\n"
		"sw a5, 4 * 15(sp)\n"
		"sw a6, 4 * 16(sp)\n"
		"sw a7, 4 * 17(sp)\n"
		"sw s0, 4 * 18(sp)\n"
		"sw s1, 4 * 19(sp)\n"
		"sw s2, 4 * 20(sp)\n"
		"sw s3, 4 * 21(sp)\n"
		"sw s4, 4 * 22(sp)\n"
		"sw s5, 4 * 23(sp)\n"
		"sw s6, 4 * 24(sp)\n"
		"sw s7, 4 * 25(sp)\n"
		"sw s8, 4 * 26(sp)\n"
		"sw s9, 4 * 27(sp)\n"
		"sw s10, 4 * 28(sp)\n"
		"sw s11, 4 * 29(sp)\n"

		"csrr a0, sscratch\n" // save the orginal stack pointer to the stack
		"sw a0, 4 * 30(sp)\n"

		"mv a0, sp\n"
		"call handle_trap\n" 

		"lw ra, 4 * 0(sp)\n" // restore each register
		"lw gp, 4 * 1(sp)\n"
		"lw tp, 4 * 2(sp)\n"
		"lw t0, 4 * 3(sp)\n"
		"lw t1, 4 * 4(sp)\n"
		"lw t2, 4 * 5(sp)\n"
		"lw t3, 4 * 6(sp)\n"
		"lw t4, 4 * 7(sp)\n"
		"lw t5, 4 * 8(sp)\n"
		"lw t6, 4 * 9(sp)\n"
		"lw a0, 4 * 10(sp)\n"
		"lw a1, 4 * 11(sp)\n"
		"lw a2, 4 * 12(sp)\n"
		"lw a3, 4 * 13(sp)\n"
		"lw a4, 4 * 14(sp)\n"
		"lw a5, 4 * 15(sp)\n"
		"lw a6, 4 * 16(sp)\n"
		"lw a7, 4 * 17(sp)\n"
		"lw s0, 4 * 18(sp)\n"
		"lw s1, 4 * 19(sp)\n"
		"lw s2, 4 * 20(sp)\n"
		"lw s3, 4 * 21(sp)\n"
		"lw s4, 4 * 22(sp)\n"
		"lw s5, 4 * 23(sp)\n"
		"lw s6, 4 * 24(sp)\n"
		"lw s7, 4 * 25(sp)\n"
		"lw s8, 4 * 26(sp)\n"
		"lw s9, 4 * 27(sp)\n"
		"lw s10, 4 * 28(sp)\n"
		"lw s11, 4 * 29(sp)\n"
		"lw sp, 4 * 30(sp)\n" // restore the orginal stack pointer
		"sret \n"
		
	);
}

void handle_trap() {
	uint32_t scause = READ_CSR(scause); // the code to why the exception occured
	uint32_t stval = READ_CSR(stval); // additional info on the exception
	uint32_t user_pc = READ_CSR(sepc); // the address of the instructon that caused the exception

	PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval, user_pc);
}


// MEMORY MANAGEMENT
// only issue with this solution is that memory is not deallocated
paddr_t alloc_pages(uint32_t n) {
	static paddr_t next_paddr = (paddr_t) __free_ram;
	paddr_t paddr = next_paddr;
	next_paddr += n * PAGE_SIZE;

	if(next_paddr > (paddr_t) __free_ram_end) {
		PANIC("out of memory");
	}

	memset((void *) paddr, 0, n * PAGE_SIZE);
	return paddr;
}

// allocates all registers to the top of the process's stack
__attribute__((naked)) void switch_context(uint32_t *prev_sp, uint32_t *next_sp) {
	__asm__ __volatile__(
		// Save callee-saved register onto the current process's stack
		"addi sp, sp, -13 * 4\n" // Allocate stack space for 13 4-byte registers
        "sw ra,  0  * 4(sp)\n"   // Save callee-saved registers only
        "sw s0,  1  * 4(sp)\n"
        "sw s1,  2  * 4(sp)\n"
        "sw s2,  3  * 4(sp)\n"
        "sw s3,  4  * 4(sp)\n"
        "sw s4,  5  * 4(sp)\n"
        "sw s5,  6  * 4(sp)\n"
        "sw s6,  7  * 4(sp)\n"
        "sw s7,  8  * 4(sp)\n"
        "sw s8,  9  * 4(sp)\n"
        "sw s9,  10 * 4(sp)\n"
        "sw s10, 11 * 4(sp)\n"
        "sw s11, 12 * 4(sp)\n"

        // Switch the stack pointer.
        "sw sp, (a0)\n"         // *prev_sp = sp;
        "lw sp, (a1)\n"         // Switch stack pointer (sp) here

        // Restore callee-saved registers from the next process's stack.
        "lw ra,  0  * 4(sp)\n"  // Restore callee-saved registers only
        "lw s0,  1  * 4(sp)\n"
        "lw s1,  2  * 4(sp)\n"
        "lw s2,  3  * 4(sp)\n"
        "lw s3,  4  * 4(sp)\n"
        "lw s4,  5  * 4(sp)\n"
        "lw s5,  6  * 4(sp)\n"
        "lw s6,  7  * 4(sp)\n"
        "lw s7,  8  * 4(sp)\n"
        "lw s8,  9  * 4(sp)\n"
        "lw s9,  10 * 4(sp)\n"
        "lw s10, 11 * 4(sp)\n"
        "lw s11, 12 * 4(sp)\n"
        "addi sp, sp, 13 * 4\n"  // We've popped 13 4-byte registers from the stack
        "ret\n"
	);
}

// process initialization
struct process procs[PROCS_MAX];

struct process *create_process(uint32_t pc) {
	// find and unused process control structure
	struct process *proc = NULL;
	int i;
	for(i = 0; i < PROCS_MAX; i++) {
		if(procs[i].state == PROC_UNUSED) {
			proc = &procs[i];
			break;	
		}
	}

	if(!proc) {
		PANIC("no free process slots");
	}

	// stack callee-saved registers. These register values will be restored
	// in the first context switch in switch_context
	uint32_t *sp = (uint32_t *) &proc->stack[sizeof(proc->stack)]; // sets the stack pointer to the top of the stack
    *--sp = 0;                      // s11
    *--sp = 0;                      // s10
    *--sp = 0;                      // s9
    *--sp = 0;                      // s8
    *--sp = 0;                      // s7
    *--sp = 0;                      // s6
    *--sp = 0;                      // s5
    *--sp = 0;                      // s4
    *--sp = 0;                      // s3
    *--sp = 0;                      // s2
    *--sp = 0;                      // s1
    *--sp = 0;                      // s0
    *--sp = (uint32_t) pc;          // ra

	// Initialize fields
	proc->pid = i + 1;
	proc->state = PROC_RUNNABLE;
	proc->sp = (uint32_t) sp;
	return proc;
}

void delay(void) {
	for(int i = 0; i < 30000000; i++) {
		__asm__ __volatile__("nop"); // do nothing
	}
}
// TEST PROCESSES
struct process *proc_a;
struct process *proc_b;

void proc_a_entry(void) {
	printf("starting process A\n");
	while(1) {
		printf("fuck");
		switch_context(&proc_a->sp, &proc_b->sp);
		delay();
	}
}

void proc_b_entry(void) {
	printf("starting process B\n");
	while(1) {
		printf("shit");
		switch_context(&proc_b->sp, &proc_a->sp);
		delay();
	}
}

// main code of the kernel
void kernel_main(void) {
	// initialize the bss section to 0
	memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

	// tells the CPU where the exception handler is
	WRITE_CSR(stvec, (uint32_t) kernel_entry);
	
	paddr_t paddr0 = alloc_pages(2);
	paddr_t paddr1 = alloc_pages(1);
	
	printf("alloc_pages test: paddr0=%x\n", paddr0);
	printf("alloc_pages test: paddr1=%x\n", paddr1);

	printf("\n\nHello %s\n", "World!");
	printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);

	proc_a = create_process((uint32_t) proc_a_entry);
	proc_b = create_process((uint32_t) proc_b_entry);
	proc_a_entry();
	
	PANIC("unreachable here!");
}

__attribute__((section(".text.boot"))) // controls the placement of the function in the linker script
__attribute__((naked)) // adds no extra code to the function
// first thing to run, sets the stack pointer to end address 
// (i belive it is the 128kb from the linker script)
// then jumps to the kernel_main function
void boot(void) {
	__asm__ __volatile__(
		"mv sp, %[stack_top]\n"
		"j kernel_main\n"
		:
		: [stack_top] "r" (__stack_top)
	);
}