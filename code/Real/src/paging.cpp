#include "paging.h"
#include "memlib.h"
#include "primitives.h"


u64int align(u64int address) {

  if ((address & 0xfff) == 0) {
    return address;
    }
  else {
    return (address | 0xfff) + 1;
    }
  
  }

void *align(void *pointer) {

  return (void *) align((u64int) pointer);

  }
