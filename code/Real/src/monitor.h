/**
 * Ekrano valdymo klasė.
 */
#ifndef MONITOR_H
#define MONITOR_H

#define SCREEN_HEIGHT 25
#define SCREEN_WIDTH 80

// Spalvos.
#define COLOR_BLACK         0x0
#define COLOR_BLUE          0x1
#define COLOR_GREEN         0x2
#define COLOR_CYAN          0x3
#define COLOR_RED           0x4
#define COLOR_MAGENTA       0x5
#define COLOR_BROWN         0x6
#define COLOR_LIGHT_GREY    0x7
#define COLOR_DARK_GREY     0x8
#define COLOR_LIGHT_BLUE    0x9
#define COLOR_LIGHT_GREEN   0xA
#define COLOR_LIGHT_CYAN    0xB
#define COLOR_LIGHT_RED     0xC
#define COLOR_LIGHT_MAGENTA 0xD
#define COLOR_LIGHT_BROWN   0xE
#define COLOR_WHITE         0xF

#define FOREGROUND 0x0f                 // TODO: Ištrinti
#define BACKGROUND 0xf0

#include "common.h"


struct ScreenCharacter{

// Atributai.
  char character;
  u8int
    foreground_color : 4,
    background_color : 4;

// Metodai.
  
  ScreenCharacter() { 
    this->character = 0x20;             // Tarpo simbolis.
    this->foreground_color = COLOR_WHITE;
    this->background_color = COLOR_BLACK;
    }

  ScreenCharacter(
      char character, 
      u8int foreground_color=COLOR_WHITE, 
      u8int background_color=COLOR_BLACK) {
    this->character = character;
    this->foreground_color = foreground_color;
    this->background_color = background_color;
    }
  
  };


class Monitor {

// Atributai.
private:

  Array2dPointer<ScreenCharacter> video_memory;
                                        // Video atminties blokas.
  u8int foreground_color;               // Teksto spalva. (Reikšmė nuo
                                        // 0x0 iki 0xF.)
  u8int background_color;               // Fono spalva. (Reikšmė nuo
                                        // 0x0 iki 0xF.)

  class Cursor {

    // Atributai.
    private:

    u8int row, col;                     // Žymeklio pozicija.
    Monitor &monitor;                   // Nuoroda į ekrano objektą.

    // Metodai.

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

    // Užrašo simbolį ir paslenka žymeklį.
    void put(char character) {
      this->monitor.set_character(this->row, this->col, character);
      // TODO: Užbaigti, kad apdorotų \n \r \b ir t.t.
      this->inc();
      }
    
    } cursor;

// Metodai.

public:

  Monitor(): 
    video_memory((ScreenCharacter *)0xB8000, SCREEN_WIDTH, SCREEN_HEIGHT),
    cursor(*this) {

    this->foreground_color = COLOR_WHITE;
    this->background_color = COLOR_BLACK;

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
        //*sc = this->video_memory.get(i, j); FIXME: Išsiaiškinti, kodėl
        // nesikompiliuoja.
        this->video_memory.set(i-1, j, *sc);
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

  };



class Monitor2 {

// Atributai.

private:

  // Rodyklė į VGA atmintį.
  u16int *memory;

  // Naujo teksto spalva.
  u8int color;

  Point<u16int> cursor;

// Metodai.

  // Grąžina kursoriaus padėti poslinkiu.
  u16int get_offset() {
    return this->cursor.x * SCREEN_WIDTH + this->cursor.y;
    }

  // Grąžina dvibaitį, kurio viršutiniame baite yra spalva.
  u16int get_color() {
    return this->color << BYTE_SIZE;
    }

  // Nurodytai ekrano eilutei priskiria perduotąją.
  void set_line(u8int number, const u16int line[]) {

    for (int i = 0; i < SCREEN_WIDTH && line[i]; i++) {
      this->memory[number * SCREEN_WIDTH + i] = line[i];
      }

    }

  // Atnaujina žymeklį (hardware cursor).
  void move_cursor() {

    u16int offset = this->get_offset();

    send_byte(0x3D4, 14);               // Nurodymas, kad siųsime 
                                        // viršutinį baitą.
    send_byte(0x3D5, offset >> BYTE_SIZE);
                                        // Išsiunčiame viršutinį baitą.
    send_byte(0x3D4, 15);               // Nurodymas, kad siųsime
                                        // apatinį baitą.
    send_byte(0x3D5, offset);           // Išsiunčiame apatinį baitą.

    }

  // Pastumia tekstą per vieną eilutę.
  void scroll() {

    // Nusistatome tarpą, su numatytaja spalva.
    u16int blank = 0x20 | this->get_color();

    // Pasiekėme paskutinę eilutę, reikia pastumti ekraną.
    if (this->cursor.y >= (SCREEN_HEIGHT - 1)) {

      // Pastumiame visas eilutes per vieną į viršų.
      for (int i = 0; i < SCREEN_HEIGHT - 1; i++) {
        this->set_line(i, this->memory + (i * SCREEN_WIDTH));
        }

      // Išvalome paskutinę eilutę.
      for (int i = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; 
          i < SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
        this->memory[i] = blank;
        }

      this->cursor.y = SCREEN_HEIGHT - 1;

      }

    }

public:

  Monitor2() {
    this->memory = (u16int *)0xB8000;
    this->color = (FOREGROUND & COLOR_WHITE) | (BACKGROUND & COLOR_BLACK);
    }
  
  void set_foreground_color(u8int color) {
    this->color = (FOREGROUND & color) | (BACKGROUND & this->color);
    }
  
  void set_background_color(u8int color) {
    this->color = (FOREGROUND & this->color) | (BACKGROUND & this->color);
    }

  // Užrašyti nurodytąjį simbolį c ekrane.
  void put(char c) {

    if (c == '\b' && this->cursor.x) {
      this->cursor.x--;
      }
    else if (c == '\t') {
      this->cursor.x = (this->cursor.x + 8) & ~(8 - 1);
      }
    else if (c == '\r') {
      this->cursor.x = 0;
      }
    else if (c == '\n') {
      this->cursor.x = 0;
      this->cursor.y++;
      }
    else if (c >= ' ') {
      *(this->memory + this->get_offset()) = this->get_color() | c;
      this->cursor.y++;
      }

    if (this->cursor.x >= SCREEN_WIDTH) {
      this->cursor.x = 0;
      this->cursor.y++;
      }

    this->scroll();
    this->move_cursor();
    
    }

  // Išvalyti ekraną;
  void clear() {

    // Nusistatome tarpą, su numatytaja spalva.
    u16int blank = 0x20 | this->get_color();

    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
      this->memory[i] = blank;
      }
    
    this->cursor.x = 0;
    this->cursor.y = 0;
    this->move_cursor();

    }

  // Užrašyti simbolių eilutę, kurios pabaigos požymis yra 0, ekrane.
  void write(const char *str) {

    for (const char *c = str; c; c++) {
      this->put(*c);
      }

    }

  };




#endif
