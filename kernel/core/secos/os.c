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
    gdt_seg_init(GDT_CODE_R0_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_KRN | GDT_G) ;
    gdt_seg_init(GDT_DATA_R0_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_KRN | GDT_G) ;
    gdt_seg_init(GDT_CODE_R3_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_USR | GDT_G) ;
    gdt_seg_init(GDT_DATA_R3_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_USR | GDT_G) ;
    gdt_prepare_tss() ;

    // Initializing the IDT.
    idt_set_handler(IDT_SYSCALL_INDEX, syscall_isr, RING_3) ;
    idt_set_handler(IDT_IRQ0_INDEX, irq0_timer_callback,  RING_0) ;

    // Initialize the multitasking.
    task_initialize() ;

    // Initialize Kernel pagination.
    page_kernel_init() ;
}

/**
 * Add a task to the OS.
 *
 * @param main
 */
void secos_add_task(void * main) {
    // Add the task to the array.
    t_task * task = task_add((uint32_t) main) ;
    page_user_init(task->pgd_base) ;

    // Make the pagination.
    page_translate(task->pgd_base, task->eip, task->eip, PG_USR|PG_RW) ; // Code page.
    page_translate(task->pgd_base, task->stack_krn_ebp, task->stack_krn_ebp, PG_KRN|PG_RW) ; // Code page.
    page_translate(task->pgd_base, 0x302010, 0x302010, PG_KRN|PG_RO) ; // Code page.
}

/**
 * Start the OS.
 *
 * @return void
 */
void secos_start() {
    // Set the user segments.
    set_ds(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3)) ;
    set_es(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3)) ;
    set_fs(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3)) ;
    set_gs(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3)) ;

    // Start pagination.
    set_cr3(PAGINATION_BASE_KRN) ;
    set_cr0(get_cr0() | CR0_PG) ;

    // Start the OS scheduler.
    force_interrupts_on() ;

    while(1) ;
}
