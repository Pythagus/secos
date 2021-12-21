/**
 * Functions manipulating the Global
 * Descriptor Table (GDT) and the segments.
 *
 * @author Damien Molina
 * @date 2021-10-20
 */

#include <secos/gdt.h>
#include <debug.h>

/**
 * GDT entries.
 */
seg_desc_t gdt_entries[GDT_NBR_ENTRY] ;

/**
 * Task State Segment variable.
 */
tss_t tss ;

/**
 * Initialize the GDT.
 *
 * @return void
 */
void gdt_init() {
    gdt_reg_t gdtr ;

    // Global GDT data.
    gdtr.desc  = gdt_entries ;
    gdtr.limit = GDT_NBR_ENTRY * GDT_SEG_SIZE - 1 ;

    // Initialize the first entry to NULL
    gdt_seg_init(0, 0, 0, 0, 0) ;
    gdtr.desc->p = 0 ;

    set_gdtr(gdtr) ;
}

/**
 * Initialize the segment of the GDT
 * with default values and given ones.
 * 
 * @param index
 * @param type
 * @param base
 * @param limit
 * @param options
 * @return the updated segment
 */
seg_desc_t * gdt_seg_init(uint32_t index, uint32_t type, uint32_t base, uint32_t limit, uint32_t options) {
    seg_desc_t * segment = gdt_entries + index ;

    /*
     * Reset the memory area to be sure the
     * entry is completely empty.
     */
    memset(segment, 0, GDT_SEG_SIZE) ;

    // Default values.
    segment->d = 1 ;
    segment->s = 1 ;
    segment->l = 0 ;

    // Options.
    segment->dpl = (options >> GDT_DPL_OFFSET) & 0b11 ;
    segment->g   = (options >> GDT_G_OFFSET) & 0b1 ;

    segment->type = type ;
    segment->p    = 1 ;
    segment->limit_1 = limit & 0xFFFF ;
    segment->limit_2 = (limit >> 16) & 0x0F ;
    segment->base_1  = base ;
    segment->base_2  = base >> 16 ;
    segment->base_3  = base >> 24 ;

    return segment ;
}

/**
 * Prepare the TSS segment descriptor.
 *
 * @return void
 */
void gdt_prepare_tss() {
    raw32_t tss_addr = { .raw = (offset_t) &tss } ;
    seg_desc_t * tss_desc = gdt_seg_init(GDT_TSS_SEG, SEG_DESC_SYS_TSS_AVL_32, 0, 0xFFFFF, 0) ;
    tss_desc->raw    = sizeof(tss_t) ;
    tss_desc->base_1 = tss_addr.wlow ;
    tss_desc->base_2 = tss_addr._whigh.blow ;
    tss_desc->base_3 = tss_addr._whigh.bhigh ;
    tss_desc->type   = SEG_DESC_SYS_TSS_AVL_32 ;
    tss_desc->p      = 1 ;

    // Preparing Task State Segment (TSS).
    tss.s0.esp = get_ebp() ;
    tss.s0.ss  = gdt_seg_sel(GDT_DATA_R0_SEG, RING_0) ;
    set_tr(gdt_seg_sel(GDT_TSS_SEG, RING_0)) ;
}

/**
 * Get the segment at the given
 * index.
 *
 * @param index
 * @return pointer to the GDT segment.
 */
seg_desc_t * gdt_seg_get(uint32_t index) {
    return gdt_entries + index ;
}

/**
 * Display the segment using debug().
 *
 * @param segment
 */
static inline void display_segment(seg_desc_t * segment) {
    int base  = 0 | segment->base_1  | (segment->base_2  << 16) | (segment->base_3 << 24) ;
    int limit = 0 | segment->limit_1 | (segment->limit_2 << 16) ;

    if(segment->g && limit != 0) {
        limit++ ;
        limit *= 4096 ;
        limit-- ;
    }

    if(segment->p) {
        char * type = "  UNKNOWN  " ;

        switch(segment->type) {
            case SEG_DESC_DATA_R:     type = "DATA      R" ; break ;
            case SEG_DESC_DATA_RW:    type = "DATA     RW" ; break ;
            case SEG_DESC_DATA_RWA:   type = "DATA    RWA" ; break ;
            case SEG_DESC_CODE_X:     type = "CODE      X" ; break ;
            case SEG_DESC_CODE_XR:    type = "CODE     XR" ; break ;
            case SEG_DESC_CODE_XRA:   type = "CODE    XRA" ; break ;
            case SEG_DESC_CODE_CXRA:  type = "CODE   CXRA" ; break ;
            case SEG_DESC_SYS_TSS_AVL_32: type = "TSS      32" ; break ;
        }

        printf("| 0x%8x | 0x%8x | %s |  %d  |\n", base, limit, type, segment->dpl) ;
    } else {
        printf("|                      NULL                   |\n") ;
    }
}

/**
 * Display the GDT.
 *
 * @return void
 */
void gdt_display() {
    gdt_reg_t gdt ;
    get_gdtr(gdt) ;

    int size = (int) ((gdt.limit + 1) / GDT_SEG_SIZE) ;

    printf("<------- Global Descriptor Table (GDT) ------->\n") ;

    if(size > 0) {
        printf("+    BASE    +    LIMIT   +    TYPE     + DPL +\n") ;

        for(int i = 0 ; i < size ; i++) {
            display_segment(gdt.desc + i) ;
        }
    }

    printf("<----------------- End of GDT ---------------->\n") ;
}
