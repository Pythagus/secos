/**
 * Manage the system calls.
 *
 * @author Damien MOLINA
 * @date 2021-11-26
 */

#ifndef SECOS_SYSCALL_H
#define SECOS_SYSCALL_H

#include <intr.h>

/**
 * Interruption handler to execute
 * the system call.
 *
 * @return void
 */
void syscall_isr() ;

#endif
