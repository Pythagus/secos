/**
 * Manage all the tasks the kernel
 * need to treat.
 *
 * @author Damien MOLINA
 * @date 2021-11-25
 */

#ifndef SECOS_TASK_H
#define SECOS_TASK_H

#define TASK_ARRAY_SIZE 3

#include <intr.h>

typedef struct t_task t_task ;
struct t_task {
    // Is the task present?
    uint8_t present:1 ;

    // Has the task been run at least once?
    uint8_t executed:1 ;

    // Stack Kernel addresses.
    uint32_t stack_krn_ebp ;
    uint32_t stack_krn_esp ;

    // Stack User addresses
    uint32_t stack_usr_esp ;
    uint32_t stack_usr_ebp ;

    // Code address.
    uint32_t eip ;

    // Pagination credentials.
    uint32_t pgd_base ;

    // Process id.
    uint8_t pid ;

    // Next task to be run.
    t_task * next ;
} ;

/**
 * Initialize the tasks management.
 *
 * @return void
 */
void task_initialize() ;

/**
 * Add a new task to the array.
 *
 * @param main
 * @return the task struct pointer.
 */
t_task * task_add(uint32_t main) ;

/**
 * Function executed each time the
 * hardware timer IRQ0 generates
 * an interruption.
 *
 * @return void
 */
void irq0_timer_callback() ;

#endif
