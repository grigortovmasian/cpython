#ifndef IDOUBLE_H
#define IDOUBLE_H

struct idouble {
    double* val;
};

struct idouble  CREATE_NEW_IDOUBLE(double d);

struct  idouble  COPY_IDOUBLE(struct idouble* l);

void  DELETE_IDOUBLE(struct idouble* l);


void CAAD_iVarAdd(double* l, double* r, double* res);

struct idouble ADD_OP(const struct idouble* l, const struct idouble* r);

#endif //IDOUBLE_H
