/**
 * The main entry features for
 * our wonderful SecOS.
 *
 * @author Damien MOLINA
 * @date 2021-11-28
 */

#ifndef SECOS_OS_H
#define SECOS_OS_H

#include <secos/task.h>

// Specify a function section to force the
// compilation address of the function.
#define ATTR_SECTION(__section__) __attribute__((section(__section__)))

/**
 * Initialize the SecOS main features
 * required to be working.
 *
 * @return void
 */
void secos_initialize() ;

/**
 * Add a task to the OS.
 *
 * @param main
 * @return the task structure.
 */
t_task * secos_add_task(void * main) ;

/**
 * Translate addresses for the given task.
 *
 * @param task
 * @param physical
 * @param virtual
 */
void secos_translate(t_task * task, uint32_t physical, uint32_t virtual) ;

/**
 * Start the OS.
 *
 * @return void
 */
void secos_start() ;

#endif
