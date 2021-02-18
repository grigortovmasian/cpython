#include <stdlib.h>
#include "idouble.h"
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t lock;

struct idouble  CREATE_NEW_IDOUBLE(double d) {
    struct  idouble ret;
    ret.val = (double*)malloc(sizeof(double));
    *(ret.val) = d;
    printf("Create\n");	
    return ret;
}

void  DELETE_IDOUBLE(struct idouble* l) {
    pthread_mutex_lock(&lock);
    free(l->val);
    printf("Delete\n");
    pthread_mutex_unlock(&lock);
}


void CAAD_iVarAdd(double* l, double* r, double* res) {
    *res = *l + *r;
}

struct idouble ADD_OP(const struct idouble* l, const struct idouble* r) {
    struct  idouble res = CREATE_NEW_IDOUBLE(0);
    CAAD_iVarAdd(l->val, r->val, res.val);
    return res;
}

