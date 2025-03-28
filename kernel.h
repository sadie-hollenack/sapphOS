#include "common.h"
#pragma once

#define PROCS_MAX 8 // Max amount of processes

#define PROC_UNUSED 0 // unused process control structure
#define PROC_RUNNABLE 1 // runnable process

// Macros for paging
#define SATP_SV32 (1u << 31) // sets a single bit in the satp register to enable Sv32 mode
#define PAGE_V (1 << 0) // "Valid" bit (entry is enabled)
#define PAGE_R (1 << 1) // Readable
#define PAGE_W (1 << 2) // Writable
#define PAGE_X (1 << 3) // Executable
#define PAGE_U (1 << 4) // User (accessible in user mode)

#define PANIC(fmt, ...)                                                   \
    do {                                                                       \
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);  \
        while (1) {}                                                           \
    } while (0)

#define READ_CSR(reg) ({                               \
    unsigned long __tmp;                                \
    __asm__ __volatile__("csrr %0, " #reg : "=r"(__tmp));\
    __tmp;                                                \
})

#define WRITE_CSR(reg, value)                               \
    do {                                                     \
        uint32_t __tmp = (value);                             \
        __asm__ __volatile__("csrw " #reg ", %0" ::"r"(__tmp));\
    } while(0)

struct process {
    int pid;
    int state; // can either be PROC_UNUSED or PROC_RUNNABLE
    vaddr_t sp; // points to the top of this processes sp
    uint32_t *page_table; // pointer to the first elvel page table
    uint8_t stack[8192]; // the stack of the process
};

struct sbiret {
    long error;
    long value;
};

struct trap_frame {
    uint32_t ra;
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t sp;
} __attribute__((packed));