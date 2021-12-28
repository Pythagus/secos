/**
 * The main entry features for
 * our wonderful SecOS.
 *
 * @author Damien MOLINA
 * @date 2021-11-28
 */

#ifndef SECOS_OS_H
#define SECOS_OS_H

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
 */
void secos_add_task(void * main) ;

/**
 * Start the OS.
 *
 * @return void
 */
void secos_start() ;

#endif
