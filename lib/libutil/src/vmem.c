#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <util/vmem.h>
#include <util/log.h>

void vmem_list(void) {
	const vmem_t * mem = vmem_map;
	while(mem->length != 0) {
		printf("%s \t0x%lx 0x%lx %"PRIu32"\r\n", mem->name, mem->start, mem->physmem_start, mem->length);
		mem++;
	}
}

vmemptr_t vmem_cpy(vmemptr_t to, const vmemptr_t from, size_t size) {

	/* Search memories */
	const vmem_t *vmem_from = NULL, *vmem_to = NULL;
	const vmem_t *mem = vmem_map;
	while(mem->length != 0) {
		//printf("0x%lx 0x%lx %"PRIu32" %lu %lu\r\n", mem->start, mem->physmem_start, mem->length, to, from);
		if ((to >= mem->start) && (to < mem->start + mem->length))
			vmem_to = mem;
		if ((from >= mem->start) && (from < mem->start + mem->length))
			vmem_from = mem;
		mem++;
	}

	/* If copying from RAM to RAM use memcpy */
	if ((vmem_to == NULL) && (vmem_from == NULL)) {
		return (vmemptr_t) (uintptr_t) memcpy((void *) (uintptr_t) to, (const void *) (uintptr_t) from, size);

	/* If copying from VMEM to VMEM, use RAM as temp variable */
	} else if ((vmem_to) && (vmem_from)) {
		printf("Currently MEM to MEM is not supported\r\n");
		return (vmemptr_t) (uintptr_t) NULL;

	/* If copying from RAM to MEM */
	} else if (vmem_to != NULL) {
		if (vmem_to->write_mem == NULL) {
			printf("This mem does not support writing\r\n");
			return (vmemptr_t) (uintptr_t) NULL;
		}
		vmemptr_t physaddr = (to - vmem_to->start) + vmem_to->physmem_start;
		//printf("Copying from ram 0x%lx to physaddr 0x%lx %u\r\n", from, physaddr, (unsigned int) size);
		return (vmemptr_t) (uintptr_t) (*vmem_to->write_mem)((void *) (uintptr_t) physaddr, (const void *) (uintptr_t) from, size);

	/* If copying from MEM to RAM */
	} else if (vmem_from != NULL) {
		if (vmem_from->read_mem == NULL) {
			printf("This mem does not support reading\r\n");
			return (vmemptr_t) (uintptr_t) NULL;
		}
		vmemptr_t physaddr = (from - vmem_from->start) + vmem_from->physmem_start;
		//printf("Copying from mem physaddr 0x%lx to 0x%lx %u\r\n", physaddr, to, (unsigned int) size);
		return (vmemptr_t) (uintptr_t) (*vmem_from->read_mem)((void *) (uintptr_t) to, (const void *) (uintptr_t)physaddr, size);
	}

	return to;
}
