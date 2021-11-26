#include <asm.h>
#include <info.h>
#include <debug.h>
#include <secos/gdt.h>
#include <secos/idt.h>
#include <secos/task.h>
#include <secos/syscall.h>

extern info_t * info ;

void userland_1() {
    printf("COUCOU 1\n") ;
    //while(1) ;
}

void userland_2() {
    printf("COUCOU 2\n") ;
    //while(1) ;
}

void tp() {
    // Initializing the segmentation.
    gdt_init() ;
    gdt_seg_init(GDT_CODE_RO_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_KRN | GDT_G) ;
    gdt_seg_init(GDT_DATA_R0_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_KRN | GDT_G) ;
    gdt_seg_init(GDT_CODE_R3_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_USR | GDT_G) ;
    gdt_seg_init(GDT_DATA_R3_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_USR | GDT_G) ;
    gdt_prepare_tss() ;

    // Initializing the IDT.
    idt_set_handler(IDT_SYSCALL_INDEX, syscall_isr, RING_3) ;

    // Initialize tasking management.
    task_initialize() ;
    task_add(&userland_1) ;
    task_add(&userland_2) ;
    task_start_scheduling() ;

    // Enable interruptions.
    force_interrupts_on() ;

    while(1) ;
}
