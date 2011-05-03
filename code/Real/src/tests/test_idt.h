#ifndef TEST_IDT_H
#define TEST_IDT_H 1


void test_idt() {

  asm volatile("int $0x4;");

  }

#endif
