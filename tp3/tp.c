/* GPLv2 (c) Airbus */
#include <info.h>
#include <segmem.h>
#include <secos/gdt.h>
#include <intr.h>
#include <debug.h>

extern info_t * info ;

tss_t tss ;

// 0x304277
void userland() {
    __asm__ volatile ("MOV $0xDD, %eax") ;

    while(1) ;
    //__asm__ volatile ("RET") ;
    //__asm__ volatile ("INT $17") ;
}

/**
 * Handle the interruption 17. This
 * interruption is thrown when a
 * userland app finished and the
 * kernel must take the leadership
 * again.
 *
 * @return void
 */
ATTR_NAKED void int17_handler() {
    __asm__ volatile ("MOV $0xBB, %eax") ;
}

void prepare_user_land() {
    // Add int17 handler.
    idt_reg_t idtr ;
    get_idtr(idtr) ;
    int_desc(&idtr.desc[17], gdt_seg_sel(GDT_USER_CODE_SEG, RING_3), (offset_t) int17_handler) ;
    set_idtr(idtr) ;

    // Preparing TSS segment in GDTR.
    raw32_t tss_addr = { .raw = (offset_t) &tss } ;
    seg_desc_t * tss_desc = gdt_seg_init(GDT_TSS_SEG, SEG_DESC_SYS_TSS_AVL_32, 0, 0xFFFFF, RING_0) ;
    tss_desc->raw    = sizeof(tss_t) ;
    tss_desc->base_1 = tss_addr.wlow ;
    tss_desc->base_2 = tss_addr._whigh.blow ;
    tss_desc->base_3 = tss_addr._whigh.bhigh ;
    tss_desc->type   = SEG_DESC_SYS_TSS_AVL_32 ;
    tss_desc->p      = 1 ;

    // Preparing Task State Segment (TSS).
    tss.s0.esp = get_ebp() ;
    tss.s0.ss  = gdt_seg_sel(2, 0) ;
    set_tr(gdt_seg_sel(5, 0)) ;

    // Loading the registers.
    set_ds(gdt_seg_sel(GDT_USER_DATA_SEG, RING_3)) ;
    set_es(gdt_seg_sel(GDT_USER_DATA_SEG, RING_3)) ;
    set_fs(gdt_seg_sel(GDT_USER_DATA_SEG, RING_3)) ;
    set_gs(gdt_seg_sel(GDT_USER_DATA_SEG, RING_3)) ;

    void * userland_ptr = userland;
    __asm__ volatile ("mov %esp, %eax");
    __asm__ volatile ("PUSHl %0" :: "i"(gdt_seg_sel(4, 3))) ; // DS (SS)
    __asm__ volatile ("PUSHl %eax") ; // ESP
    __asm__ volatile ("PUSHF") ; // E flags.
    __asm__ volatile ("PUSHl %0" :: "i"(gdt_seg_sel(3, 3))) ; // CS
    __asm__ volatile ("PUSHl %%ebx"::"b"(userland_ptr)) ; // EIP
    __asm__ volatile ("IRET") ;
}

void tp() {
    gdt_init() ;

    // Kernel code segment.
    gdt_seg_init(GDT_KERNEL_CODE_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_KRN|GDT_G) ;

    // Kernel data segment.
    gdt_seg_init(GDT_KERNEL_DATA_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_KRN|GDT_G) ;

    // Userland code segment.
    gdt_seg_init(GDT_USER_CODE_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_USR|GDT_G) ;

    // Userland data segment.
    gdt_seg_init(GDT_USER_DATA_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_USR|GDT_G) ;

    // Set the CPU's registers value.
    set_ss(gdt_seg_sel(GDT_KERNEL_DATA_SEG, RING_0)) ; // Stack Segment (SP)
    set_ds(gdt_seg_sel(GDT_KERNEL_DATA_SEG, RING_0)) ; // Data Segment (DS)
    set_es(gdt_seg_sel(GDT_KERNEL_DATA_SEG, RING_0)) ;
    set_fs(gdt_seg_sel(GDT_KERNEL_DATA_SEG, RING_0)) ;
    set_gs(gdt_seg_sel(GDT_KERNEL_DATA_SEG, RING_0)) ;
    set_cs(gdt_seg_sel(GDT_KERNEL_CODE_SEG, RING_0)) ;

    gdt_display() ;
    prepare_user_land() ;
   // gdt_display() ;
}
