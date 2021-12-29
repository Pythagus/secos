#ifndef SECOS_IDT_H
#define SECOS_IDT_H

// Basic handler indexes.
#define IDT_IRQ0_INDEX    32
#define IDT_SYSCALL_INDEX 0x80

#define idt_set_handler_trap(__i__, __h__, __dpl__) (idt_set_handler(__i__, __h__, __dpl__, SEG_DESC_SYS_TRAP_GATE_32))
#define idt_set_handler_intr(__i__, __h__, __dpl__) (idt_set_handler(__i__, __h__, __dpl__, SEG_DESC_SYS_INTR_GATE_32))

/**
 * Set the handler as the new
 * interruption handler.
 *
 * @param index
 * @param handler
 * @param dpl
 * @param type
 */
void idt_set_handler(int index, void * handler, int dpl, int type) ;

#endif
