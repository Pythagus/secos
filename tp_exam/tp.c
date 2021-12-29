#include <debug.h>
#include <secos/os.h>

void ATTR_SECTION(".user1") userland_1() {
    printf("COUCOU 1\n") ;

    uint32_t * value_ptr   = (uint32_t *) SHARED_ADDR ;
    uint32_t * value_mutex = value_ptr + 1 ;
    *value_mutex = 0 ;
    *value_ptr   = 0 ;

    while(1) {
        if(*value_mutex == 1) {
            *value_ptr += 1 ;
            *value_mutex = 0 ;
        }
    }
}

void ATTR_SECTION(".user2") userland_2() {
    printf("COUCOU 2\n") ;
    //uint32_t * value_ptr   = (uint32_t *) SHARED_ADDR ;
    //uint32_t * value_mutex = value_ptr + 1 ;

    while(1) {
        /*if(*value_mutex == 0) {
            *value_mutex = 1 ;*/
            __asm__ volatile ("INT $0x80" :: "S"(SHARED_ADDR)) ;
        //}
    }
}

void tp() {
    secos_initialize() ;
    secos_add_task(&userland_1) ;
    secos_add_task(&userland_2) ;
    secos_start() ;
}
