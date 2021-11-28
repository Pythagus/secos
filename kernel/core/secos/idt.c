/**
 * Managing the Interrupt Descriptor
 * Table.
 *
 * @author Damien Molina
 * @date 2021-11-24
 */

#include <intr.h>

/**
 * Set the handler as the new
 * interruption handler.
 *
 * @param index
 * @param handler
 * @param dpl
 */
void idt_set_handler(int index, void * handler, int dpl) {
    idt_reg_t idtr ;
    get_idtr(idtr) ;

    int_desc_t * desc = &idtr.desc[index] ;
    int_desc(&idtr.desc[index], desc->selector, (offset_t) handler) ;
    desc->dpl = dpl ;
    desc->p   = 1 ;
}