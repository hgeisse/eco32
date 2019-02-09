/*
 * f2d.c -- convert single-precision fp calls to double precision
 */


#include <math.h>

#include "f2d.h"


float sinf(float x) {
  return (float) sin((double) x);
}


float cosf(float x) {
  return (float) cos((double) x);
}


float tanf(float x) {
  return (float) tan((double) x);
}


float asinf(float x) {
  return (float) asin((double) x);
}


float acosf(float x) {
  return (float) acos((double) x);
}


float atanf(float x) {
  return (float) atan((double) x);
}


float atan2f(float y, float x) {
  return (float) atan2((double) y, (double) x);
}


float sinhf(float x) {
  return (float) sinh((double) x);
}


float coshf(float x) {
  return (float) cosh((double) x);
}


float tanhf(float x) {
  return (float) tanh((double) x);
}


float expf(float x) {
  return (float) exp((double) x);
}


float logf(float x) {
  return (float) log((double) x);
}


float log10f(float x) {
  return (float) log10((double) x);
}


float powf(float x, float y) {
  return (float) pow((double) x, (double) y);
}


float sqrtf(float x) {
  return (float) sqrt((double) x);
}
