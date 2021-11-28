/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <cr.h>
#include <pagemem.h>
#include <secos/page.h>

#define PGD_KRN 0x600000

// @see https://wiki.osdev.org/Paging
extern info_t * info ;

void question_1() {
    printf("CR3 = %x\n", get_cr3()) ;
}

// Set the CR3 value.
void question_2() {
    set_cr3(pg_pgd(PGD_KRN)) ;
    printf("CR3 = %x\n", get_cr3()) ;
}

// Activate pagination.
// @see https://en.wikipedia.org/wiki/Control_register#CR0
void question_3() {
    set_cr0(get_cr0() | CR0_PG) ;
    // C'est cassé (pour Q3)
}

void question_4() {
    int table_addr = pg_pte(PGD_KRN, 0) ;
    pde32_t * pgd  = (pde32_t *) pg_pgd(PGD_KRN) ;
    pte32_t * pte  = (pte32_t *) table_addr ;

    // Prepare the first page.
    for(uint32_t i = 0 ; i < 1024 ; i++) {
        pg_set_entry(pte + i, PG_KRN|PG_RW, i) ;
    }

    // Set the first entry in the PGD for the PDT.
    pg_set_entry(pgd, PG_KRN|PG_RW, table_addr >> PG_4K_SHIFT) ;
}

void question_5() {
    printf("===> QUESTION 5\n") ;

    int table_addr = pg_pte(PGD_KRN, 0) ;
    pte32_t * pte  = (pte32_t *) table_addr ;

    printf("ptb->addr = %d\n", pte[6].addr) ;
    // Génère une Page Fault... :(
}

void question_6() {
    int table_addr = pg_pte(PGD_KRN, 1) ;
    pde32_t * pgd  = (pde32_t *) pg_pgd(PGD_KRN) ;
    pte32_t * pte  = (pte32_t *) table_addr ;

    for(uint32_t i = 0 ; i < 1024 ; i++) {
        pg_set_entry(pte + i, PG_KRN|PG_RW, i + 1024) ;
    }

    pg_set_entry(pgd + 1, PG_KRN|PG_RW, table_addr >> PG_4K_SHIFT) ;
}

/*
void question_7() {
    pde32_t * pgd  = (pde32_t *) pgd_entry_addr(0) ;
    pte32_t * ptb3 = (pte32_t *) pgd_entry_addr(3) ;

    // @see https://wiki.osdev.org/Paging#Manipulation
    uint32_t * virtual_addr = (uint32_t *) 0xc0000000 ;
    int pd_index = pd32_idx(virtual_addr) ;
    int pt_index = pt32_idx(virtual_addr) ;

    pg_set_entry(ptb3, 0, 0) ;
    pg_set_entry(&ptb3[pt_index], PG_KRN|PG_RW, page_nr(pgd)) ;
    pg_set_entry(&pgd[pd_index], PG_KRN|PG_RW, page_nr(ptb3)) ;

    debug("PGD = %p, target = %p\n", pgd[0].raw, *virtual_addr) ;
}

void question_8() {
    uint32_t * virtual_1 = (uint32_t *) 0x700000 ;
    uint32_t * virtual_2 = (uint32_t *) 0x7FF000 ;
    uint32_t target = 0x2000 ;

    int pt_index_1 = pt32_idx(virtual_1) ;
    int pt_index_2 = pt32_idx(virtual_2) ;

    pte32_t * ptb2 = (pte32_t *) pgd_entry_addr(0) ;
    pg_set_entry(&ptb2[pt_index_1], PG_KRN|PG_RW, target >> PG_4K_SHIFT) ;
    pg_set_entry(&ptb2[pt_index_2], PG_KRN|PG_RW, target >> PG_4K_SHIFT) ;

    printf("%x pointe sur %x\n", virtual_1, *virtual_1) ;
    printf("%x pointe sur %x\n", virtual_2, *virtual_2) ;
}

void question_9() {
    pde32_t * pgd  = (pde32_t *) pgd_entry_addr(0) ;
    char * virtual = (char *) 0x700000 ;

    pgd[0].p = 0 ;

    *virtual = 'A' ;
    printf("%x pointe sur %x\n", (void *) virtual, *virtual) ;
}*/

void tp() {
    question_1() ;
    question_2() ;
    question_4() ;
    question_6() ;
    question_3() ;
    question_5() ;
    //question_7() ;
    //question_8() ;
    //question_9() ;
}
