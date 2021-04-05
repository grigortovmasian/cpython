#ifndef CAAD_IINT_H
#define CAAD_IINT_H

#include "aadc/idouble.h"
#include "aadc/ibool.h"

#include <algorithm>

extern "C" AADC_API void CAAD_iIntVarConstructor(int64_t*);
extern "C" AADC_API void CAAD_iIntVarConstructorConstant(int64_t*, const int64_t*);

extern "C" AADC_API void CAAD_iIntVarDestructor(int64_t*);

extern "C" AADC_API void CAAD_iIntVarAssign(const int64_t* in, int64_t* out);

extern "C" AADC_API void CAAD_iIntVarAdd(const int64_t* in1, const int64_t* in2, int64_t* out);
extern "C" AADC_API void CAAD_iIntVarSub(const int64_t* in1, const int64_t* in2, int64_t* out);
extern "C" AADC_API void CAAD_iIntVarMult(const int64_t* in1, const int64_t* in2, int64_t* out);

extern "C" AADC_API void CAAD_iIntArrayGet(const double* arr_start, int64_t step, int64_t size, const int64_t* in, double* out);
extern "C" AADC_API void CAAD_iIntLowerBound(const double* arr_start, int64_t step, int64_t size, const double* in, int64_t* out);

extern "C" AADC_API void CAAD_iIntVarCmpEqual(const int64_t* in1, const int64_t* in2, uint64_t* out);
extern "C" AADC_API void CAAD_iIntVarCmpGreater(const int64_t* in1, const int64_t* in2, uint64_t* out);

extern "C" AADC_API void CAAD_iIntVarIf(const uint64_t* cond, const int64_t* in1, const int64_t* in2, int64_t* out);

extern "C" AADC_API void CAAD_iIntVarDouble2Int(const double* in, int64_t* out);

class iint {
public:
    iint(const int64_t& c) : val(c) { if(AADC_UNLIKELY(idouble::recording)) CAAD_iIntVarConstructorConstant(&val, &c); }
    iint() { if(AADC_UNLIKELY(idouble::recording)) CAAD_iIntVarConstructor(&val); }
    iint(const iint& other) : val(other.val) { if(AADC_UNLIKELY(idouble::recording)) CAAD_iIntVarAssign(&other.val, &val); }
    ~iint() { if(AADC_UNLIKELY(idouble::recording)) CAAD_iIntVarDestructor(&val); }

    iint& operator = (const iint& other) {
        if(AADC_UNLIKELY(idouble::recording)) CAAD_iIntVarAssign(&(other.val), &val);
        val = other.val;
        return *this;
    }

    // TODO: Remove from production code
    uint64_t varIndex() {
        return CAAD_iVarIndex((double*)&val);
    }

public:
    int64_t val;
};

inline iint operator + (const iint& a, const iint& b) {
    iint c;
    if(AADC_UNLIKELY(idouble::recording)) CAAD_iIntVarAdd(&a.val, &b.val, &c.val);
    c.val = a.val + b.val;
    return c;
}
inline iint operator - (const iint& a, const iint& b) {
    iint c;
    if(AADC_UNLIKELY(idouble::recording)) CAAD_iIntVarSub(&a.val, &b.val, &c.val);
    c.val = a.val - b.val;
    return c;
}
inline iint operator * (const iint& a, const iint& b) {
    iint c;
    if(AADC_UNLIKELY(idouble::recording)) CAAD_iIntVarMult(&a.val, &b.val, &c.val);
    c.val = a.val * b.val;
    return c;
}

// extern "C" AADC_API void CAAD_iIntArrayGet(const double* arr_start, int64_t step, int64_t size, const int64_t* in, double* out);

namespace aadcArrayOps {
template<class A>
idouble get(const A& arr, const iint& i) {
    idouble c;
    if(AADC_UNLIKELY(idouble::recording)) {
        int64_t size = arr.size();
        CAAD_iIntArrayGet(&(arr[0].val), &(arr[1].val) - &(arr[0].val), size, &i.val, &c.val);
    }
    c.val = arr[i.val].val;
    return c;
}

template<class A>
const A& get(const std::vector<A>& arr, const int64_t& i) {
    return arr[i];
}

}; // namespace aadcArrayOps
// extern "C" AADC_API void CAAD_iIntLowerBound(const double* arr_start, int64_t step, int64_t size, const double* in, int64_t* out);
namespace aadc {
// specialized version of lower_bound specifically for idouble
template<typename _ForwardIterator>
iint lower_bound(_ForwardIterator __first, _ForwardIterator __last, const idouble& x)
{
    iint pos;
    if(AADC_UNLIKELY(idouble::recording)) {
        auto next(__first); next++;
        size_t step = &(*next) - &(*__first);
        size_t size = __last - __first;
        CAAD_iIntLowerBound(&(*__first), step, size, &x.val, &pos.val);
    }
    pos.val = std::lower_bound(__first, __last, x.val) - __first;
    return pos;

}

template<class A>
iint lower_bound(const A& arr, const idouble& x) {
    return lower_bound(arr.begin(), arr.end(), x);
}

template<class A>
int64_t lower_bound(const A& arr, const double& x) {
    int64_t pos;
    pos = int64_t(std::lower_bound(arr.begin(), arr.end(), x) - arr.begin());
    return pos;
}

inline iint toInt(const idouble& x) {
    iint pos;
    if(AADC_UNLIKELY(idouble::recording)) {
        CAAD_iIntVarDouble2Int(&x.val, &pos.val);
    }
    pos.val = (int)(x.val + 0.5); // rounding to the nearest

    return pos;
}

inline int toInt(const double& x) {
    return (int)x;
}
}; // namespace aadc

inline ibool operator > (const iint& a, const iint& b) {
    ibool c;
    if(AADC_UNLIKELY(idouble::recording)) CAAD_iIntVarCmpGreater(&a.val, &b.val, &c.val);
    c.val = a.val > b.val;
    return c;
}

inline ibool operator == (const iint& a, const iint& b) {
    ibool c;
    if(AADC_UNLIKELY(idouble::recording)) CAAD_iIntVarCmpEqual(&a.val, &b.val, &c.val);
     c.val = a.val == b.val;
    return c;
}

inline iint iIf(const ibool& cond, const iint& a, const iint& b) {
    iint c;
    if(AADC_UNLIKELY(idouble::recording)) CAAD_iIntVarIf(&cond.val, &a.val, &b.val, &c.val);
    c.val = cond.val ? a.val : b.val;
    return c;
}

inline iint max(const iint& a,const iint& b) {
    return iIf(a > b, a, b);
}
inline iint min(const iint& a,const iint& b) {
    return iIf(a > b, b, a);
}

#endif // CAAD_IINT_H