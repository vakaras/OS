#ifndef PIT_H
#define PIT_H 1

#include "types.h"
#include "monitor.h"
#include "idt.h"
#include "primitives.h"

class PIT {
  
private:
  u64int frequency;
  u64int divisor;

public:
  
  PIT(u64int frequency) {
    this->frequency = frequency;
    this->divisor = 1193181 / this->frequency;
  }
  
  void init_pit() {
    /* Set the PIT to repeating mode */
    send_byte(0x43, 0x36);
    /* Set up divisor to be sent */
    u8int l = (u8int)(this->divisor & 0xFF);
    u8int h = (u8int)((this->divisor>>8) & 0xFF);
    /* Send the divisor in bit-wise level */
    send_byte(0x40, l);
    send_byte(0x40, h);
  }

};




#endif