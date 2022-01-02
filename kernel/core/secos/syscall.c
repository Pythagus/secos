/**
 * Manage the system calls.
 *
 * @author Damien MOLINA
 * @date 2021-11-26
 */

#include <types.h>
#include <debug.h>
#include <secos/syscall.h>

/**
 * Interruption handler to execute
 * the system call.
 *
 * @return void
 */
void syscall_isr() {
    __asm__ volatile ("LEAVE") ;
    __asm__ volatile ("MOV %ebx, %esi") ;
    __asm__ volatile ("PUSHa") ;
    __asm__ volatile ("MOV %esp, %eax") ;
    __asm__ volatile ("CALL syscall_handler") ;
    __asm__ volatile ("POPa") ;
    __asm__ volatile ("IRET") ;
}

/**
 * Manage the system call checking the
 * registers values.
 *
 * @param ctx
 */
void __regparm__(1) syscall_handler(int_ctx_t * ctx) {
    printf("Value = %d\n", *((uint32_t *) ctx->gpr.eax.raw)) ;
}
