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
    pde32_t * pgd = pg_pde(PAGINATION_BASE_KRN) ;
    pte32_t * ptb = pg_pte(PAGINATION_BASE_KRN, 0) ;

    // Clean up the memory area.
    memset(pgd, 0, PAGE_SIZE) ;

    for(uint32_t i = 0 ; i < PTE32_PER_PT ; i++) {
        pg_set_entry(ptb + i, PG_KRN|PG_RW, i) ;
    }

    pg_set_entry(pgd, PG_KRN|PG_RW, page_nr(ptb)) ;
}

/**
 * Prepare the user pages.
 *
 * @param base
 */
void page_user_init(uint32_t base) {
    pde32_t * pgd  = pg_pde(base) ;
    pde32_t * pgd2 = pgd + 2 ;
    pte32_t * ptb0 = pg_pte(base, 0) ;
    pte32_t * ptb2 = pg_pte(base, 2) ;

    // Clean up the memory area.
    memset(pgd, 0, PAGE_SIZE) ;
    memset(pgd2, 0, PAGE_SIZE) ;

    for(uint32_t i = 0 ; i < PTE32_PER_PT ; i++) {
        pg_set_entry(ptb0 + i, PG_USR|PG_RW, i) ;
    }

    for(uint32_t i = 0 ; i < PTE32_PER_PT ; i++) {
        pg_set_entry(ptb2, PG_USR|PG_RW, i) ;
    }

    pg_set_entry(pgd, PG_USR|PG_RW, page_nr(ptb0)) ;
    pg_set_entry(pgd2, PG_USR|PG_RW, page_nr(ptb2)) ;
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
    pte32_t * ptb     = pg_pte(base, pd_index) ;

    pg_set_entry(ptb + pt_index, attributes, page_nr(physical)) ;
}
