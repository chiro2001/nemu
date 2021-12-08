#ifndef __MEMORY_PADDR_H__
#define __MEMORY_PADDR_H__

#include <common.h>

// #define CONFIG_MBASE 0x80000000
// #define CONFIG_PC_RESET_OFFSET 0x0

#define PMEM_SIZE (256 * 1024 * 1024)
// #define RESET_VECTOR (CONFIG_MBASE + CONFIG_PC_RESET_OFFSET)

/* convert the guest physical address in the guest program to host virtual address in NEMU */
void* guest_to_host(paddr_t addr);
/* convert the host virtual address in NEMU to guest physical address in the guest program */
paddr_t host_to_guest(void *addr);

word_t paddr_read(paddr_t addr, int len);
void paddr_write(paddr_t addr, word_t data, int len);

#endif
