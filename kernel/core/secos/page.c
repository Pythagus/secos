/**
 * Manage the pagination system
 * to secure the OS.
 *
 * @author Damien MOLINA
 * @ðate 2021-11-28
 * @see https://wiki.osdev.org/Paging
 */

#include <debug.h>
#include <secos/page.h>

/**
 * Initialize the kernel pages.
 *
 * @return void
 */
void page_kernel_init() {
    pte32_t * ptb ;
    pde32_t * pgd = (pde32_t *) pg_pgd(PAGINATION_BASE_KRN) ;

    // Clean the memory area.
    memset(pgd, 0, PAGINATION_AREA_SIZE_KRN) ;

    // Prepare all PGD entries.
    for(uint32_t i = 0 ; i < PAGINATION_NBR_PGD_KRN ; i++) {
        ptb = (pte32_t *) pg_pte_krn(i) ;
        pg_set_entry(pgd + i, PG_KRN|PG_RW, page_nr(ptb)) ;

        for(uint32_t j = 0 ; j < PAGINATION_NBR_PTE_KRN ; j++) {
            pg_set_entry(ptb + j, PG_KRN|PG_RW, j) ;
        }
    }
}

/**
 * Prepare the user pages.
 *
 * @param base
 */
void page_user_init(uint32_t base) {
    pte32_t * ptb = (pte32_t *) pg_pte_usr(base, 0) ;
    pde32_t * pgd = (pde32_t *) pg_pgd(base) ;

    printf("BASE  = %x (%x)\n", base, PAGINATION_AREA_SIZE_USR) ;
    printf("- pgd = %x\n", pgd) ;
    printf("- ptb = %x\n", ptb) ;

    // Clean the memory area.
    memset(pgd, 0, PAGINATION_AREA_SIZE_USR) ;

    // Prepare a single PGD.
    pg_set_entry(pgd, PG_USR|PG_RW, page_nr(ptb)) ;

    // Only prepare the first PGD entries.
    for(uint32_t j = 0 ; j < PAGINATION_NBR_PTE_USR ; j++) {
        pg_set_entry(ptb + j, PG_USR|PG_RW, j) ;
    }
}

/**
 * Add a page translation for the given
 * addresses.
 *
 * @param base (virtual)
 * @param physical
 * @param virtual
 * @param attributes
 */
void page_translate(uint32_t base, uint32_t physical, uint32_t virtual, int attributes) {
    // Important indexes to add page translation.
    uint32_t pd_index = pd32_idx(virtual) ;
    uint32_t pt_index = pt32_idx(virtual) ;

    pte32_t * ptb ;
    if((attributes & PG_USR) == PG_USR) {
        ptb = (pte32_t *) pg_pte_usr(base, pd_index) ;
    } else {
        ptb = (pte32_t *) pg_pte_krn(pd_index) ;
    }

    pg_set_entry(ptb + pt_index, attributes, page_nr(physical)) ;
}
