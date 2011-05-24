#include "debug.h"


// Nusiunčia numatytą į Bochs.
void debug_ping() {
  debug_string("Veikia.\n");
  }


// Nusiunčia baitą į Bochs.
void debug_char(char c) {
  send_byte(0xE9, c);
  }


// Nusiunčia simbolių seką, kurios pabaigos požymis yra 0, į Bochs.
void debug_string(const char *str) {
  for (const char *i = str; *i; i++)
    send_byte(0xE9, *i);
  }


// Nusiunčia šešioliktainį skaičių į Bochs.
void debug_hex(u64int number) {

  for (int i = 60; i >= 0; i -= 4) {
    u8int digit = (number >> i) & 0xF;
    if (digit >= 10) {
      debug_char(digit - 10 + 'A');
      }
    else {
      debug_char(digit + '0');
      }
    }

  }
  
  void debug_dec(u64int number) {
    int length = 1;
    for (int i = number; i; i /= 10) {
      length *= 10;
    }
    if (length > 1) {
      length /= 10;
    }
    for (; length; length /= 10) {
      debug_char('0' + ((number) / length) % 10);
    }
  }
  
  ScreenCharacter memory[8][SCREEN_WIDTH];
  
  void set_chr(int row, int col, char value) {
    memory[row][col] = ScreenCharacter(
      value, COLOR_CYAN, COLOR_RED);
  }
  
  void panic(const char * file, int line,
             const char * func, const char * text) {
    debug_string("PANIC!!! In function ");
    debug_string(func);
    debug_string("() on line <"); debug_dec(line);
    debug_string("> @ "); debug_string(file);
    debug_string(".\n");
    debug_string("ZINUTE: \"");
    debug_string(text);
    debug_string("\".\n");
    
    asm volatile("mov %0, %%cr3" : : "r"((u64int)0x00000000011fa000));
    const char * tekstas = 
    "                              P A N I C ! ! ! !                      ";
    
    int r=0;
    int co=0;
    for(r=0;r<8;r++)
      for(co=0;co<SCREEN_WIDTH;co++){
        set_chr(r, co, ' ');
      }
    r = 2;
    co = 0;
    for (const char *c = tekstas; *c; c++) {
      set_chr(r, co++, *c);
      if(co>=SCREEN_WIDTH){
        co=0;r++;
      };
    };
    r++;
    co=0;
    for (const char *c = " Info: "; *c; c++) {
      set_chr(r, co++, *c);
      if(co>=SCREEN_WIDTH){
        co=0;r++;
      };
    };
    for (const char *c = func; *c; c++) {
      set_chr(r, co++, *c);
      if(co>=SCREEN_WIDTH){
        r++; co=0;
      };
    };
    for (const char *c = "() <"; *c; c++) {
      set_chr(r, co++, *c);
      if(co>=SCREEN_WIDTH){
        r++; co=0;
      };
    };
    co+=3;
    int ilgis = 0;
    while(line > 0) {
      char c = (line % 10) + '0';
      line /= 10;
      set_chr(r, co--, c);
      ilgis++;
    };
    co+=ilgis+1;
    for (const char *c = "> @ "; *c; c++) {
      set_chr(r, co++, *c);
      if(co>=SCREEN_WIDTH){
        r++; co=0;
      };
    };
    for (const char *c = file; *c; c++) {
      set_chr(r, co++, *c);
      if(co>=SCREEN_WIDTH){
        r++; co=0;
      };
    };
    r++;co=0;
    for (const char *c = " Zinute: "; *c; c++) {
      set_chr(r, co++, *c);
      if(co>=SCREEN_WIDTH){
        r++; co=0;
      };
    };
    for (const char *c = text; *c; c++) {
      set_chr(r, co++, *c);
      if(co>=SCREEN_WIDTH){
        r++; co=0;
      };
    };
    
    memcpy((u8int*)0xB8000, (u8int*)memory, SCREEN_WIDTH*8*2) ;
    
    pause();
  }

// Nusiunčia šešioliktainį skaičių su paaiškinimu į Bochs.
void debug_value(const char *str, u64int number) {

  debug_string(str);
  debug_hex(number);
  debug_char('\n');
  }

// Pauzė. 
void pausep(u64int *pointer) {
  pause1((u64int) pointer);
  }
