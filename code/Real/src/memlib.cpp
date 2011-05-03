#include "types.h"
#include "memlib.h"

u64int placement_address = 16777215;

u64int kmalloc(u64int size, int align, u64int *phys)
{
  if (align == 1 && (placement_address & 0xFFFFFFFFFFFFF000)) // If the address is not already page-aligned
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

// Copy len bytes from src to dest.
void memcpy(u8int *dest, const u8int *src, u32int len)
{
  const u8int *sp = (const u8int *)src;
  u8int *dp = (u8int *)dest;
  for(; len != 0; len--) *dp++ = *sp++;
}

// Write len copies of val into dest.
void memset(u8int *dest, u8int val, u32int len)
{
  u8int *temp = (u8int *)dest;
  for ( ; len != 0; len--) *temp++ = val;
}
