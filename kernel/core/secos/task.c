/**
 * Manage all the tasks the kernel
 * need to treat.
 *
 * @author Damien MOLINA
 * @date 2021-11-25
 */

#include <io.h>
#include <pic.h>
#include <asm.h>
#include <types.h>
#include <debug.h>
#include <string.h>
#include <secos/task.h>
#include <secos/page.h>

#define TASK_ARRAY_SIZE 16

typedef struct t_task t_task ;
struct t_task {
    // Is the task present?
    uint8_t present:1 ;

    // Has the task been run at least once?
    uint8_t executed:1 ;

    // Stack variables.
    uint32_t esp, ebp, eip ;

    // Pagination credentials.
    uint32_t pgd_base ;

    // Process id.
    uint8_t pid ;

    // Next task to be run.
    t_task * next ;
} ;

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

/**
 * Add a new task to the array.
 *
 * @param main
 */
uint8_t task_add(void * main) {
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
            panic("[ERROR] Cannot add another task.\n") ; return -1 ;
        }
    }

    addr->present  = 1 ;
    addr->executed = 0 ;
    addr->eip = (uint32_t) main ;

    // TODO : init esp and ebp.

    // Set the task at the end of the list.
    t_task * tmp    = last_task->next ;
    last_task->next = addr ;
    last_task  = addr ;
    addr->next = tmp ;

    // If no task was added yet.
    if(current_task == NULL) {
        current_task = addr ;
    }

    return addr->pid ;
}

/**
 * Function executed each time the
 * hardware timer IRQ0 generates
 * an interruption.
 *
 * @param ctx
 */
void irq0_timer_callback(int_ctx_t * ctx) {
    if(current_task != NULL) {
        printf("Je switch : %x, %x\n", current_task->eip, ctx->eip) ;
        current_task->executed = 1 ;
        current_task->ebp = *((uint32_t *) (&ctx->gpr.ebp)) ;
        current_task->esp = *((uint32_t *) (&ctx->gpr.esp)) ;

        //current_task->eip = *((uint32_t *) (&ctx->eip)) ;

            /*
        *__asm__ volatile ("PUSHF") ; // Push ALU flags.
        __asm__ volatile ("PUSHL %0" :: "b"(ctx->gpr.eax)) ;
        __asm__ volatile ("PUSHL %0" :: "b"(ctx->gpr.ebx)) ;
        __asm__ volatile ("PUSHL %0" :: "b"(ctx->gpr.ecx)) ;
        __asm__ volatile ("PUSHL %0" :: "b"(ctx->gpr.edx)) ;
        __asm__ volatile ("PUSHL %0" :: "b"(ctx->gpr.esi)) ;
        __asm__ volatile ("PUSHL %0" :: "b"(ctx->gpr.edi)) ;*/

        /*

         void * userland_ptr = userland;
        __asm__ volatile ("MOV %esp, %eax");
        __asm__ volatile ("PUSHl %0" :: "i"(gdt_seg_sel(GDT_DATA_R3_SEG, RING_3))) ; // DS (SS)
        __asm__ volatile ("PUSHl %eax") ; // ESP
        __asm__ volatile ("PUSHF") ; // E flags.
        __asm__ volatile ("PUSHl %0" :: "i"(gdt_seg_sel(GDT_CODE_R3_SEG, RING_3))) ; // CS
        __asm__ volatile ("PUSHl %%ebx"::"b"(userland_ptr)) ; // EIP
        __asm__ volatile ("IRET") ;

         */

        // TODO : PUSH registers in stack.

        current_task = current_task->next ;
        if(current_task->executed) {
            // TODO : POP registers from stack.
        } else {
            printf("Don't POP for %x\n", current_task->eip) ;
        }
    }

    // Send an ACK to the timer.
    outb(PIC1, PIC_EOI) ;
    force_interrupts_on() ;
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
        task->pgd_base = pg_base(task->pid) ;
        task->ebp = pg_user_base(task->pid) + PG_4K_SIZE - 1 ;
        task->esp = task->ebp ;
    }
}
