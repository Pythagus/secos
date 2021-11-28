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
#include <secos/task.h>

#define TASK_ARRAY_SIZE 16

typedef struct t_task t_task ;
struct t_task {
    // Is the task present?
    uint8_t present:1 ;

    // Has the task been run at least once?
    uint8_t executed:1 ;

    // Stack variables.
    uint32_t esp, ebp, eip ;

    // Pagination credentials.
    uint32_t pgd_base ;

    // Next task to be run.
    t_task * next ;
} ;

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
 */
void task_add(void * main) {
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
            panic("[ERROR] Cannot add another task.\n") ; return ;
        }
    }

    addr->present  = 1 ;
    addr->executed = 0 ;
    addr->eip  = (uint32_t) main ;

    // TODO : init esp and ebp, and a page.

    // Set the task at the end of the list.
    t_task * tmp    = last_task->next ;
    last_task->next = addr ;
    last_task  = addr ;
    addr->next = tmp ;

    // If no task was added yet.
    if(current_task == NULL) {
        current_task = addr ;
    }
}

/**
 * Function executed each time the
 * hardware timer IRQ0 generates
 * an interruption.
 *
 * @return void
 */
void irq0_timer_callback() {
    printf("Je switch : %x\n", current_task->eip) ;
    current_task->executed = 1 ;

    // TODO : PUSH registers in stack.

    current_task = current_task->next ;
    if(current_task->executed) {
        // TODO : POP registers from stack.
    } else {
        printf("Don't POP for %x\n", current_task->eip) ;
    }

    // Send an ACK to the timer.
    outb(PIC1, PIC_EOI) ;
    force_interrupts_on() ;
}

/**
 * Initialize the tasks management.
 *
 * @return void
 */
void task_initialize() {
    for(int i = 0 ; i < TASK_ARRAY_SIZE ; i++) {
        memset(tasks + i, 0, sizeof(t_task)) ;
    }
}
