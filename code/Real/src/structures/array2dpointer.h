#ifndef ARRAY_2D_POINTER_H
#define ARRAY_2D_POINTER_H 1


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

#endif
