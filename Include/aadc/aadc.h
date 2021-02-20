#ifndef _AADC_H_
#define _AADC_H_

#include <memory>
#include <chrono>
#include <aadc/idouble.h>
#include <aadc/ibool.h>
#include <aadc/iint.h>
#include <aadc/aadc_tools.h>
#include "aadc/aadc_ext.h"
#include <aadc/tools/runtime.h>
#include <aadc/aadc_thread_sync.h>

typedef uint64_t AADCOpCounter;

extern "C" AADC_API AADCOpCounter CAAD_GetCurrentOpIndex();
extern "C" AADC_API void CAAD_ResetOptions();
extern "C" AADC_API void CAAD_SetOption(uint64_t option, uint64_t val);

extern "C" AADC_API void CAAD_WorkSpaceSetArray(int mmTypeSize, void* v, void* array_begin);

extern "C" AADC_API uint64_t CAAD_GetNumCheckPoints();
extern "C" AADC_API void CAAD_GetCheckPointCodeBlocks(uint64_t* check_point_blocks_indx);
extern "C" AADC_API uint64_t CAAD_GetNumCheckPointVars(uint64_t check_point_indx);
extern "C" AADC_API void CAAD_GetCheckPointVars(uint64_t check_point_indx, uint64_t* check_point_var_i);

extern "C" AADC_API int CAAD_iVarNumExtractPassive();

extern "C" AADC_API const char* CAAD_iVarGetExtractPassiveLocation(uint64_t loc_index);
extern "C" AADC_API uint64_t CAAD_iVarGetExtractPassiveLocationLine(uint64_t loc_index);

namespace aadc {

enum CompilerOption {
    AADC_MemOptimizations,
    AADC_RValueOptimizations,
    AADC_UseCompressedCode,
    AADC_NumCompressorThreads,
    AADC_CodeBufferSize,
    AADC_CodeCompressorFactor,
    AADC_CodeCompressorOrderFreeIndex,
    AADC_ReuseConstants,
    AADC_MaxNumVariables,
    AADC_InitInputDiff,
    AADC_ReleaseUncompressedCode,
    AADC_SingleAdjointPass,
    AADC_LAST_OPTION
};

template<typename mmType> class AADCFunctions;

template<typename mmType>
class AADCWorkSpace {

friend class AADCFunctions<mmType>;

public:
    AADCWorkSpace(const int64_t& work_array_size, const int64_t& m_stack_size)
        : m_v(work_array_size)
        , m_d(work_array_size)
        , m_stack(m_stack_size)
    {
      resetDiff();
    }
    AADCWorkSpace(
        const int64_t& work_array_size, const int64_t& m_stack_size
        , const std::vector<uint64_t> num_checkpoint_vars
    )
        : m_v(work_array_size)
        , m_d(work_array_size)
        , m_stack(m_stack_size)
        , m_check_points(num_checkpoint_vars.size())
    {
      resetDiff();
      for (int i = 0; i < m_check_points.size(); ++i) {
          m_check_points[i].resize(num_checkpoint_vars[i]);
      }
    }
    AADCWorkSpace(const AADCWorkSpace& other)
        : m_v(other.m_v)
        , m_d(other.m_d)
        , m_stack(other.m_stack)
        , m_check_points(other.m_check_points)
        , m_rev_runs_after_fwd(other.m_rev_runs_after_fwd)
    {}
    ~AADCWorkSpace() {}
public:
    void setArray(const mmType* array) {
        CAAD_WorkSpaceSetArray(sizeof(mmType), (void*)&(m_v[0]), (void*)array);
    }
    AADCWorkSpace& setVal(const AADCArgument& var_indx, const mmType& val) {
        m_v[var_indx.getIndex()] = val;
        return *this;
    }
    AADCWorkSpace& setVal(const AADCArgument& var_indx, const double val) {
        m_v[var_indx.getIndex()] = mmSetConst<mmType>(val);
        return *this;
    }
    AADCWorkSpace& setVal(const AADCScalarArgument& var_indx, const double val) {
        double* v((double*)(&m_v[0]));
        v[var_indx.getIndex()] = val;
        return *this;
    }

