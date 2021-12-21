/**
 * Manage all the tasks the kernel
 * need to treat.
 *
 * @author Damien MOLINA
 * @date 2021-11-25
 */

#include <io.h>
#include <pic.h>
#include <asm.h>
#include <types.h>
#include <debug.h>
#include <string.h>
#include <secos/gdt.h>
#include <secos/task.h>
#include <secos/page.h>

#define TASK_ARRAY_SIZE 16

/*
 * The task array containing all the
 * tasks' data.
 */
t_task tasks[TASK_ARRAY_SIZE] ;

/*
 * Determine what is the current
 * task index in the tasks array.
 */
t_task * current_task = NULL ;

/*
 * Determine the end of the task
 * scheduler to be able to add
 * a new task.
 */
t_task * last_task = NULL ;

/**
 * Add a new task to the array.
 *
 * @param main
 * @return the task struct pointer.
 */
t_task * task_add(void * main) {
    t_task * addr = NULL ;

    // If there is no tasks.
    if(last_task == NULL) {
        addr       = tasks ;
        addr->next = addr ;
        last_task  = addr ;
    } else {
        for(int i = 0 ; i < TASK_ARRAY_SIZE ; i++) {
            if(! tasks[i].present) {
                addr = tasks + i ;
                break ;
            }
        }

        if(addr == NULL) {
            panic("[ERROR] Cannot add another task.\n") ; return NULL ;
        }
    }

    addr->present   = 1 ;
    addr->executed  = 0 ;
    addr->code_addr = (uint32_t) ((uint32_t *) main) ;
    addr->eip = addr->code_addr ;

    // Set the task at the end of the list.
    t_task * tmp    = last_task->next ;
    last_task->next = addr ;
    last_task  = addr ;
    addr->next = tmp ;

    // If no task was added yet.
    if(current_task == NULL) {
        current_task = addr ;
    }

    return addr ;
}

/**
 * Function executed each time the
 * hardware timer IRQ0 generates
 * an interruption.
 *
 * @param ctx
 */
void irq0_timer_callback(int_ctx_t * ctx) {
    if(current_task != NULL) {
        //printf("Je switch : %x, %x\n", current_task->eip, ctx->eip) ;

        if(current_task->executed) {
            // Save context values.
            current_task->ebp = ctx->gpr.ebp.raw ;
            current_task->esp = ctx->gpr.esp.raw ;
            current_task->eip = ctx->eip.raw ;

            /* // Save ALU flags.
             __asm__ volatile ("PUSHF") ;

             // Save global registers values.
             __asm__ volatile ("PUSHL %0" :: "b"(ctx->gpr.eax)) ;
             __asm__ volatile ("PUSHL %0" :: "b"(ctx->gpr.ebx)) ;
             __asm__ volatile ("PUSHL %0" :: "b"(ctx->gpr.ecx)) ;
             __asm__ volatile ("PUSHL %0" :: "b"(ctx->gpr.edx)) ;
             __asm__ volatile ("PUSHL %0" :: "b"(ctx->gpr.esi)) ;
             __asm__ volatile ("PUSHL %0" :: "b"(ctx->gpr.edi)) ;*/

            // Go to the next task.
            current_task = current_task->next ;
/*
            // If executed, restore the CPU/ALU registers.
            if(current_task->executed) {
                // Set the ALU flags.
                __asm__ volatile ("POPF") ;

                // Set the global registers values.
                __asm__ volatile ("POPL %eax") ;
                __asm__ volatile ("POPL %ebx") ;
                __asm__ volatile ("POPL %ecx") ;
                __asm__ volatile ("POPL %edx") ;
                __asm__ volatile ("POPL %esi") ;
                __asm__ volatile ("POPL %edi") ;
            }*/
        }

        current_task->executed = 1 ;

        // Restore the pagination area.
        //printf("WAITING : %x\n", current_task->pgd_base) ;

        // 0x301fa0
        set_cr3(current_task->pgd_base) ;

        // Prepare to jump to the next task.
        void * task_ptr = (void *) current_task->eip ;
        __asm__ volatile ("MOV %esp, %eax");
        __asm__ volatile ("PUSHl %0" :: "i"(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3))) ; // DS (SS)
        __asm__ volatile ("PUSHl %eax") ; // ESP
        __asm__ volatile ("PUSHF") ; // E flags.
        __asm__ volatile ("PUSHl %0" :: "i"(gdt_seg_sel(GDT_CODE_R3_SEG, RING_3))) ; // CS
        __asm__ volatile ("PUSHl %%ebx"::"b"(task_ptr)) ; // EIP
        __asm__ volatile ("IRET") ;
    }

    // Send an ACK to the timer.
    outb(PIC_EOI, PIC1) ;
    force_interrupts_on() ;
}

/**
 * Initialize the tasks management.
 *
 * @return void
 */
void task_initialize() {
    t_task * task ;

    for(int i = 0 ; i < TASK_ARRAY_SIZE ; i++) {
        task = tasks + i ;

        memset(task, 0, sizeof(t_task)) ;
        task->pid = i + 1 ;
        task->present  = 0 ;
        task->pgd_base = pg_base_usr(i) ;

        // Prepare the stack.
        task->stack_usr_addr = pg_stack_usr(i) ;
        task->stack_krn_addr = pg_stack_krn(i) ;
        task->ebp = task->stack_usr_addr ;
        task->esp = task->ebp ;
    }
}
