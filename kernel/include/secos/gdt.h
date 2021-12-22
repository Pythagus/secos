#ifndef SECOS_GDT_H
#define SECOS_GDT_H

#include <segmem.h>
#include <string.h>

// Size of a GDT segment.
#define GDT_SEG_SIZE sizeof(seg_desc_t)

// Number in the GDT entries.
#define GDT_NBR_ENTRY 6

// Rings.
#define RING_0 0
#define RING_3 3

// GDT segments repartition.
#define GDT_CODE_R0_SEG 1
#define GDT_DATA_R0_SEG 2
#define GDT_CODE_R3_SEG 3
#define GDT_DATA_R3_SEG 4
#define GDT_TSS_SEG     5

// GDT options offset
#define GDT_G_OFFSET   0 // Size: 1
#define GDT_DPL_OFFSET 1 // Size: 2

// GDT options.
#define GDT_G   (1 << GDT_G_OFFSET)
#define GDT_KRN (RING_0 << GDT_DPL_OFFSET)
#define GDT_USR (RING_3 << GDT_DPL_OFFSET)

/**
 * Initialize the GDT.
 *
 * @return void
 */
void gdt_init() ;

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
seg_desc_t * gdt_seg_init(uint32_t index, uint32_t type, uint32_t base, uint32_t limit, uint32_t options) ;

/**
 * Prepare the TSS segment descriptor.
 *
 * @return void
 */
void gdt_prepare_tss() ;

/**
 * Get the segment at the given
 * index.
 *
 * @param index
 * @return pointer to the GDT segment.
 */
seg_desc_t * gdt_seg_get(uint32_t index) ;

/**
 * Display the GDT.
 *
 * @return void
 */
void gdt_display() ;

#endif
