/**
 * Manage all the tasks the kernel
 * need to treat.
 *
 * @author Damien MOLINA
 * @date 2021-11-25
 */

#ifndef SECOS_TASK_H
#define SECOS_TASK_H

#include <intr.h>

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
 */
uint8_t task_add(void * main) ;

/**
 * Function executed each time the
 * hardware timer IRQ0 generates
 * an interruption.
 *
 * @return void
 */
void irq0_timer_callback(int_ctx_t * ctx) ;

#endif
