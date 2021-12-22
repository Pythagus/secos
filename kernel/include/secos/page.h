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

// Size of the PGD area.
#define PAGINATION_BASE_SZE 0x010000

// PGD base for the Kernel pagination.
#define PAGINATION_BASE_KRN 0x350000

// PGD base for the User pagination.
#define PAGINATION_BASE_USR (PAGINATION_BASE_KRN + PAGINATION_BASE_SZE)

// Get the PGD base.
#define pg_base_usr(__i__) (PAGINATION_BASE_USR + __i__ * PAGINATION_BASE_SZE)

// Get the address of the first PGD.
#define pg_pde(__base__) ((pde32_t *) (__base__))

// Get the address of the i-th PGD entry.
#define pg_pte(__base__, __i__) ((pte32_t *) (__base__ + 1024 * PGD_SIZE + __i__ * 1024 * PTE_SIZE))

//=================================//
//========== STACK MACRO ==========//
//=================================//

// Physical stack base addr for users.
#define PAGINATION_STACKS_BASE 0x900000

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
