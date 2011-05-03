/**
 * Įvairūs primityvai.
 */
#ifndef PRIMITIVES_H
#define PRIMITIVES_H 1

#include "types.h"
 

// Pasiųsti baitą į nurodytą portą.
void send_byte(u16int port, u8int value);


// Nuskaityti baitą iš nurodyto porto.
u8int get_byte(u16int port);


// Nuskaityti du baitus iš nurodyto porto.
u16int get_bytes2(u16int port);

#endif
