#ifndef MEMLIB_H
#define MEMLIB_H

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

void memset(u8int *dest, u8int val, u32int len);
void memcpy(u8int *dest, const u8int *src, u32int len);


#endif