#include "icmath.h"

//defining function here to avoid multiply definitions during linking
//delete this when this functions will be added in libaadc.so

idouble hypot(idouble x, idouble y) {
	return  std::hypot(AAD_PASSIVE(x),AAD_PASSIVE(y));	
}

bool isinf (idouble x) {
	return std::isinf(AAD_PASSIVE(x));
}

idouble modf(idouble x, idouble * intpart) {
	int i = (int)AAD_PASSIVE(x);	
	if(intpart) {
		intpart->val=i;
	}
        return AAD_PASSIVE(x) - i;
}

int idoubleToInt(idouble x) {
	return (int)AAD_PASSIVE(x);

}

float idoubleTofloat(idouble x) {
	return (float)AAD_PASSIVE(x);
}


bool idoubleToBool(idouble x) {
	return (bool)AAD_PASSIVE(x);
}



idouble frexp(idouble x, int* i) {
	return std::frexp(AAD_PASSIVE(x),i);
}

bool isnan(idouble x) {
	return std::isnan(AAD_PASSIVE(x));
}

idouble round(idouble x){
	return std::round(AAD_PASSIVE(x));
}
 
idouble ldexp(idouble x, int exp) {
	return std::ldexp(AAD_PASSIVE(x),exp);
}

idouble nextafter(idouble x , idouble y ) {
	return std::nextafter(AAD_PASSIVE(x),AAD_PASSIVE(y));
}

