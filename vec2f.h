/*
  Copyright (c) 2013 Auston Sterling
  See license.txt for copying permission.
  
  -----Vec2f Class-----
  Auston Sterling
  austonst@gmail.com

  A class for storing and performing operations on a 2D vector.
  Originally based off of Justin Legakis' vectors.h.
*/

#ifndef _vec2f_h_
#define _vec2f_h_

#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Vec2f
{
 public:
  //Constructors
  Vec2f() {data[0] = data[1] = 0;}
  Vec2f(const Vec2f &V) {
    data[0] = V.data[0];
    data[1] = V.data[1]; }
  Vec2f(double d0, double d1) {
    data[0] = d0;
    data[1] = d1; }

  //Assignment operator
  const Vec2f& operator=(const Vec2f &V) {
    data[0] = V.data[0];
    data[1] = V.data[1];
    return *this; }

  //Simple accessors and modifiers
  double operator[](int i) const {
    if (i >= 0 && i < 2) return data[i];
    return 0; }
  double x() const {return data[0];}
  double y() const {return data[1];}
  void setx(double x) {data[0] = x;}
  void sety(double y) {data[1] = y;}
  void set(double d0, double d1) {
    data[0] = d0;
    data[1] = d1; }

  //Common vector operations
  double length() const {return sqrt(data[0]*data[0]+data[1]*data[1]);}
  void normalize() {
    double d = length();
    if (d > 0) scale(1/d); }
  void scale(double d) {scale(d,d);}
  void scale(double d0, double d1) {
    data[0] *= d0;
    data[1] *= d1;}
  void negate() {scale(-1);}
  double dot2(const Vec2f &V) const {
    return data[0]*V.data[0] + data[1]*V.data[1]; }
  double angleBetween(const Vec2f &V) const {
    return acos(dot2(V) / (length()*V.length())); }

  //Vector math operations
  Vec2f& operator+=(const Vec2f &V) {
    data[0] += V.data[0];
    data[1] += V.data[1];
    return *this; }
  Vec2f& operator-=(const Vec2f &V) {
    data[0] -= V.data[0];
    data[1] -= V.data[1];
    return *this; }
  Vec2f& operator*=(double d) {
    data[0] *= d;
    data[1] *= d;
    return *this; }
  Vec2f& operator/=(double d) {
    data[0] /= d;
    data[1] /= d;
    return *this; }
  friend Vec2f operator+(const Vec2f &v1, const Vec2f &v2) {
    Vec2f v3 = v1; v3 += v2; return v3; }
  friend Vec2f operator-(const Vec2f &v1, const Vec2f &v2) {
    Vec2f v3 = v1; v3 -= v2; return v3; }
  friend Vec2f operator-(const Vec2f &V) {
    Vec2f v2 = V; v2.negate(); return v2; }
  friend Vec2f operator*(const Vec2f &v1, double d) {
    Vec2f v2 = v1; v2.scale(d); return v2; }
  friend Vec2f operator*(double d, const Vec2f &v1) {
    return v1 * d; }

 private:
  //Representation
  double data[2];
};
  
#endif
