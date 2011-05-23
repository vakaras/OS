#ifndef MONITOR_H
#define MONITOR_H 1

#include "monitor_defines.h"
#include "memlib.h"
#include "monitor_screen_character.h"
#include "structures/array2dpointer.h"
#include "debug.h"


/**
 * Ekrano valdymo klasė.
 */
class Monitor {

private:

// Atributai.

  Array2dPointer<ScreenCharacter> video_memory;
                                        // Video atminties blokas.
  u8int foreground_color;               // Teksto spalva. (Reikšmė nuo
                                        // 0x0 iki 0xF.)
  u8int background_color;               // Fono spalva. (Reikšmė nuo
                                        // 0x0 iki 0xF.)
// Vidinės klasės.

  class Cursor {

    // Atributai.
    private:
    u8int row, col;                     // Žymeklio pozicija.
    Monitor &monitor;                   // Nuoroda į ekrano objektą.

    // Metodai.
        
    // Pataiso žymeklio poziciją į egzistuojančią.
    void normalize() {
      if (this->col >= SCREEN_WIDTH) {
        this->col = 0;
        this->row++;
        }
      if (this->row >= SCREEN_HEIGHT) {
        this->monitor.scroll();
        this->row--;
        }
      }
    
    public:
      
    Cursor(Monitor &_monitor): monitor(_monitor) {
      }
      
      // Perkelia žymeklį.
    void move() {
      this->normalize();
      
      u16int offset = this->row * SCREEN_WIDTH + this->col;
      send_byte(0x3D4, 14);             // Nurodymas, kad siųsime 
      // viršutinį baitą.
      send_byte(0x3D5, offset >> BYTE_SIZE);
      // Išsiunčiame viršutinį baitą.
      send_byte(0x3D4, 15);             // Nurodymas, kad siųsime
      // apatinį baitą.
      send_byte(0x3D5, offset);         // Išsiunčiame apatinį baitą.
    }
      
    void set_row(u8int row){
      this->row = row;
    }
    void set_col(u8int col){
      this->col = col;
    }
    u8int get_row(){
      return this->row;
    }
    u8int get_col(){
      return this->col;
    }
    // Perkelia žymeklį.
    void move(u8int row, u8int col) {
      this->row = row;
      this->col = col;
      this->move();
      }

    // Pastumia žymeklį per vieną simbolį į priekį.
    void inc() {
      this->col++;
      this->move();
      }

    // Pastumia žymeklį per vieną simbolį atgal.
    void dec() {
      this->col--;
      this->move();
      }

    // Pastumia žymeklį per tiek pozicijų į priekį, kad pozicija būtų
    // 8 kartotinis.
    void tab() {
      this->col = (this->col + 8) & (~(8 - 1));
      this->move();
      }

    // Grąžina žymeklį į eilutės pradžią.
    void back() {
      this->col = 0;
      this->move();
      }

    // Perkelia žymeklį į naują eilutę.
    void newline() {
      this->col = 0;
      this->row++;
      this->move();
      }

    // Užrašo simbolį ir paslenka žymeklį.
    void put(char character) {

      if (character == '\b' && this->col) {
        this->dec();
        }
      else if (character == '\t') {
        this->tab();
        }
      else if (character == '\r') {
        this->back();
        }
      else if (character == '\n') {
        this->newline();
        }
      else if (character >= ' ') {      // Visi kiti nematomi simboliai yra
                                        // ignoruojami.
        this->monitor.set_character(this->row, this->col, character);
        this->inc();
        }
      
      }

    } cursor;
    
    class Screen {
      
    private:

      // Atributai.

      ScreenCharacter video_memory[SCREEN_HEIGHT][SCREEN_WIDTH];
      u8int cursor_row; 
      u8int cursor_col;
      
    public:

      // Metodai.
      
      Screen() {
        }
        
      void save_screen_memory(Array2dPointer<ScreenCharacter> *old_mem) {

        memcpy(
            (u8int*) this->video_memory, 
            (u8int*) old_mem->get_pointer(0, 0),
            SCREEN_HEIGHT * SCREEN_WIDTH * 2);

        }
      
      void reset_screen_memory(Array2dPointer<ScreenCharacter> *old_mem) {

        memcpy(
            (u8int*) old_mem->get_pointer(0, 0),
            (u8int*) this->video_memory,
            SCREEN_HEIGHT * SCREEN_WIDTH * 2);

        }
      
      u8int get_cursor_row() {
        return this->cursor_row;
        }
      
      u8int get_cursor_col() {
        return this->cursor_col;
        }
            
      void save_screen_cursor(u8int row, u8int col) {

        this->cursor_row = row;
        this->cursor_col = col;

        }
      
    } screen[6];

// Atributai.
  
  Screen *active_screen;                // Nuoroda į aktyvų ekraną.

// Metodai.

public:
  u8int active_screen_id;
  
  Monitor():
    video_memory((ScreenCharacter *)0xB8000, SCREEN_WIDTH, SCREEN_HEIGHT),
    cursor(*this) {
      
      this->foreground_color = COLOR_WHITE;
      this->background_color = COLOR_BLACK;
      this->active_screen = &screen[4];
      this->active_screen_id = 5;
      
    }