    template<typename IdxIter, typename ValIter>
    void setVal(IdxIter indx, ValIter val, ValIter val_end) {
        for (; val != val_end; ++indx, ++val) { setVal(*indx, *val); }
    }

    double& val(const AADCScalarArgument& var_indx) {
        double* v((double*)(&m_v[0]));
        return v[var_indx.getIndex()];
    }

    mmType& val(const AADCArgument& var_indx) {
        return m_v[var_indx.getIndex()];
    }

    const mmType& val(const AADCArgument& var_indx) const {
        return m_v[var_indx.getIndex()];
    }

    double* valp(const AADCArgument& var_indx) {
        return (double*)&(m_v[var_indx.getIndex()]);
    }

    const double* valp(const AADCArgument& var_indx) const {
        return (double*)&(m_v[var_indx.getIndex()]);
    }

    mmType& val(const AADCResult& var_indx) {
        return m_v[var_indx.getIndex()];
    }

    const mmType& val(const AADCResult& var_indx) const {
        return m_v[var_indx.getIndex()];
    }

    double* valp(const AADCResult& var_indx) {
        return (double*)&(m_v[var_indx.getIndex()]);
    }

    const double* valp(const AADCResult& var_indx) const {
        return (double*)&(m_v[var_indx.getIndex()]);
    }

    mmType& diff(const AADCArgument& var_indx) {
        return m_d[var_indx.getIndex()];
    }
    double* diffp(const AADCArgument& var_indx) {
        return (double*)&(m_d[var_indx.getIndex()]);
    }
    double& diff(const AADCScalarArgument& var_indx) {
        double* d((double*)(&m_d[0]));
        return d[var_indx.getIndex()];
    }
	const mmType& diff(const AADCArgument& var_indx) const {
        return m_d[var_indx.getIndex()];
    }
    mmType& diff(const AADCResult& var_indx) {
        return m_d[var_indx.getIndex()];
    }
	const mmType& diff(const AADCResult& var_indx) const {
        return m_d[var_indx.getIndex()];
    }
    double* diffp(const AADCResult& var_indx) {
        return (double*)&(m_d[var_indx.getIndex()]);
    }

    void setDiff(const AADCResult& var_indx, const mmType& val) {
        m_d[var_indx.getIndex()] = val;
    }
    void setDiff(const AADCResult& var_indx, const double val) {
        m_d[var_indx.getIndex()] = mmSetConst<mmType>(val);
    }

	// reset all adjoint variables
    void resetDiff() {
        std::fill(m_d.begin(), m_d.end(), mmZero<mmType>());
    }
private:
    mmVector<mmType> m_v;
    mmVector<mmType> m_d;
    mmVector<mmType> m_stack;
    std::vector<mmVector<mmType> > m_check_points;
    uint64_t m_rev_runs_after_fwd;
};

template<typename mmType>
class AADCFunctions {
public:
    typedef void (* BinFunc)(mmType* v, mmType* d, const double* c, mmType* s);
    typedef std::vector<std::pair<CompilerOption, uint64_t>> AADCOptions;
public:
    AADCFunctions(const AADCOptions& options = AADCOptions()) 
        : m_options(options)
        , m_code_options(16)
    {
    }
    AADCFunctions(const AADCFunctions<mmType>& other) = delete;

    void setOption(CompilerOption option, uint64_t val) {
        m_options.push_back(std::pair<CompilerOption, uint64_t>(option, val));
    }
    void startRecording() {
        m_start_recording = std::chrono::high_resolution_clock::now();
        initOptions();
        if (CAAD_StartRecording() != 0) throw std::runtime_error("AADC license has expired");
        idouble::recording = true;
    }

