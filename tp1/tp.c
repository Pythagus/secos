/* GPLv2 (c) Airbus */
/*
 * For any help, see : https://wiki.osdev.org/Global_Descriptor_Table#Segment_Descriptor
 */
#include <string.h>
#include <segmem.h>
#include <info.h>
#include <secos/gdt.h>

extern info_t * info ;

void tp() {
    gdt_init() ;

    // Code segment for executables.
    gdt_seg_init(GDT_CODE_RO_SEG, SEG_DESC_CODE_XR, 0, 0xFFFFF, GDT_KRN | GDT_G) ;

    // Data segment for data.
    gdt_seg_init(GDT_DATA_R0_SEG, SEG_DESC_DATA_RW, 0, 0xFFFFF, GDT_KRN | GDT_G) ;

    // Set the CPU's registers value.
    set_ss(gdt_seg_sel(GDT_DATA_R0_SEG, RING_0)) ; // Stack Segment (SP)
    set_ds(gdt_seg_sel(GDT_DATA_R0_SEG, RING_0)) ;
    set_cs(gdt_seg_sel(GDT_CODE_RO_SEG, RING_0)) ;

    // Question 3 :
    gdt_seg_init(GDT_CODE_R3_SEG, SEG_DESC_DATA_RW, 0x600000, 0x31, GDT_KRN | GDT_G) ;

     // Extra segment.
    set_es(gdt_seg_sel(GDT_CODE_R3_SEG, RING_0)) ;

    gdt_reg_t gdtr ;
    get_gdtr(gdtr) ;
    gdt_display(&gdtr) ;

    char src[64] ;
    memset(src, 1, 1) ;
    //char * dst = 0 ;
    //_memcpy8(dst, src, 32) ;
}