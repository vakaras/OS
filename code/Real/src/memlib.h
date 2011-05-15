#ifndef MEMLIB_H
#define MEMLIB_H 1

#include "types.h"


u64int kmalloc(u64int size, int align, u64int *phys);


/**
 A llocate a chunk of mem*ory, size in size. The chunk must be
 page aligned.
 **/
u64int kmalloc_a(u64int size);


/**
 A llocate a chunk of mem*ory, size in size. The physical address
 is returned in phys. Phys MUST be a valid pointer to u64int!
 **/
u64int kmalloc_p(u64int size, u64int *phys);


/**
 A llocate a chunk of mem*ory, size in size. The physical address 
 is returned in phys. It must be page-aligned.
 **/
u64int kmalloc_ap(u64int size, u64int *phys);


/**
 G eneral allocation func*tion.
 **/
u64int kmalloc(u64int size);


// Nukopijuoja nurodytus len baitų iš src į dest.
extern "C" void memcpy(u8int *dest, const u8int *src, u64int length);


// Užpildo nurodytą atmintį reikšme value.
extern "C" void memset(u8int *dest, u8int value, u64int length);

#endif