    virtual void stopRecording() {
        CAAD_FinalizeRecording();
        m_work_array_size = CAAD_VDWorkArraySize();
        m_stack_size = CAAD_StackSize();
        m_const_data.resize(CAAD_ConstantsArraySize());
        double* c_vals = CAAD_ConstantsArray();
        std::copy(c_vals, c_vals + m_const_data.size(), m_const_data.begin());
        m_num_code_blocks = CAAD_GetNumCodeBlocks();
        m_func_forward.resize(m_num_code_blocks);
        m_func_reverse.resize(m_num_code_blocks);
        m_code_size_fwd = 0; m_code_size_rev = 0;
        for (uint64_t ci = 0; ci < m_num_code_blocks; ++ci) {
            uint64_t code_size_fwd = CAAD_GetCodeSize(true, ci);
            uint64_t code_size_rev = CAAD_GetCodeSize(false, ci);

            m_code_size_fwd += code_size_fwd; m_code_size_rev += code_size_rev;

            // TODO: go directly from ExtractCode to m_code
            std::vector<uint8_t > code(code_size_fwd);
            CAAD_ExtractCode(true, &(code[0]), ci);
            std::vector<uint8_t > rcode(code_size_rev);
            CAAD_ExtractCode(false, &(rcode[0]), ci);

//            std::cout << "CodeBlock : " << ci << " fwd code size : " << code_size_fwd 
//            << " rev code size : " << code_size_rev << std::endl;

            m_code.Add(&(m_func_forward[ci]), code);
            m_code.Add(&(m_func_reverse[ci]), rcode);
        }
        m_num_checkpoints = CAAD_GetNumCheckPoints();
        m_checkpoint_indx.resize(m_num_checkpoints);
        if (m_num_checkpoints > 0) CAAD_GetCheckPointCodeBlocks(&(m_checkpoint_indx.front()));
        m_op_code_func_start = CAAD_RecordingStartAt();
        m_op_code_func_end = CAAD_GetCurrentOpIndex()+1;

        m_checkpoint_vars.resize(m_num_checkpoints);m_num_checkpoint_vars.resize(m_num_checkpoints);
        m_total_num_checkpoint_vars = 0;
        for (int i = 0; i < m_num_checkpoints; ++i) {
            m_total_num_checkpoint_vars += m_num_checkpoint_vars[i] = CAAD_GetNumCheckPointVars(i);
            m_checkpoint_vars[i].resize(m_num_checkpoint_vars[i]);
            if (m_num_checkpoint_vars[i]> 0) CAAD_GetCheckPointVars(i, &(m_checkpoint_vars[i].front()));
        }
        m_ext_funcs_new = getRecordingConstStateExtFunctionRegistry();
        m_number_of_required_random_variables=CAAD_iVarNumberOfRequiredRandomVariables();
        m_stop_recording = std::chrono::high_resolution_clock::now();
        
        idouble::recording = false;
    }

	virtual ~AADCFunctions() {}

    AADCFunctions& operator= (const AADCFunctions& other) = delete;
    
