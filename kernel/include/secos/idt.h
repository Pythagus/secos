#ifndef SECOS_IDT_H
#define SECOS_IDT_H

// Basic handler indexes.
#define IDT_IRQ0_INDEX    32
#define IDT_SYSCALL_INDEX 48

/**
 * Set the handler as the new
 * interruption handler.
 *
 * @param index
 * @param handler
 * @param dpl
 */
void idt_set_handler(int index, void * handler, int dpl) ;

#endif