  void activate_screen(int no) {

    this->active_screen->save_screen_memory(&(this->video_memory));
    this->active_screen->save_screen_cursor(
        this->cursor.get_row(), this->cursor.get_col());
    this->active_screen = &screen[no-1];
    this->active_screen_id = no;
    this->active_screen->reset_screen_memory(&this->video_memory);
    this->cursor.set_row(this->active_screen->get_cursor_row());
    this->cursor.set_col(this->active_screen->get_cursor_col());
    this->cursor.move();

    }
    
  void set_foreground_color(u8int color) {
    this->foreground_color = color;
    }
  
  void set_background_color(u8int color) {
    this->background_color = color;
    }

  // Nurodytam „langeliui“ priskiria simbolį.
  void set_character(int row, int col, char value) {
    this->set_character(
        row, col, ScreenCharacter(
          value, this->foreground_color, this->background_color));
    }

  // Nurodytam „langeliui“ priskiria simbolį ir spalvą.
  void set_character(int row, int col, ScreenCharacter value) {
    this->video_memory.set(row, col, value);
    }

  /** 
   * Užpildo nurodytą regioną.
   *
   * Regiono dalis, patekusi už ekrano ribų yra ignoruojama.
   *
   * @param row – regiono viršutinis kairiojo kampo eilutės numeris.
   * @param col – regiono viršutinis kairiojo kampo stulpelio numeris.
   * @param width – regiono plotis stulpeliais.
   * @param height – regiono plotis eilutėmis.
   */
  void fill(
      int row, int col, int width, int height, ScreenCharacter value) {

    for (int i = row; i < row + height && i < SCREEN_HEIGHT; i++) {
      for (int j = col; j < col + width && j < SCREEN_WIDTH; j++) {
        this->set_character(i, j, value);
        }
      }
    }

  /// Nurodytą regioną užpildo nurodytu simboliu.
  void fill(int row, int col, int width, int height, char value) {
    this->fill(
        row, col, width, height, 
        ScreenCharacter(
          value, this->foreground_color, this->background_color));
    }

  /// Visą ekraną užpildo numatytąja fono spalva.
  void clear() {
    this->fill(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x20);
    this->cursor.move(0, 0);
    }

  /// Pastumia visas eilutes per vieną į viršų.
  void scroll() {

    for (int i = 1; i < SCREEN_HEIGHT; i++) {
      for (int j = 0; j< SCREEN_WIDTH; j++) {
        ScreenCharacter *sc = this->video_memory.get_pointer(i, j);
        this->video_memory.set(i-1, j, *sc);
        // *sc = this->video_memory.get(i, j); 
        // Nesikompiliuoja, nes g++ reikalauja, kad būtų realizuota funkcija
        // memmove. Žr.:
        // http://unix.derkeiler.com/Mailing-Lists/FreeBSD/hackers/
        // 2009-01/msg00224.html
        }
      }

    this->fill(SCREEN_HEIGHT-1, 0, SCREEN_WIDTH, 1, ' ');
    }

  /// Į žymeklio poziciją įrašo nurodytą simbolį.
  void put_character(char value) {
    this->cursor.put(value);
    }

  /// Išveda „null-terminated“ simbolių eilutę.
  void write_string(const char *value) {
    for (const char *c = value; *c; c++) {
      this->put_character(*c);
      }
    }

  /** 
   * Išveda skaičių šešioliktainiu formatu.
   *
   * @param number – išvedamas skaičius.
   * @param size – kiek žemiausių number bitų atspausdinti.
   */
  void write_hex(u64int number, u8int size) {

    for (int i = size - 4; i >= 0; i -= 4) {
      u8int digit = (number >> i) & 0xF;
      if (digit >= 10) {
        this->put_character(digit - 10 + 'A');
        }
      else {
        this->put_character(digit + '0');
        }
      }

    }

  /// Išveda skaičių šešioliktainiu formatu.
  void write_hex(u32int number) {
    this->write_hex((u64int) number, 32);
    }

  /// Išveda skaičių šešioliktainiu formatu.
  void write_hex(s32int number) {
    this->write_hex((u64int) number, 32);
    }

  /// Išveda skaičių šešioliktainiu formatu.
  void write_hex(u64int number) {
    this->write_hex((u64int) number, 64);
    }

  /// Išveda skaičių šešioliktainiu formatu.
  void write_hex(s64int number) {
    this->write_hex((u64int) number, 64);
    }

  /// Išveda skaičių dešimtainiu formatu.
  void write_dec(s32int number) {
    if (number < 0) {
      this->put_character('-');
      this->write_dec(((u32int) -number));
      }
    else {
      this->put_character('+');
      this->write_dec(((u32int) number));
      }
    }

  /// Išveda skaičių dešimtainiu formatu.
  void write_dec(u32int number) {
    int length = 1;
    for (int i = number; i; i /= 10) {
      length *= 10;
      }
    if (length > 1) {
      length /= 10;
      }
    for (; length; length /= 10) {
      this->put_character('0' + ((number) / length) % 10);
      }
    }

  };

#endif
