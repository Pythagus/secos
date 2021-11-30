/**
 * The main entry features for
 * our wonderful SecOS.
 *
 * @author Damien MOLINA
 * @date 2021-11-28
 */

#include <asm.h>
#include <print.h>
#include <secos/gdt.h>
#include <secos/idt.h>
#include <secos/task.h>
#include <secos/page.h>
#include <secos/syscall.h>

/**
 * Initialize the SecOS main features
 * required to be working.
 *
 * @return void
 */
void secos_initialize() {
    // We don't want to be interrupted.
    force_interrupts_off() ;

    // Initializing the segmentation.
    gdt_init() ;
    gdt_seg_init(GDT_CODE_RO_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_KRN | GDT_G) ;
    gdt_seg_init(GDT_DATA_R0_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_KRN | GDT_G) ;
    gdt_seg_init(GDT_CODE_R3_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_USR | GDT_G) ;
    gdt_seg_init(GDT_DATA_R3_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_USR | GDT_G) ;
    gdt_prepare_tss() ;

    // Initializing the IDT.
    idt_set_handler(IDT_SYSCALL_INDEX, syscall_isr, RING_3) ;
    idt_set_handler(IDT_IRQ0_INDEX, irq0_timer_callback,  RING_0) ;

    // Start the multitasking.
    task_initialize() ;

    // Start pagination.
    page_kernel_init() ;

    // Really start the OS.
    force_interrupts_on() ;
}

/**
 * Start the OS. This function should not
 * be called, but pagination failed to be
 * as "real" as possible. So we need to
 * call this function at tp.c.
 *
 * @return void
 */
void secos_start() {
    set_cr3(pg_pgd(PAGINATION_KERNEL_BASE)) ;
    set_cr0(get_cr0() | CR0_PG) ;
}

/**
 * Add a task to the OS.
 *
 * @param main
 */
void secos_add_task(void * main) {
    force_interrupts_off() ;

    uint32_t pid = task_add(main) ;
    page_user_init(pid) ;

    /*
    uint32_t base = pg_base(pid) ;
    int ptb_addr  = pg_pte(PAGINATION_KERNEL_BASE, 10) ;
    page_translate(PAGINATION_KERNEL_BASE, base, ptb_addr, PG_KRN|PG_RW) ;*/

    force_interrupts_on() ;
}