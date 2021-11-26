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
    __asm__ volatile ("leave") ;
    __asm__ volatile ("MOV %ebx, %esi") ;
    __asm__ volatile ("pusha") ;
    __asm__ volatile ("mov %esp, %eax") ;
    __asm__ volatile ("call syscall_handler") ;
    __asm__ volatile ("popa") ;
    __asm__ volatile ("iret") ;
}

/**
 * Manage the system call checking the
 * registers values.
 *
 * @param ctx
 */
void __regparm__(1) syscall_handler(int_ctx_t *ctx) {
    printf("SYSCALL eax = %p\n", ctx->gpr.eax);
    printf("SYSCALL esi = %s, %x\n", ctx->gpr.esi, ctx->gpr.esi);
}
