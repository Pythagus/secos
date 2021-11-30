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
 * Initialize the kernl pages.
 *
 * @return void
 */
void page_kernel_init() {
    uint32_t table_addr = pg_pte(PAGINATION_KERNEL_BASE, 0) ;
    pde32_t * pgd = (pde32_t *) pg_pgd(PAGINATION_KERNEL_BASE) ;
    pte32_t * ptb = (pte32_t *) table_addr ;

    // Clean the memory area.
    memset(pgd, 0, PAGINATION_NBR_PGD * PGD_SIZE) ;

    // Prepare the Kernel pages.
    pg_set_entry(pgd, PG_KRN|PG_RW, table_addr >> PG_4K_SHIFT) ;
    for(uint32_t i = 0 ; i < PAGINATION_NBR_PTE ; i++) {
        pg_set_entry(ptb + i, PG_KRN|PG_RW, i) ;
    }

    // Start pagination.
    //set_cr3(pg_pgd(PAGINATION_KERNEL_BASE)) ;
    //set_cr0(get_cr0() | CR0_PG) ;
}

uint32_t page_user_init(uint8_t index) {
    uint32_t base  = pg_base(index) ;
    /*uint32_t table_addr = pg_pte(base, 0) ;
    pde32_t * pgd = (pde32_t *) pg_pgd(base) ;
    pte32_t * pte = (pte32_t *) table_addr ;


    // TODO : memset(0) ?

    pg_set_entry(pgd, PG_KRN|PG_RW, table_addr >> PG_4K_SHIFT) ;
    for(uint32_t i = 0 ; i < PAGINATION_NBR_PTE ; i++) {
        // TODO : check
        //pg_set_entry(pte + i, PG_KRN|PG_RW, PAGINATION_KERNEL_BASE + i * 0x1000) ;
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
    int pd_index = pd32_idx(virtual) ;
    int pt_index = pt32_idx(virtual) ;

    pte32_t * ptb = (pte32_t *) pg_pte(base, pd_index) ;
    pg_set_entry(ptb + pt_index, attributes, page_nr(physical)) ;
}
