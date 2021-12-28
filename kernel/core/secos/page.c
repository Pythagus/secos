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

#define virtual_addr(__pgd_id__, __ptb_id__) ((__pgd_id__ << PG_4M_SHIFT) + (__ptb_id__ << PG_4K_SHIFT))

/**
 * Identity map a full PTB.
 *
 * @param base
 * @param pgd_id
 * @param pg_attr
 */
void pg_identity_map(uint32_t base, uint32_t pgd_id, int pg_attr) {
    pte32_t * ptb = pg_pte(base, pgd_id) ;
    pde32_t * pgd = pg_pde(base) ;
    pgd += pgd_id ;

    uint32_t virtual_base = (pgd_id << 10) ;

    // Clean up the memory area.
    memset(pgd, 0, PAGE_SIZE) ;

    for(uint32_t i = 0 ; i < PTE32_PER_PT ; i++) {
        pg_set_entry(ptb + i, pg_attr, virtual_base + i) ;
    }

    pg_set_entry(pgd, pg_attr, page_nr(ptb)) ;
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

/**
 * Display the pagination.
 *
 * @param pgd
 * @param nbr_pgd
 * @param nbr_pde
 */
void page_display(pde32_t * pgd, uint32_t nbr_pgd, uint32_t nbr_pde) {
    pte32_t * pte, * pte_i ;
    pde32_t * pgd_i ;
    uint32_t base = page_addr(page_nr(pgd)) ;
    uint32_t virtual ;

    for(uint32_t i = 0 ; i < nbr_pgd ; i++) {
        pgd_i = pgd + i ;
        pte = pg_pte(base, i) ;

        if(pgd_i->p) {
            printf("|-- Id --+-- Physical --+-- Virtual --| Entry %d\n", i) ;
            for(uint32_t j = 0 ; j < nbr_pde ; j++) {
                pte_i = pte + j ;
                virtual = virtual_addr(i, j) ;

                printf("|  %4x  | ", j) ;
                if(pte_i->p) {
                    printf("  0x%6x   |   0x%6x  | ", virtual, page_addr(pte_i->addr), pte) ;

                    if(pte_i->lvl == 0UL) {
                        printf("K\n") ;
                    } else {
                        printf("U\n") ;
                    }
                } else {
                    printf("NULL\n") ;
                }
            }
        } else {
            printf("|-- Entry %d null\n", i) ;
        }
    }

    printf("---------------------------------------\n") ;
}
