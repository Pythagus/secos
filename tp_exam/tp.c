#include <debug.h>
#include <secos/os.h>

// Physical memory area address shared by the user processes.
#define SHARED_ADDR_PHY 0x801000

// Virtual memory area address.
#define SHARED_ADDR_USR1 0x801000
#define SHARED_ADDR_USR2 0x801000

/*
 * Structure used to easily store
 * the mutex and the counter value.
 */
typedef struct t_counter {
    uint32_t mutex ;
    uint32_t value ;
} t_counter ;

void ATTR_SECTION(".user1") userland_1() {
    printf("COUCOU 1\n") ;

    uint32_t mutex ;
    t_counter * counter = (t_counter *) SHARED_ADDR_USR1 ;
    counter->mutex = 0 ;
    counter->value = 20 ;

    while(1) {
        mutex = counter->mutex ;

        if(mutex == 0) {
            counter->value += 1 ;
            counter->mutex  = 1 ;
        } else if(mutex != 1) {
            printf("\n\n\n\n======> [ERROR 1] : MUTEX (%x : %x) = %d\n\n\n\n\n", &counter, counter, mutex) ;
            while(1) ;
        }
    }
}

void ATTR_SECTION(".user2") userland_2() {
    printf("COUCOU 2\n") ;

    uint32_t mutex ;
    t_counter * counter = (t_counter *) SHARED_ADDR_USR2 ;

    while(1) {
        mutex = counter->mutex ;

        if(mutex == 1) {
            __asm__ volatile ("INT $0x80" :: "a"(&counter->value)) ;
            counter->mutex = 0 ;
        } else if(mutex != 0) {
            printf("\n\n\n\n======> [ERROR 2] : MUTEX (%x : %x) = %d\n\n\n\n\n", &counter, counter, mutex) ;
            while(1) ;
        }
    }
}

void tp() {
    secos_initialize() ;

    // Add first task.
    t_task * task_1 = secos_add_task(&userland_1) ;
    secos_translate(task_1, SHARED_ADDR_PHY, SHARED_ADDR_USR1) ;

    // Add second task.
    t_task * task_2 = secos_add_task(&userland_2) ;
    secos_translate(task_2, SHARED_ADDR_PHY, SHARED_ADDR_USR2) ;

    secos_start() ;
}
