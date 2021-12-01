#include <info.h>
#include <debug.h>
#include <secos/os.h>

extern info_t * info ;

void userland_1() {
    printf("COUCOU 1\n") ;
    while(1) ;
}

void userland_2() {
    printf("COUCOU 2\n") ;
    while(1) ;
}

void tp() {
    secos_initialize() ;
    secos_add_task(&userland_1) ;
    secos_add_task(&userland_2) ;

    while(1) ;
}
