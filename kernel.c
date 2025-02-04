#include "kernel.h"
#include "common.h"

// define types for unsigned ints
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

// bss: section of the kernel stack of data with an inital value of zero
// char type doesnt matter, we simply want the addresses of the symbols
extern char __bss[], __bss_end[], __stack_top[];

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

// sets a block of memory, starting at buf and ending at n to a specified value, c


// main code of the kernel
void kernel_main(void) {
	// initialize the bss section to 0
	memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);
	
	printf("\n\nHello %s\n", "World!");
	printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);

	PANIC("booted!");
	printf("unreachable");
	// busy waits (this broke deepseeks ai LMAO)
	for (;;){
		// wait for interrupt
		__asm__ __volatile__("wfi");
	}
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
