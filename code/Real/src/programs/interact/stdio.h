#ifndef STDIO_H
#define STDIO_H 1


#include "cxx.h"
#include "icxxabi.h"

#define MAX_DECIMAL_LENGTH 20


namespace std {

  typedef unsigned long long u64int;
  typedef   signed long long s64int;

  extern "C" u64int open_file_read(u64int file_name);
  extern "C" u64int open_file_write(u64int file_name);
  extern "C" void close_file(u64int file_descriptor);
  extern "C" char read_byte(u64int file_descriptor);
  extern "C" void write_byte(char symbol, u64int file_descriptor);


  class StreamOut {

    private:

    // Atributai.

    u64int file_descriptor;

    public:

    // Metodai.

    StreamOut() {
      this->file_descriptor = 1;
      }
    
    StreamOut(u64int file_name) {
      this->file_descriptor = open_file_write(file_name);
      }
    
    ~StreamOut() {

      if (this->file_descriptor != 1) {
        close_file(this->file_descriptor);
        }
      
      }
  
    /**
     * Įrašo simbolį.
     */
    u64int write(char c) {

      write_byte(c, this->file_descriptor);

      return 0;
      }
    
    /**
     * Įrašo simbolių eilutę, kurios pabaigos požymis yra 0.
     */
    u64int write(const char *str) {

      for (const char *i = str; *i; i++) {
        write_byte(*i, this->file_descriptor);
        }
      
      return 0;
      }

    u64int write(u64int value) {

      if (!value) {
        write_byte('0', this->file_descriptor);
        }
      else {

        u64int mul = 1;
        u64int tmp = value;
        while (tmp > 0) {
          tmp /= 10;
          mul *= 10;
          }

        mul /= 10;
        while (mul > 0) {
          write_byte('0' + (value / mul) % 10, this->file_descriptor);
          mul /= 10;
          }
        
        }
      
      return 0;
      }

    };


  StreamOut stdout;


  class StreamIn {

    private:

    // Atributai.

    u64int file_descriptor;

    public: 

    // Metodai

    StreamIn() {
      this->file_descriptor = 0;
      }

    StreamIn(u64int file_name) {
      this->file_descriptor = open_file_read(file_name);
      }
    
    ~StreamIn() {

      if (this->file_descriptor != 0) {
        close_file(this->file_descriptor);
        }

      }

    /**
     * Nuskaito simbolį.
     */
    u64int read(char *c) {

      *c = read_byte(this->file_descriptor);

      return 0;
      }
    
    /**
     * Nuskaito simbolių eilutę, kur baigiasi end.
     */
    u64int read(char *str, int len, char end) {

      //char *p = str;

      //stdout.write("Nuskaitoma: ");
      for (int i = 0; i < len; i++) {
        *str = read_byte(this->file_descriptor);
        //stdout.write(*str);
        if (*str == end) {
          break;
          }
        else {
          str++;
          }
        }

      *str = 0;
      //stdout.write("<---\n");
      //stdout.write("Nuskaite:");
      //stdout.write(p);
      //stdout.write("\n");

      return 0;
      }

    u64int read(char *str, int len) {
      return read(str, len-1, 0);
      }
    
    u64int read_line(char *str, int len) {
      return read(str, len-1, '\n');
      }

    u64int read(u64int *number) {

      char buffer[MAX_DECIMAL_LENGTH + 2];

      for (u64int i = 0; i < MAX_DECIMAL_LENGTH + 2; i++) {
        buffer[i] = 0;
        }

      u64int i;
      for (i = 0; i < MAX_DECIMAL_LENGTH; i++) {
        buffer[i] = read_byte(this->file_descriptor);
        if ('0' <= buffer[i] && buffer[i] <= '9') {
          buffer[i] -= '0';
          }
        else {
          buffer[i] = 0;
          break;
          }
        }

      *number = 0;
      for (u64int j = 0; j < i; j++) {
        *number *= 10;
        *number += buffer[j];
        }

      return 0;
      }

    };

  StreamIn stdin;

  }

#endif
