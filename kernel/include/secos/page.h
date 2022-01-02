/**
 * Manage the pagination system
 * to secure the OS.
 *
 * @author Damien MOLINA
 * @date 2021-11-26
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
#define _pg_stack_th(__th__) (PAGINATION_STACKS_BASE + (__th__) * PAGE_SIZE + PAGE_SIZE - 1)

// Get the address of the i-th user stack.
#define pg_stack_usr(__i__) _pg_stack_th(2 * (__i__))

// Get the address of the i-th kernel stack.
#define pg_stack_krn(__i__) _pg_stack_th(2 * (__i__) + 1)

/**
 * Initialize the kernel pages.
 *
 * @return void
 */
#define page_kernel_init() ({ \
    pg_identity_map(PAGINATION_BASE_KRN, 0, PG_KRN|PG_RW) ; \
    pg_identity_map(PAGINATION_BASE_KRN, 2, PG_KRN|PG_RW) ; \
})

/**
 * Prepare the user pages.
 *
 * @param base
 */
#define page_user_init(__base__) ({  \
    pg_identity_map(__base__, 0, PG_USR|PG_RW) ; \
    pg_identity_map(__base__, 1, PG_USR|PG_RW) ; \
    pg_identity_map(__base__, 2, PG_USR|PG_RW) ; \
})

/**
 * Add a page translation for the given
 * addresses.
 *
 * @param base (virtual)
 * @param address
 * @param attributes
 */
#define page_translate_identity(__base__, __addr__, __attr__) (page_translate(__base__, __addr__, __addr__, __attr__))

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

/**
 * Identity map a full PTB.
 *
 * @param base
 * @param pgd_id
 * @param pg_attr
 */
void pg_identity_map(uint32_t base, uint32_t pgd_id, int pg_attr) ;

/**
 * Display the pagination.
 *
 * @param pgd
 * @param nbr_pgd
 * @param nbr_pde
 */
void page_display(pde32_t * pgd, uint32_t nbr_pgd, uint32_t nbr_pde) ;

#endif
