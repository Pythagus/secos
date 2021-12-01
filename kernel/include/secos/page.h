/**
 * Manage the pagination system
 * to secure the OS.
 *
 * @author Damien MOLINA
 * @ðate 2021-11-26
 * @see https://wiki.osdev.org/Paging
 */

#ifndef SECOS_PAGE_H
#define SECOS_PAGE_H

#include <cr.h>
#include <pagemem.h>

// Size of the PGD struct.
#define PGD_SIZE sizeof(pde32_t)

// Size of the PTE struct.
#define PTE_SIZE sizeof(pte32_t)

// How many PDT a PGD can have.
#define PAGINATION_NBR_PTE 1024

// How many PGD there is in an area.
#define PAGINATION_NBR_PGD 1024

// Size of a full PGD entry (i.e. table).
#define PGD_ENTRY_SIZE (PAGINATION_NBR_PTE * PTE_SIZE)

// Size of a full pagination area.
#define PAGINATION_AREA_SIZE (PAGINATION_NBR_PGD * (PGD_SIZE + PAGINATION_NBR_PTE * PTE_SIZE))

// Get the PGD base address.
#define pg_pgd(__base__) (__base__)

// Get the PTE base address.
#define pg_pte(__base__, __pgd__) (__base__ + PAGINATION_NBR_PGD * PGD_SIZE + __pgd__ * PGD_ENTRY_SIZE)

// Get the PGD base.
#define pg_base(__i__) (PAGINATION_KERNEL_BASE + __i__ * PAGINATION_AREA_SIZE)

// Where the Kernel pagination table starts.
#define PAGINATION_KERNEL_BASE 0x4000000

// Base of the virtual memory area for user processes.
#define PAGINATION_USER_VIRTUAL_BASE 0x700000

// Max size of the user processes' virtual memory area.
#define PAGINATION_USER_VIRTUAL_SIZE 0x10000

// Address of the base user process memory area.
#define pg_user_base(__i__) (PAGINATION_USER_VIRTUAL_BASE + __i__ * PAGINATION_USER_VIRTUAL_SIZE)

/**
 * Initialize the kernel pages.
 *
 * @return void
 */
void page_kernel_init() ;

/**
 * Prepare the user pages.
 *
 * @param index
 * @return the base
 */
uint32_t page_user_init(uint8_t index) ;

/**
 * Add a page translation for the given
 * addresses.
 *
 * @param base
 * @param physical
 * @param virtual
 * @param attributes
 */
void page_translate(uint32_t base, uint32_t physical, uint32_t virtual, int attributes) ;

#endif
