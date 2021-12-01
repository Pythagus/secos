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
    pde32_t * pgd = (pde32_t *) pg_pgd(PAGINATION_KERNEL_BASE) ;

    // Clean the memory area.
    memset(pgd, 0, PAGINATION_AREA_SIZE) ;

    // Prepare all PGD entries.
    for(uint32_t i = 0 ; i < PAGINATION_NBR_PGD ; i++) {
        ptb = (pte32_t *) pg_pte(PAGINATION_KERNEL_BASE, i) ;
        pg_set_entry(pgd + i, PG_KRN|PG_RW, page_nr(ptb)) ;

        for(uint32_t j = 0 ; j < PAGINATION_NBR_PTE ; j++) {
            pg_set_entry(ptb + j, PG_KRN|PG_RW, j) ;
        }
    }

    // Start pagination.
    set_cr3(pg_pgd(PAGINATION_KERNEL_BASE)) ;
    set_cr0(get_cr0() | CR0_PG) ;
}

/**
 * Prepare the user pages.
 *
 * @param index
 * @return the base
 */
uint32_t page_user_init(uint8_t index) {
    // User PGD base.
    uint32_t base = pg_base(index) ;

    //pde32_t * pgd = (pde32_t *) pg_pgd(base) ;
    //uint32_t pd_index = pd32_idx(PAGINATION_USER_VIRTUAL_BASE) ;

    // Clean the memory area.
    //memset(pgd, 0, PAGINATION_AREA_SIZE) ;

    // Prepare PGD entries.
    //pg_set_entry(pgd + pd_index, PG_KRN|PG_RW, page_nr(PAGINATION_USER_VIRTUAL_BASE)) ;
    //page_translate(base, pg_user_base(index), PAGINATION_USER_VIRTUAL_BASE, PG_USR|PG_RW) ;
    /*for(uint32_t i = 0 ; i < PAGINATION_NBR_PTE ; i++) {
        pg_set_entry(ptb + i, PG_KRN|PG_RW, page_nr(PAGINATION_USER_VIRTUAL_BASE) + i) ;
    }*/

    return base ;
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

    pte32_t * ptb = (pte32_t *) pg_pte(base, pd_index) ;
    pg_set_entry(ptb + pt_index, attributes, page_nr(physical)) ;
}
