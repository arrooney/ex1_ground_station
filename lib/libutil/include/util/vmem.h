#ifndef UTIL_VMEM_H_
#define UTIL_VMEM_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __AVR__
typedef uint32_t vmemptr_t;
#define VMEM_NULL ((vmemptr_t) (unsigned int) NULL)
#else
typedef uintptr_t vmemptr_t;
#define VMEM_NULL ((vmemptr_t) NULL)
#endif

typedef struct vmem_s {
	const char *const name;
	vmemptr_t start;
	vmemptr_t physmem_start;
	const uint32_t length;
	void * (*const write_mem)(void *, const void *, size_t size);
	void * (*const read_mem)(void *, const void *, size_t size);
} vmem_t;

/* The vmem_map struct must be declared by the user somewhere else */
extern const vmem_t vmem_map[];

#define VMEM_CPY(to, from, size) vmem_cpy((vmemptr_t) to, (vmemptr_t) from, size)

vmemptr_t vmem_cpy(vmemptr_t to, const vmemptr_t from, size_t size);
void vmem_list(void);

#endif /* UTIL_VMEM_H_ */
