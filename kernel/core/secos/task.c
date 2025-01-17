/**
 * Manage all the tasks the kernel
 * need to treat.
 *
 * @author Damien MOLINA
 * @date 2021-11-25
 */

#include <io.h>
#include <pic.h>
#include <types.h>
#include <debug.h>
#include <string.h>
#include <secos/gdt.h>
#include <secos/task.h>
#include <secos/page.h>

// Iterate the given macro on the registers.
#define ITERATE_REGISTERS(__macro__) ({ \
    __macro__(eax) ; \
    __macro__(ebx) ; \
    __macro__(ecx) ; \
    __macro__(edx) ; \
    __macro__(esi) ; \
    __macro__(edi) ; \
})

// Save the registers into the current_task variable.
#define SAVE_REGISTER(__reg__) current_task->__reg__ = get_reg(__reg__)

// Restore the registers.
#define RESTORE_REGISTER(__reg__) set_reg(__reg__, current_task->__reg__)

// Pop the stack top value, but do nothing with it.
#define POP_TRASH __asm__ volatile ("POP %eax")

// Pop the stack top value and store it in __var__.
#define POP_SAVE(__var__) __asm__ volatile ("POP %0" : "=r"(__var__))

/*
 * The task array containing all the
 * tasks' data.
 */
t_task tasks[TASK_ARRAY_SIZE] ;

/*
 * Determine what is the current
 * task index in the tasks array.
 */
t_task * current_task = NULL ;

/*
 * Determine the end of the task
 * scheduler to be able to add
 * a new task.
 */
t_task * last_task = NULL ;

extern tss_t tss ;

/**
 * Add a new task to the array.
 *
 * @param main
 * @return the task struct pointer.
 */
t_task * task_add(uint32_t main) {
    t_task * addr = NULL ;

    // If there is no tasks.
    if(last_task == NULL) {
        addr       = tasks ;
        addr->next = addr ;
        last_task  = addr ;
    } else {
        for(int i = 0 ; i < TASK_ARRAY_SIZE ; i++) {
            if(! tasks[i].present) {
                addr = tasks + i ;
                break ;
            }
        }

        if(addr == NULL) {
            panic("[ERROR] Cannot add another task.\n") ; return NULL ;
        }
    }

    addr->present  = 1 ;
    addr->executed = 0 ;
    addr->eip = main ;

    // Set the task at the end of the list.
    t_task * tmp    = last_task->next ;
    last_task->next = addr ;
    last_task  = addr ;
    addr->next = tmp ;

    // If no task was added yet.
    if(current_task == NULL) {
        current_task = addr ;
    }

    return addr ;
}

/**
 * Function executed each time the
 * hardware timer IRQ0 generates
 * an interruption.
 *
 * @param ctx
 */
void irq0_timer_callback() {
    outb(PIC_EOI, PIC1) ;

    if(current_task != NULL) {
        if(current_task->executed) {
            POP_TRASH ;
            POP_TRASH ;
            POP_TRASH ;
            POP_TRASH ;
            POP_TRASH ;
            POP_TRASH ;
            POP_TRASH ;
            POP_TRASH ;
            POP_TRASH ;
            POP_SAVE(current_task->stack_usr_ebp) ;
            POP_SAVE(current_task->eip) ;
            POP_TRASH ;
            POP_TRASH ;
            POP_SAVE(current_task->stack_usr_esp) ;
            POP_TRASH ;
            POP_TRASH ;
            POP_TRASH ;
            POP_TRASH ;
            POP_TRASH ;

            // Save the registers.
            ITERATE_REGISTERS(SAVE_REGISTER) ;

            // Go to the next task.
            current_task = current_task->next ;

            // Restore the registers
            ITERATE_REGISTERS(RESTORE_REGISTER) ;
        }

        current_task->executed = 1 ;

        tss.s0.esp = current_task->stack_krn_esp ;
        set_cr3(current_task->pgd_base) ;
        set_ebp(current_task->stack_usr_ebp) ;

        // Save the current eflags (ALU flags).
        save_flags(current_task->eflags) ;
        current_task->eflags = current_task->eflags | EFLAGS_IF ;

        // Prepare the IRET.
        __asm__ volatile ("PUSHl %0"    :: "i"(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3))) ; // DS (SS)
        __asm__ volatile ("PUSHl %%ebx" :: "b"(current_task->stack_usr_esp)) ; // ESP
        __asm__ volatile ("PUSH %0"     :: "m"(current_task->eflags)) ; // E flags.
        __asm__ volatile ("PUSHl %0"    :: "i"(gdt_seg_sel(GDT_CODE_R3_SEG, RING_3))) ; // CS
        __asm__ volatile ("PUSHl %%ebx" :: "b"((void *) current_task->eip)) ; // EIP
        __asm__ volatile ("IRET") ;
    }
}

/**
 * Initialize the tasks management.
 *
 * @return void
 */
void task_initialize() {
    t_task * task ;

    for(int i = 0 ; i < TASK_ARRAY_SIZE ; i++) {
        task = tasks + i ;

        memset(task, 0, sizeof(t_task)) ;
        task->pid = i + 1 ;
        task->present  = 0 ;
        task->executed = 0 ;
        task->pgd_base = pg_base_usr(i) ;

        // Prepare the stack.
        task->stack_usr_esp = pg_stack_usr(i) ;
        task->stack_usr_ebp = task->stack_usr_esp ;
        task->stack_krn_esp = pg_stack_krn(i) ;
    }
}