    std::shared_ptr<AADCWorkSpace<mmType> > createWorkSpace() const {
        if (m_num_checkpoints == 0) {
            return std::make_shared<AADCWorkSpace<mmType> >(m_work_array_size, m_stack_size);
        } else {
            return std::make_shared<AADCWorkSpace<mmType> >(m_work_array_size, m_stack_size, m_num_checkpoint_vars);
        }
    }
    void forward(
        AADCWorkSpace<mmType>& workspace, int first_cp = 0, int last_cp = -1,
        std::shared_ptr<AADCFunctionSynchronizer> thread_syncer = 0
    ) {
        last_cp = last_cp == -1 ? int(m_num_checkpoints) : last_cp;
        if (m_num_checkpoints > 0) {
//            for (uint64_t cp = 0; cp <= m_num_checkpoints; ++cp) {
            for (uint64_t cp = first_cp; cp <= last_cp; ++cp) {
                uint64_t code_block_start = (cp == 0 ? 0 : m_checkpoint_indx[cp-1]);
                uint64_t code_block_end = (cp < m_num_checkpoints ? m_checkpoint_indx[cp] : getNumCodeBlocks());
                for (uint64_t ci = code_block_start; ci < code_block_end; ++ci) {
                    if (thread_syncer) thread_syncer->syncThreads();
                    ForwardFunc(ci)(
                        &(workspace.m_v.front()), &(workspace.m_d.front()),
                        &(m_const_data.front()), &(workspace.m_stack.front())
                    );
                }
                if (cp < m_num_checkpoints) {
                    saveCheckPointVars(cp, workspace);
                }
            }
        } else {
            for (uint64_t ci = 0; ci < getNumCodeBlocks(); ++ci) {
//                if (ext_func_call_iter < m_ext_func_code_block_index.size()) {
//                    if (m_ext_func_code_block_index[ext_func_call_iter] == ci) {
//                        m_ext_funcs[ext_func_call_iter]->forward(&(workspace.m_v.front()));
//                        ++ext_func_call_iter;
//                    }
//                }
                if (thread_syncer) thread_syncer->syncThreads();
                ForwardFunc(ci)(
                    &(workspace.m_v.front()), &(workspace.m_d.front()),
                    &(m_const_data.front()), &(workspace.m_stack.front())
                );
            }
        }
        workspace.m_rev_runs_after_fwd = 0;
    }
    void reverse(
        AADCWorkSpace<mmType>& workspace, int first_cp = 0, int last_cp = -1,
        std::shared_ptr<AADCFunctionSynchronizer> thread_syncer = 0
    ) {
        last_cp = last_cp == -1 ? int(m_num_checkpoints) : last_cp;
        if (m_num_checkpoints > 0) {
//            std::cout << "*******\n";
//            std::cout << "Num checkpoints " << m_num_checkpoints << std::endl;
//            for (int ci = 0; ci < m_checkpoint_indx.size(); ++ci) {
//                std::cout << " check at " <<  m_checkpoint_indx[ci];
//            }
//            std::cout << "\n*******\n";
            if (last_cp == m_num_checkpoints) {
                if (workspace.m_rev_runs_after_fwd > 0) {
    //                std::cout << "restore check point " << m_checkpoint_indx.size() - 1 << std::endl;
                    loadCheckPointVars(m_checkpoint_indx.size() - 1, workspace);
                    for (uint64_t ci = m_checkpoint_indx.back(); ci < getNumCodeBlocks(); ++ci) {
    //                    std::cout << "forward " << ci << std::endl;
                        if (thread_syncer) thread_syncer->syncThreads();
                        ForwardFunc(ci)(
                            &(workspace.m_v.front()), &(workspace.m_d.front()),
                            &(m_const_data.front()), &(workspace.m_stack.front())
                        );
                    }
                }
                for (int ci = int(getNumCodeBlocks())-1; ci >= int(m_checkpoint_indx.back()); --ci) {
    //                std::cout << "reverse " << ci << std::endl;
                    if (thread_syncer) thread_syncer->syncThreads();
                    ReverseFunc(ci)(
                        &(workspace.m_v.front()), &(workspace.m_d.front()),
                        &(m_const_data.front()), &(workspace.m_stack.front())
                    );
                }
                last_cp--;
            }
//            for (int checkp_i = int(m_checkpoint_indx.size())-1; checkp_i > 0; --checkp_i) {
            for (int checkp_i = last_cp; checkp_i >= first_cp; --checkp_i) {
                if (checkp_i > 0) {
    //                std::cout << "restore check point " << checkp_i << std::endl;
                    loadCheckPointVars(checkp_i, workspace);
    //                std::cout << "restore check point " << checkp_i-1 << std::endl;
                    loadCheckPointVars(checkp_i-1, workspace);
                    for (int ci = int(m_checkpoint_indx[checkp_i-1]); ci < int(m_checkpoint_indx[checkp_i]); ++ci) {
    //                    std::cout << "forward " << ci << std::endl;
                        if (thread_syncer) thread_syncer->syncThreads();
                        ForwardFunc(ci)(
                            &(workspace.m_v.front()), &(workspace.m_d.front()),
                            &(m_const_data.front()), &(workspace.m_stack.front())
                        );
                    }
                    for (int ci = int(m_checkpoint_indx[checkp_i])-1; ci >= int(m_checkpoint_indx[checkp_i-1]); --ci) {
    //                    std::cout << "reverse " << ci << std::endl;
                        if (thread_syncer) thread_syncer->syncThreads();
                        ReverseFunc(ci)(
                            &(workspace.m_v.front()), &(workspace.m_d.front()),
                            &(m_const_data.front()), &(workspace.m_stack.front())
                        );
                    }
                }
                else { // very first interval
                    for (int ci = int(m_checkpoint_indx[0])-1; ci >= 0; --ci) {
        //                std::cout << "reverse " << ci << std::endl;
                        loadCheckPointVars(0, workspace);
                        if (thread_syncer) thread_syncer->syncThreads();
                        ReverseFunc(ci)(
                            &(workspace.m_v.front()), &(workspace.m_d.front()),
                            &(m_const_data.front()), &(workspace.m_stack.front())
                        );
                    }
                }
            }
        } else {
//			int ext_func_call_iter(int(m_ext_func_code_block_index.size())-1);

            for (int64_t ci = getNumCodeBlocks()-1; ci >= 0; --ci) {
                if (thread_syncer) thread_syncer->syncThreads();
                ReverseFunc(ci)(
                    &(workspace.m_v.front()), &(workspace.m_d.front()),
                    &(m_const_data.front()), &(workspace.m_stack.front())
                );
//                if (ext_func_call_iter >= 0) {
//                    if (m_ext_func_code_block_index[ext_func_call_iter] == ci) {
//                        m_ext_funcs[ext_func_call_iter]->reverse(
//                            &(workspace.m_v.front()),
//                            &(workspace.m_d.front())
//                        );
//                        --ext_func_call_iter;
//                    }
//                }
            }
        }
        workspace.m_rev_runs_after_fwd++;
    }
    AADCOpCounter OpCodeStart() const {
        return m_op_code_func_start;
    }
    AADCOpCounter OpCodeEnd() const {
        return m_op_code_func_end;
    }
public:
    uint64_t getCodeSizeFwd() const {
        return m_code_size_fwd;
    }
    uint64_t getCodeSizeRev() const {
        return m_code_size_rev;
    }
    uint64_t getWorkArraySize() const {
        return m_work_array_size;
    }
    uint64_t getStackSize() const {
        return m_stack_size;
    }
    uint64_t getConstDataSize() const {
        return m_const_data.size();
    }
    uint64_t getNumCodeBlocks() const {
        return m_num_code_blocks;
    }
    uint64_t getNumCheckPoints() const {
        return m_num_checkpoints;
    }
    uint64_t getNumCheckPointVars() const {
        return m_total_num_checkpoint_vars;
    }
    uint64_t getMemUse() const {
        return getCodeSizeFwd() + getCodeSizeRev() + getConstDataSize() * sizeof(double);
    }
    uint64_t getWorkSpaceMemUse() const {
        return (getWorkArraySize() * 2 + getStackSize() + getNumCheckPointVars()) * sizeof(mmType);
    }
    template<class ostream>
    void outStats(ostream& ostr, const std::string& func_name) {
        std::chrono::microseconds compile_time = std::chrono::duration_cast<std::chrono::microseconds>(m_stop_recording - m_start_recording);

        std::cout << func_name << " Compile time      : " << compile_time.count() << std::endl;
        std::cout << func_name << " Code size forward : " << getCodeSizeFwd() << std::endl;
        std::cout << func_name << " Code size reverse : " << getCodeSizeRev() << std::endl;
        std::cout << func_name << " Work array size   : " << getWorkArraySize() << std::endl;
        std::cout << func_name << " Stack size        : " << getStackSize() << std::endl;
        std::cout << func_name << " Const data size   : " << getConstDataSize() << std::endl;
        std::cout << func_name << " CheckPoint size   : " << getNumCheckPointVars() << std::endl;
        std::cout << func_name << " Func + WS mem use : " << getMemUse()+getWorkSpaceMemUse() << " bytes" << std::endl;
        std::cout << func_name << " Num code blocks   : " << m_func_forward.size() << " " << m_func_reverse.size() << std::endl;

    }
    template<class ostream>
    void printPassiveExtractLocations(ostream& ostr, const std::string& func_name) {
        ostr << "Number active to passive conversions: " << CAAD_iVarNumExtractPassive() 
            << " while recording " << func_name
            << std::endl;
		for (int i = 0; i < CAAD_iVarNumExtractPassive(); ++i) {
			ostr << CAAD_iVarGetExtractPassiveLocation(i) << ":" << CAAD_iVarGetExtractPassiveLocationLine(i) << std::endl;
		}
    }
    uint64_t getNumberOfRequiredRandomVariables() {
        return m_number_of_required_random_variables;
    }
private:
    void initOptions() {
        CAAD_ResetOptions();
        for (auto i = m_options.begin(); i != m_options.end(); ++i) {
            CAAD_SetOption(i->first, i->second);
        }
        CAAD_Init(m_code_options, sizeof(mmType) / sizeof(double));
        getRecordingConstStateExtFunctionRegistry().resize(0);
    }
    virtual BinFunc ForwardFunc(uint64_t code_block) {
        return m_func_forward[code_block];
    }
    virtual BinFunc ReverseFunc(uint64_t code_block) {
        return m_func_reverse[code_block];
    }
    void saveCheckPointVars(uint64_t cp, AADCWorkSpace<mmType>& workspace) {
        // Save checkpoint variables
        for (uint64_t cp_var_i = 0; cp_var_i < m_checkpoint_vars[cp].size(); ++cp_var_i) {
            uint64_t var_indx(m_checkpoint_vars[cp][cp_var_i]);
            workspace.m_check_points[cp][cp_var_i] = workspace.m_v[var_indx];
        }
    }
    void loadCheckPointVars(uint64_t cp, AADCWorkSpace<mmType>& workspace) {
        for (uint64_t cp_var_i = 0; cp_var_i < m_checkpoint_vars[cp].size(); ++cp_var_i) {
            uint64_t var_indx(m_checkpoint_vars[cp][cp_var_i]);
            workspace.m_v[var_indx] = workspace.m_check_points[cp][cp_var_i];
        }
    }

protected:
    AADCOptions m_options;
    int m_code_options;
    std::vector<BinFunc> m_func_forward, m_func_reverse;
    uint64_t m_code_size_fwd, m_code_size_rev;
    int64_t m_work_array_size;
    int64_t m_stack_size;
    uint64_t m_num_code_blocks;
    uint64_t m_total_num_checkpoint_vars;
    std::vector<uint64_t> m_checkpoint_indx;
    std::vector<double> m_const_data; // TODO: if recording multiple functions in one valuation flow, need to share const_data somehow
    JitRuntime m_code;

    uint64_t m_num_checkpoints;
    uint64_t m_number_of_required_random_variables;

    std::vector<std::vector<uint64_t>> m_checkpoint_vars;
    std::vector<uint64_t> m_num_checkpoint_vars;

//    std::vector<std::shared_ptr<ExtFunc> > m_ext_funcs;
//    std::vector<int> m_ext_func_code_block_index;

    std::vector<std::shared_ptr<ConstStateExtFunc> > m_ext_funcs_new;

    AADCOpCounter m_op_code_func_start, m_op_code_func_end;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_recording, m_stop_recording;
};
}; // napespace aadc


#endif
