#ifndef POINT_H
#define POINT_H


/**
 * Pora (taškas) su standartiniu skaičiumi suderinamų kintamųjų.
 */
template <class T> struct Point {

// Atributai.

  T x;
  T y;

// Metodai.

  Point(): x(0), y(0) { }
  Point(T _x, T _y): x(_x), y(_y) { }
  
  };


#endif
