#include "user.h"

extern char __stack_top[];

__attribute__((noreturn)) void exit(void) {
    // TODO
    for(;;);
}

void putchar(char ch) {
    /* TODO */
}

__attribute__((sections(".text.start")))
__attribute__((naked))
void start(void) {
    asmv(
        "mv sp, %[stack_top]\n"
        "call main\n"
        "call exit\n"
        :: [stack_top] "r" (__stack_top)
    );
}