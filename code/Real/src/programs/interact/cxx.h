#ifndef CXX_H
#define CXX_H 1


/*
 * Funkcijos, kurios yra iškviečiamos, kai yra bandoma iškviesti virtualią
 * funkciją. (Jų reikalauja g++.)
 *
 * Pagal http://wiki.osdev.org/C%2B%2B
 */


extern "C" void __cxa_pure_virtual() {
  return;
  }


int __cdecl_purecall() {
  return -1;
  }


/*
 * Statinio lokalaus kintamojo apsauga.
 *
 * Pagal http://wiki.osdev.org/C%2B%2B#Local_Static_Variables_.28GCC_Only.29
 *
 * FIXME: Reikia realizuoti, čia tik tiek, kiek reikia, jog kompiliuotųsi.
 * FIXME: Patikrinti, ar veikia su 64 bitais.
 */
namespace __cxxabiv1 {
 
  __extension__ typedef int __guard __attribute__((mode(__DI__)));
 
  extern "C" int __cxa_guard_acquire (__guard *);
  extern "C" void __cxa_guard_release (__guard *);
  extern "C" void __cxa_guard_abort (__guard *);
 
  extern "C" int __cxa_guard_acquire (__guard *g) {
    return !*(char *)(g);
    }
 
  extern "C" void __cxa_guard_release (__guard *g) {
    *(char *)g = 1;
    }
 
  extern "C" void __cxa_guard_abort (__guard *) {
    }

  }

#endif
