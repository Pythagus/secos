/* GPLv2 (c) Airbus */
#include <info.h>
#include <segmem.h>
#include <secos/gdt.h>
#include <intr.h>
#include <debug.h>

extern info_t * info ;

tss_t tss ;

void syscall_isr() {
    __asm__ volatile ("leave") ;
    __asm__ volatile ("MOV %ebx, %esi") ;
    __asm__ volatile ("pusha") ;
    __asm__ volatile ("mov %esp, %eax") ;
    __asm__ volatile ("call syscall_handler") ;
    __asm__ volatile ("popa") ;
    __asm__ volatile ("iret") ;
}

void __regparm__(1) syscall_handler(int_ctx_t *ctx) {
    printf("SYSCALL eax = %p\n", ctx->gpr.eax);
    printf("SYSCALL esi = %s, %x\n", ctx->gpr.esi, ctx->gpr.esi);
}

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

/**
 * Handle the interruption 17. This
 * interruption is thrown when a
 * userland app finished and the
 * kernel must take the leadership
 * again.
 *
 * @return void
 */
// TODO


void prepare_user_land() {
    // Add int48 handler.
    idt_reg_t idtr ;
    get_idtr(idtr) ;
    int_desc_t * desc_48 = &idtr.desc[48] ;
    int_desc(&idtr.desc[48], desc_48->selector, (offset_t) syscall_isr) ;
    desc_48->dpl = RING_3 ;

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
    set_ds(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3)) ;
    set_es(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3)) ;
    set_fs(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3)) ;
    set_gs(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3)) ;

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
    gdt_seg_init(GDT_CODE_RO_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_KRN | GDT_G) ;
    gdt_seg_init(GDT_DATA_R0_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_KRN | GDT_G) ;
    gdt_seg_init(GDT_CODE_R3_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_USR | GDT_G) ;
    gdt_seg_init(GDT_DATA_R3_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_USR | GDT_G) ;

    // Set the CPU's registers value.
    set_ss(gdt_seg_sel(GDT_DATA_R0_SEG, RING_0)) ; // Stack Segment (SP)
    set_ds(gdt_seg_sel(GDT_DATA_R0_SEG, RING_0)) ; // Data Segment (DS)
    set_es(gdt_seg_sel(GDT_DATA_R0_SEG, RING_0)) ;
    set_fs(gdt_seg_sel(GDT_DATA_R0_SEG, RING_0)) ;
    set_gs(gdt_seg_sel(GDT_DATA_R0_SEG, RING_0)) ;
    set_cs(gdt_seg_sel(GDT_CODE_RO_SEG, RING_0)) ;

    gdt_display() ;
    prepare_user_land() ;
    // gdt_display() ;
}
