/**
 * Main OS entry point.
 *
 * @author Damien MOLINA
 * @date 2022-01-02
 */

#include <debug.h>
#include <secos/os.h>

// Physical memory area address shared by the user processes.
#define SHARED_ADDR_PHY 0x801000

// Virtual memory area address.
#define SHARED_ADDR_USR1 0x801000
#define SHARED_ADDR_USR2 0x801000

// Manage a mutex error.
#define MUTEX_ERROR(__id__, __val__) ({ \
    printf("\n\n\n\n======> [ERROR %d] : MUTEX = %d\n\n\n\n", __id__, __val__) ; \
    while(1) ; \
})

/*
 * Structure used to easily store
 * the mutex and the counter value.
 */
typedef struct t_counter {
    uint32_t mutex ;
    uint32_t value ;
} t_counter ;

/**
 * Call the system function to print the
 * counter value.
 *
 * @param value
 */
void ATTR_SECTION(".sys_counter") sys_counter(const uint32_t * value) {
    __asm__ volatile ("INT $0x80" :: "a"(value)) ;
}

/**
 * Userland function used to increment
 * the counter value.
 *
 * @return void
 */
void ATTR_SECTION(".user1") userland_1() {
    uint32_t mutex ;
    t_counter * counter = (t_counter *) SHARED_ADDR_USR1 ;
    counter->mutex = 0 ;
    counter->value = 10 ;

    while(1) {
        mutex = counter->mutex ;

        if(mutex == 0) {
            counter->value += 1 ;
            counter->mutex  = 1 ;
        } else if(mutex != 1) {
            MUTEX_ERROR(1, counter->mutex) ;
        }
    }
}

/**
 * Userland function used to print
 * the counter value thanks to a
 * system call.
 *
 * @return void
 */
void ATTR_SECTION(".user2") userland_2() {
    uint32_t mutex ;
    t_counter * counter = (t_counter *) SHARED_ADDR_USR2 ;

    while(1) {
        mutex = counter->mutex ;

        if(mutex == 1) {
            sys_counter(&counter->value) ;
            counter->mutex = 0 ;
        } else if(mutex != 0) {
            MUTEX_ERROR(2, counter->mutex) ;
        }
    }
}

/**
 * Main OS loop.
 *
 * @return void
 */
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
