cmake_minimum_required(VERSION 3.20)
project(secos)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(kernel)
include_directories(kernel/core)
include_directories(kernel/include)
include_directories(tp0)
include_directories(tp1)
include_directories(tp2)
include_directories(tp3)
include_directories(tp4)
include_directories(tp5)
include_directories(tp_exam)
include_directories(utils)

add_executable(secos
        kernel/core/entry.s
        kernel/core/excp.c
        kernel/core/idt.s
        kernel/core/intr.c
        kernel/core/pic.c
        kernel/core/print.c
        kernel/core/stack.c
        kernel/core/start.c
        kernel/core/uart.c
        kernel/include/asm.h
        kernel/include/cr.h
        kernel/include/debug.h
        kernel/include/excp.h
        kernel/include/gpr.h
        kernel/include/grub_mbi.h
        kernel/include/info.h
        kernel/include/intr.h
        kernel/include/io.h
        kernel/include/math.h
        kernel/include/mbi.h
        kernel/include/pagemem.h
        kernel/include/pic.h
        kernel/include/print.h
        kernel/include/segmem.h
        kernel/include/start.h
        kernel/include/string.h
        kernel/include/types.h
        kernel/include/uart.h
        tp0/kernel.elf
        tp0/tp.c
        tp1/tp.c
        tp2/tp.c
        tp3/tp.c
        tp4/tp.c
        tp5/tp.c
        tp_exam/tp.c
        utils/config.mk
        utils/grub.floppy
        utils/linker.lds
        utils/rules.mk kernel/core/secos/gdt.c kernel/include/secos/gdt.h)
