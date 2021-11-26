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

/**
 * Manage the system call checking the
 * registers values.
 *
 * @param ctx
 */
void __regparm__(1) syscall_handler(int_ctx_t * ctx) ;

#endif
