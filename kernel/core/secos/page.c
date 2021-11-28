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


    /*pde32_t * pgd_kernel = (pde32_t *) pgd_addr(0) ;


    // Prepare the memory area.
    memset(pgd_kernel, 0, PAGE_SIZE) ;
    set_cr3(pgd_kernel) ;

    pg_set_entry(pgd_kernel, PG_KRN | PG_RW, page_nr(pgd_entry_addr(0, 0))) ;
    for(int i = 0 ; i < PAGE_SIZE ; i++) {
        pg_set_entry() ;
    }

    pde32_t * pgd = pgd_entry(0) ;

    int pgd_index = 0 ;
    //pte32_t * pte = PGD_ADDR + pgd_index * PGD_FULL_SIZE +   ;
    pde32_t * pgd = PGD_ADDR + pgd_index * PGD_FULL_SIZE ;



    pg_set_entry(pgd, PG_KRN|PG_RW, page_nr(PTB_KRN));
    for(int i = 0 ; i < PAGE_SIZE ; i++) {
        pg_set_entry(&PTB_KRN[i], PG_KRN|PG_RW, i);
    }

    set_cr3() ;*/
}

