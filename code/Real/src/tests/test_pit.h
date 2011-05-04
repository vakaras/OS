#ifndef TEST_PIT_H
#define TEST_PIT_H 1

#include "../pit.h"

void enable_PIT(PIT* pit) {

  asm volatile("sti;");
  pit->init_pit();
}

#endif
