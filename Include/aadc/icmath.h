#ifndef ICMATH_HEADER_H
#define ICMATH_HEADER_H

#include "idouble.h"
#include <cmath>

idouble hypot(idouble x, idouble y) ;

bool isinf (idouble x) ;

idouble modf(idouble x, idouble * intpart) ;

int idoubleToInt(idouble x);

float idoubleTofloat(idouble x);

bool idoubleToBool(idouble x) ;

idouble frexp(idouble x, int* i) ;

bool isnan(idouble x);

idouble round(idouble x);
 
idouble ldexp(idouble x, int exp);

idouble nextafter(idouble x , idouble y );

#endif