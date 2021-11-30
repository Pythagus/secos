/**
 * The main entry features for
 * our wonderful SecOS.
 *
 * @author Damien MOLINA
 * @date 2021-11-28
 */

#ifndef SECOS_OS_H
#define SECOS_OS_H

/**
 * Initialize the SecOS main features
 * required to be working.
 *
 * @return void
 */
void secos_initialize() ;

/**
 * Start the OS. This function should not
 * be called, but pagination failed to be
 * as "real" as possible. So we need to
 * call this function at tp.c.
 *
 * @return void
 */
void secos_start() ;

/**
 * Add a task to the OS.
 *
 * @param main
 */
void secos_add_task(void * main) ;

#endif
