// define types for unsigned ints
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

// char type doesnt matter, we simply want the addresses of the symbols
extern char __bss[], __bss_end[], __stack_top[];

// sets a block of memory, starting at buf and ending at n to a specified value, c
void *memset(void *buf, char c, size_t n) {
	uint8_t *p = (uint8_t *) buf;
	while (n--)
		*p++=c;
	return buf;
}

// main code of the kernel
void kernel_main(void) {
	// initialize the bss section to 0
	memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);
	// busy waits (this broke deepseeks ai LMAO)
	for (;;);
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
