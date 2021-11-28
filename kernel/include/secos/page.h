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
#define PAGINATION_AREA_SIZE (PAGINATION_NBR_PGD * PGD_SIZE + PAGINATION_NBR_PTE * PTE_SIZE)

// Get the PGD base address.
#define pg_pgd(__base__) (__base__)

// Get the PTE base address.
#define pg_pte(__base__, __pgd__) (__base__ + PAGINATION_NBR_PGD * PGD_SIZE + __pgd__ * PGD_ENTRY_SIZE)

/**
 * Initialize the kernel pages.
 *
 * @return void
 */
void page_kernel_init() ;


#endif
