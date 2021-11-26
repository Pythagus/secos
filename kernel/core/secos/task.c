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
#include <secos/idt.h>
#include <secos/gdt.h>
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
 * Determine the first task of
 * the array to make a rotation.
 */
t_task * first_task = NULL ;

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

    if(first_task == NULL) {
        addr       = tasks ;
        first_task = addr ;
        last_task  = addr ;
    }

    if(addr == NULL) {
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
    addr->next = first_task ;
    addr->eip  = (uint32_t) main ;
    // TODO : init esp and ebp, and a page.

    last_task->next = addr ;
}

/**
 * Function executed each time the
 * hardware timer IRQ0 generates
 * an interruption.
 *
 * @return void
 */
static void irq0_timer_callback() {
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
 * Start the task scheduler working
 * with the tasks array.
 *
 * @return void
 */
void task_start_scheduling() {
    if(first_task == NULL) {
        panic("[ERROR] No scheduled task to run") ;
        return ;
    }

    current_task = first_task ;
    idt_set_handler(IDT_IRQ0_INDEX, &irq0_timer_callback,  RING_0) ;
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
