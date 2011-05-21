#ifndef CPU_H
#define CPU_H 1


#include "types.h"


struct CPUContext { 

  u64int   vector, 
           R15, R14, R13, R12, 
           R11, R10, R9, R8, 
           DI, SI, BP, 
           BX, DX, CX, AX,
           error, IP, CS,
           FLAGS, SP, SS;
  
  CPUContext() {

    vector = 0;
    R15 = R14 = R13 = R12 = R11 = R10 = R9 = R8 = 0;
    DI = SI = BP = BX = DX = CX = AX = 0;
    error = IP = CS = FLAGS = SP = SS = 0;

    }

  } __attribute__((packed));

#endif
