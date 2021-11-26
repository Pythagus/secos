/**
 * Manage all the tasks the kernel
 * need to treat.
 *
 * @author Damien MOLINA
 * @date 2021-11-25
 */

#ifndef SECOS_TASK_H
#define SECOS_TASK_H

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
void task_add(void * main) ;

/**
 * Switch to the next task in the
 * tasks array.
 *
 * @return void
 */
void task_switch_next() ;

#endif
