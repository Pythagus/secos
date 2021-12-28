/**
 * Manage all the tasks the kernel
 * need to treat.
 *
 * @author Damien MOLINA
 * @date 2021-11-25
 */

#include <io.h>
#include <pic.h>
#include <types.h>
#include <debug.h>
#include <string.h>
#include <secos/gdt.h>
#include <secos/task.h>
#include <secos/page.h>

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

extern tss_t tss ;

/**
 * Add a new task to the array.
 *
 * @param main
 * @return the task struct pointer.
 */
t_task * task_add(uint32_t main) {
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

    addr->present  = 1 ;
    addr->executed = 0 ;
    addr->eip = main ;

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
void irq0_timer_callback() {
    outb(PIC_EOI, PIC1) ;

    if(current_task != NULL) {
        printf("Je switch : %x (task %d)\n", current_task->eip, current_task->pid) ;

        if(current_task->executed) {
            // Save context values.
            __asm__ volatile("mov 4(%%ebp), %%eax" : "=a"(current_task->eip)) ;

            // Go to the next task.
            current_task = current_task->next ;
        }

        current_task->executed = 1 ;

        void * eip = (void *) current_task->eip ;

        set_cr3(current_task->pgd_base) ;
        tss.s0.esp = current_task->stack_krn_esp ;

        __asm__ volatile ("MOV %esp, %eax") ;
        __asm__ volatile ("PUSHl %0" :: "i"(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3))) ; // DS (SS)
        __asm__ volatile ("PUSHl %eax") ; // ESP
        __asm__ volatile ("PUSHF") ; // E flags.
        __asm__ volatile ("PUSHl %0" :: "i"(gdt_seg_sel(GDT_CODE_R3_SEG, RING_3))) ; // CS
        __asm__ volatile ("PUSHl %%ebx"::"b"(eip)) ; // EIP
        __asm__ volatile ("IRET") ;
    }
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
        task->executed = 0 ;
        task->pgd_base = pg_base_usr(i) ;

        // Prepare the stack.
        task->stack_usr_ebp = pg_stack_usr(i) ;
        task->stack_usr_esp = task->stack_usr_ebp ;
        task->stack_krn_ebp = pg_stack_krn(i) ;
        task->stack_krn_esp = task->stack_krn_ebp ;
    }
}
