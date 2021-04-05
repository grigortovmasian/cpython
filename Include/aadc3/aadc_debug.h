#pragma once
#include "aadc/aadc_ext.h"
#include "aadc/idouble.h"

#include <iostream>
#include <sstream>
#include <iomanip>

void AADDebugStart(double adjoint_print_scaler = 1.0, int aadc_debug_avx_index_ = 0);

bool AADDebugNeedBumpBase();
void AADDebugStop();
void AADDebugResult(double& res);

double AADCDebugAdj(const double& x, const char *var_name, const char* file_name, int line_num, double bump_h);
idouble AADDebugResult(idouble res);

extern "C" AADC_API void CAAD_CPUState(
    bool forward, 
    const char* name0 = 0, const double* var0 = 0,
    const char* name1 = 0, const double* var1 = 0,
    const char* name2 = 0, const double* var2 = 0,
    const char* name3 = 0, const double* var3 = 0,
    const char* name4 = 0, const double* var4 = 0,
    const char* name5 = 0, const double* var5 = 0,
    const char* name6 = 0, const double* var6 = 0,
    const char* name7 = 0, const double* var7 = 0,
    const char* name8 = 0, const double* var8 = 0,
    const char* name9 = 0, const double* var9 = 0
);

extern "C" AADC_API void CAAD_BinaryBreakPoint();

uint64_t& AADCGetDebugVarCounter();
uint64_t& AADCGetDebugPrintAVXIndex();

bool AADCIsDebugPrintNow();
double AADCDebugPrintScaler();
namespace aadc {

class DebugPrintExtFuncWrapper : public aadc::ConstStateExtFunc {
public:
  DebugPrintExtFuncWrapper(idouble& res, const idouble& x, const char *var_name_, const char* file_name_, int line_num_)
    : is_random(CAAD_iVarIsRandom(&(x.val)))
    , is_diff(CAAD_iVarIsDiff(&(x.val)))
    , xi(x)
    , resi(res)
    , counter(AADCGetDebugVarCounter())
    , var_name(var_name_)
    , file_name(file_name_)
    , line_num(line_num_)
  {
    res.val = x.val;
    std::stringstream sstr;
    sstr << "AADCDebugFwd::" << std::right << std::setw(10) << std::setfill('0') << counter 
        << ":" << var_name << "_rec =" 
        << x.val << " "
        << (is_random ? "[rv] " : "")
        << (is_diff ? "[adj] " : "")
        << "@ " << file_name << ":" << line_num
        << std::endl
    ;
    std::cout << sstr.str();
    AADCGetDebugVarCounter()++;
  }

  template<typename mmType>
  void forward(mmType* v) const {
    v[resi] = v[xi];
    if (AADCIsDebugPrintNow()) {
     std::stringstream sstr;
     sstr << "AADCDebugFwd::" << std::right << std::setw(10) << std::setfill('0') << counter 
        << ":" << var_name << "_aadc=" 
        << toArray(v[resi])[AADCGetDebugPrintAVXIndex()]
        << " @ " << file_name << ":" << line_num
        << std::endl;
        std::cout << sstr.str();
    }
  }
  template<class mmType>
  void reverse(const mmType *v, mmType *d) const {
      d[xi] = aadc::mmAdd(d[xi], d[resi]);
      if (AADCIsDebugPrintNow()) {
        std::stringstream sstr;
        sstr << "AADCDebugAdj::" << std::right <<std::setw(10) << std::setfill('0') << counter 
            << ":" << var_name << "_ad=" 
            << AADCDebugPrintScaler() * toArray(d[resi])[AADCGetDebugPrintAVXIndex()]
            << " @ " << file_name << ":" << line_num
            << std::endl
        ;
        std::cout << sstr.str();
      }
  }

private:
  const bool is_random;
  const bool is_diff;
  const ExtVarIndex xi;
  const ExtVarIndex resi;
  const int counter;
  const char *var_name;
  const char* file_name;
  const int line_num;
};

};

inline idouble AADCDebugAdj(const idouble& x, const char *var_name, const char* file_name, int line_num, double bump_h) {
    if (!idouble::recording) 
    {
      return AADCDebugAdj(x.val, var_name, file_name, line_num, bump_h);
    }
    idouble res;

    aadc::addConstStateExtFunction(
        std::make_shared<aadc::DebugPrintExtFuncWrapper>(
            res, x, var_name, file_name, line_num
        )
    );

    return res;
}

#define AADC_PRINT(var_name) \
AADCDebugAdj(var_name, #var_name, __FILE__, __LINE__, 0.00001)



inline void AADC_CPUState(bool forward, 
    const char* name0 = 0, const double* var0 = 0,
    const char* name1 = 0, const double* var1 = 0,
    const char* name2 = 0, const double* var2 = 0,
    const char* name3 = 0, const double* var3 = 0,
    const char* name4 = 0, const double* var4 = 0,
    const char* name5 = 0, const double* var5 = 0,
    const char* name6 = 0, const double* var6 = 0,
    const char* name7 = 0, const double* var7 = 0,
    const char* name8 = 0, const double* var8 = 0,
    const char* name9 = 0, const double* var9 = 0
) {

}

inline void AADC_CPUState(bool forward, 
    const char* name0 = 0, const idouble* var0 = 0,
    const char* name1 = 0, const idouble* var1 = 0,
    const char* name2 = 0, const idouble* var2 = 0,
    const char* name3 = 0, const idouble* var3 = 0,
    const char* name4 = 0, const idouble* var4 = 0,
    const char* name5 = 0, const idouble* var5 = 0,
    const char* name6 = 0, const idouble* var6 = 0,
    const char* name7 = 0, const idouble* var7 = 0,
    const char* name8 = 0, const idouble* var8 = 0,
    const char* name9 = 0, const idouble* var9 = 0
) {
  CAAD_CPUState(forward,
    name0, (double*)var0,
    name1, (double*)var1,
    name2, (double*)var2,
    name3, (double*)var3,
    name4, (double*)var4,
    name5, (double*)var5,
    name6, (double*)var6,
    name7, (double*)var7,
    name8, (double*)var8,
    name9, (double*)var9    
  );
}

#define AADC_CPU_STATE(forward, var1, var2, var3) \
AADC_CPUState(forward, \
#var1, &(var1), \
#var2, &(var2), \
#var3, &(var3) \
)
