/* GPLv2 (c) Airbus */
#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/*
** Simple I/O operations
*/
#define outb(_d_,_p_)   __asm__ volatile ("outb %%al,  %%dx"::"a"(_d_),"d"(_p_))
#define outw(_d_,_p_)   __asm__ volatile ("outw %%ax,  %%dx"::"a"(_d_),"d"(_p_))
#define outl(_d_,_p_)   __asm__ volatile ("outl %%eax, %%dx"::"a"(_d_),"d"(_p_))

#define inb(_p_)                                                \
   ({                                                           \
      uint8_t _d_;                                              \
      __asm__ volatile ("inb %%dx,%%al":"=a"(_d_):"d"(_p_));        \
      _d_;                                                      \
   })

#define inw(_p_)                                                \
   ({                                                           \
      uint16_t _d_;                                             \
      __asm__ volatile ("inw %%dx,%%ax":"=a"(_d_):"d"(_p_));        \
      _d_;                                                      \
   })

#define inl(_p_)                                                \
   ({                                                           \
      uint32_t _d_;                                             \
      __asm__ volatile("inl %%dx, %%eax":"=a"(_d_):"d"(_p_));       \
      _d_;                                                      \
   })

#define out(_d,_p)  outb(_d,_p)
#define in(_p)      inb(_p)

#endif

