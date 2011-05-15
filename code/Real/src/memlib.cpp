#include "types.h"
#include "memlib.h"


u64int placement_address = 16777215;


u64int kmalloc(u64int size, int align, u64int *phys) {
  if (align == 1 && (placement_address & 0xFFFFFFFFFFFFF000)) 
    // If the address is not already page-aligned
  {
    // Align it.
    placement_address &= 0xFFFFFFFFFFFFF000;
    placement_address += 0x1000;
  }
  if (phys)
  {
    *phys = placement_address;
  }
  u64int tmp = placement_address;
  placement_address += size;
  return tmp;
}

u64int kmalloc_a(u64int size)
{
  return kmalloc(size, 1, 0);
}

u64int kmalloc_p(u64int size, u64int *phys)
{
  return kmalloc(size, 0, phys);
}

u64int kmalloc_ap(u64int size, u64int *phys)
{
  return kmalloc(size, 1, phys);
}

u64int kmalloc(u64int size)
{
  return kmalloc(size, 0, 0);
}


// Nukopijuoja nurodytus len baitų iš src į dest.
void memcpy(u8int *dest, const u8int *src, u64int length) {

  const u8int *sp = src;
  for(u8int *dp = dest; length != 0; length--) {
    *dp++ = *sp++;
    }

  }


// Užpildo nurodytą atmintį reikšme value.
void memset(u8int *dest, u8int value, u64int length) {

  for (u8int *p = dest ; length != 0; length--) {
    *p++ = value;
    }

  }
