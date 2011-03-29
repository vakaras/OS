/**
 * common.h – įvairios pagalbinės dažnai naudojamos funkcijos.
 */

#ifndef COMMON_H
#define COMMON_H

#define BYTE_SIZE 8

// 64 bitų dydžiai.
typedef unsigned  long    u64int;
typedef           long    s64int;

// 32 bitų dydžiai.
typedef unsigned  int     u32int;
typedef           int     s32int;
typedef unsigned  short   u16int;
typedef           short   s16int;
typedef unsigned  char    u8int;
typedef           char    s8int;

// Pasiųsti baitą į nurodytą portą.
void send_byte(u16int port, u8int value);
// Nuskaityti baitą iš nurodyto porto.
u8int get_byte(u16int port);
// Nuskaityti du baitus iš nurodyto porto.
u16int get_bytes2(u16int port);

// Pora (Taškas) su standartiniu skaičiumi suderinamų kintamųjų.
template <class T> struct Point {

// Atributai.

  T x;
  T y;

// Metodai.

  Point(): x(0), y(0) { }
  Point(T _x, T _y): x(_x), y(_y) { }
  
  };


/**
 * Rodyklė į dvimatį masyvą.
 *
 * Taikant analogiją su paprastu C++ masyvu, eilutės ir stulpeliai
 * suprantami taip:
 *
 * T a[eilučių_kiekis][stulpelių_kiekis];
 */
template <class T> class Array2dPointer {

private:

// Atributai.

  T *start;                             // Pirmojo masyvo elemento adresas.
  T *end;                               // Pirmojo baito po paskutinio
                                        // masyvo elemento adresas.

  int width;                            // Vienos „eilutės“ ilgis.
  int height;                           // Kiek iš viso eilučių yra.

  int element_size;                     // Elemento dydis baitais.

public:

  Array2dPointer(T *start, int width, int height) {

    // TODO: Pridėti patikrinimą ar perduoti teisingi argumentai.

    this->start = start;
    this->end = start + width * height;
    this->width = width;
    this->height = height;
    this->element_size = sizeof(*this->start);
    }

  int get_width() {
    return this->width;
    }
  
  int get_height() {
    return this->height;
    }

  // Gražina rodyklę į nurodytąjį elementą.
  T* get_pointer(int row, int col) {
    // TODO: Idėti patikrinimą ar perduoti teisingi parametrai.
    return this->start + row * this->width + col;
    }

  // Gražina nurodytąjį elementą.
  T get(int row, int col) {
    return *(this->get_pointer(row, col));
    }
  
  // Nustato nurodytojo elemento reikšmę.
  void set(int row, int col, T value) {
    *(this->get_pointer(row, col)) = value;
    }

  };

// Nukopijuoja nurodytus len baitų iš src į dest.
void memcpy(u8int *dest, const u8int *src, u32int length);

// Užpildo nurodytą atmintį reikšme value.
void memset(u8int *dest, u8int value, u32int length);

#endif
