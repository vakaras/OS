#ifndef TEST_DEBUG_H
#define TEST_DEBUG_H 1

#include "../debug.h"


void test_debug() {

  debug_string("\nTestuojamas skaičiaus rašymas: ");
  debug_hex(0xdeadbaba12345678);
  debug_string("\nTestuojamas signalas:          ");
  debug_ping();
  debug_string("\nTestuojama Bochs pauzė su dviem argumentais. ");
  debug_string("Peržiūrėti registrų reikšmes galima su komanda „reg“.\n");
  pause2(0xdead, 0xbaba);

  }

#endif
