/* GPLv2 (c) Airbus */
#include <info.h>
#include <debug.h>
#include <segmem.h>
#include <secos/gdt.h>
#include <secos/idt.h>
#include <secos/syscall.h>

extern info_t * info ;

// 0x304277
void userland() {
    __asm__ volatile ("MOV $0xDD, %eax") ;

    // Génère une GP. IDT non définie ou autorisée seulement au Kernel ?
    char * esi = "CCC" ;
    __asm__ volatile ("INT $48"::"a"(0xABCDEF), "b"(esi)) ;

    seg_desc_t * desc = gdt_seg_get(0) ;
    printf("COUCOUUU : %x\n", desc->limit_1) ;
    __asm__ volatile ("INT $48"::"a"(0xABCDEF), "b"(desc->limit_1)) ;

    while(1) ;
}

void prepare_user_land() {
    // Add int48 handler.
    idt_set_handler_intr(48, syscall_isr, RING_3) ;

    // Loading the registers.
    set_ds(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3)) ;
    set_es(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3)) ;
    set_fs(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3)) ;
    set_gs(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3)) ;

    void * userland_ptr = userland ;
    __asm__ volatile ("MOV %esp, %eax");
    __asm__ volatile ("PUSHl %0" :: "i"(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3))) ; // DS (SS)
    __asm__ volatile ("PUSHl %eax") ; // ESP
    __asm__ volatile ("PUSHF") ; // E flags.
    __asm__ volatile ("PUSHl %0" :: "i"(gdt_seg_sel(GDT_CODE_R3_SEG, RING_3))) ; // CS
    __asm__ volatile ("PUSHl %%ebx"::"b"(userland_ptr)) ; // EIP
    __asm__ volatile ("IRET") ;
}

void tp() {
    gdt_init() ;
    gdt_seg_init(GDT_CODE_R0_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_KRN | GDT_G) ;
    gdt_seg_init(GDT_DATA_R0_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_KRN | GDT_G) ;
    gdt_seg_init(GDT_CODE_R3_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_USR | GDT_G) ;
    gdt_seg_init(GDT_DATA_R3_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_USR | GDT_G) ;
    gdt_prepare_tss() ;

    // Set the CPU's registers value.
    set_ss(gdt_seg_sel(GDT_DATA_R0_SEG, RING_0)) ; // Stack Segment (SP)
    set_ds(gdt_seg_sel(GDT_DATA_R0_SEG, RING_0)) ; // Data Segment (DS)
    set_es(gdt_seg_sel(GDT_DATA_R0_SEG, RING_0)) ;
    set_fs(gdt_seg_sel(GDT_DATA_R0_SEG, RING_0)) ;
    set_gs(gdt_seg_sel(GDT_DATA_R0_SEG, RING_0)) ;
    set_cs(gdt_seg_sel(GDT_CODE_R0_SEG, RING_0)) ;

    gdt_display() ;
    prepare_user_land() ;
    // gdt_display() ;
}
