#ifdef USE_IDOUBLE
#include "idouble.h"
#define double idouble
#endif

extern int _testfunc_i_bhilfd(char b, short h, int i, long l, float f, double d);
#ifdef USE_IDOUBLE
#undef double
#endif

