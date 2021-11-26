/* GPLv2 (c) Airbus */
#ifndef __ASM_H__
#define __ASM_H__

#include <gpr.h>

#define force_interrupts_on()     __asm__ volatile( "sti ; nop" )
#define force_interrupts_off()    __asm__ volatile( "cli" )
#define halt()                    __asm__ volatile( "cli ; hlt" )

#define interrupts_enabled()         (get_flags() & EFLAGS_IF)
#define disable_interrupts(flags)    ({save_flags(flags);force_interrupts_on();})
#define enable_interrupts(flags)     ({save_flags(flags);force_interrupts_off();})
#define restore_interrupts(flags)    load_flags(flags)

#endif
