/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <intr.h>
#include <segmem.h>

#define get_eip(eip) __asm__ volatile("1: lea 1b, %0;": "=a"(eip))

extern info_t * info ;

ATTR_NAKED void bp_handler() {
    // pusha 
    // Do something
    // popa ; leave ; iret

    __asm__("PUSH %ebp") ;
    __asm__("MOV %esp, %ebp") ;

    uint32_t eip ;
    get_eip(eip) ;
    printf("%x DURING HANDLER\n", eip) ;
    printf("==============================> BP!\n") ;

    __asm__("MOV %ebp, %esp") ;
    __asm__("POP %ebp") ;
    __asm__("IRET") ;
}

void bp_trigger() {
    uint32_t eip ;
    get_eip(eip) ;
    printf("%x BEFORE INT3\n", eip) ;

    __asm__("int $3") ;

    get_eip(eip) ;
    printf("%x AFTER INT3\n", eip) ;
}

void tp() {
    idt_reg_t idtr ;
    get_idtr(idtr) ;
    int_desc(&idtr.desc[3], gdt_krn_seg_sel(1), (offset_t) bp_handler) ;
    set_idtr(idtr) ;

    printf("==============================> BEFORE BP!\n") ;
    bp_trigger() ;
    printf("==============================> AFTER BP!\n") ;
}
