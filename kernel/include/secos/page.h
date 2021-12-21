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

// Get the PGD base address.
#define pg_pgd(__base__) (__base__)

// Where the Kernel pagination table starts.
#define PAGINATION_BASE_KRN 0x1000000

// Where the User pagination table starts.
#define PAGINATION_BASE_USR 0x400000

// Physical stack base addr for users.
#define PAGINATION_STACKS_BASE 0x900000

//=================================//
//======= KERNEL PAGINATION =======//
//=================================//

// How many PDT a Kernel PGD can have.
#define PAGINATION_NBR_PTE_KRN 1024

// How many PGD there is in a kernel area.
#define PAGINATION_NBR_PGD_KRN 1024

// Size of a full PGD entry (i.e. table).
#define PGD_ENTRY_SIZE_KRN (PAGINATION_NBR_PTE_KRN * PTE_SIZE)

// Size of a full pagination area.
#define PAGINATION_AREA_SIZE_KRN (PAGINATION_NBR_PGD_KRN * (PGD_SIZE + PAGINATION_NBR_PTE_KRN * PTE_SIZE))

// Get the PTE base address.
#define pg_pte_krn(__pgd__) (PAGINATION_BASE_KRN + PAGINATION_NBR_PGD_KRN * PGD_SIZE + __pgd__ * PGD_ENTRY_SIZE_KRN)

//=================================//
//======== USER PAGINATION ========//
//=================================//

// How many PDT a user PGD can have.
#define PAGINATION_NBR_PTE_USR 20

// How many PGD there is in a user area.
#define PAGINATION_NBR_PGD_USR PAGINATION_NBR_PGD_KRN

// Size of a full PGD entry (i.e. table).
#define PGD_ENTRY_SIZE_USR (PAGINATION_NBR_PTE_USR * PTE_SIZE)

// Size of a full pagination area.
#define PAGINATION_AREA_SIZE_USR (PAGINATION_NBR_PGD_USR * (PGD_SIZE + PAGINATION_NBR_PTE_USR * PTE_SIZE))

// Get the PTE base address.
#define pg_pte_usr(__base__, __pgd__) (__base__ + PAGINATION_NBR_PGD_USR * PGD_SIZE + __pgd__ * PGD_ENTRY_SIZE_USR)

// Get the PGD base.
#define pg_base_usr(__i__) (PAGINATION_BASE_USR + __i__ * PAGINATION_AREA_SIZE_USR)

// Private macro to get the i-th stack.
#define _pg_stack_th(__th__) (PAGINATION_STACKS_BASE + __th__ * PAGE_SIZE + PAGE_SIZE - 1)

// Get the address of the i-th user stack.
#define pg_stack_usr(__i__) _pg_stack_th(2 * __i__)

// Get the address of the i-th kernel stack.
#define pg_stack_krn(__i__) _pg_stack_th(2 * __i__ + 1)

/**
 * Initialize the kernel pages.
 *
 * @return void
 */
void page_kernel_init() ;

/**
 * Prepare the user pages.
 *
 * @param base
 */
void page_user_init(uint32_t base) ;

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
