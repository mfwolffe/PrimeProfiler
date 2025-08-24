#include "pin.H"
#include <fstream>
#include <iostream>
#include <map>
#include <string>

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */
std::ofstream TraceFile;
std::map<std::string, UINT64> function_calls;
std::map<std::string, UINT64> instruction_counts;

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */
KNOB<std::string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "prime_profile.out", "specify trace file name");

/* ===================================================================== */
/* Analysis routines */
/* ===================================================================== */
VOID record_function_call(const CHAR* name) {
    std::string func_name(name);
    function_calls[func_name]++;
    TraceFile << "Function call: " << func_name << " (count: " << function_calls[func_name] << ")" << std::endl;
}

VOID record_instruction() {
    // This will be called for every instruction - keep it minimal
    // We'll track instructions per function in a more sophisticated way later
}

/* ===================================================================== */
/* Instrumentation routines */
/* ===================================================================== */
VOID Instruction(INS ins, VOID *v) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)record_instruction, IARG_END);
}

VOID Routine(RTN rtn, VOID *v) {
    std::string func_name = RTN_Name(rtn);
    
    // Only instrument our target functions
    if (func_name == "naive_prime" || 
        func_name == "naive_prime_squares" || 
        func_name == "less_naive_prime") {
        
        TraceFile << "Instrumenting function: " << func_name << std::endl;
        
        RTN_Open(rtn);
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)record_function_call,
                      IARG_PTR, func_name.c_str(), IARG_END);
        RTN_Close(rtn);
    }
}

/* ===================================================================== */
VOID Fini(INT32 code, VOID *v) {
    TraceFile << std::endl << "========== FINAL STATISTICS ==========" << std::endl;
    
    for (const auto& pair : function_calls) {
        TraceFile << "Function: " << pair.first 
                  << " - Calls: " << pair.second << std::endl;
    }
    
    TraceFile.close();
}

/* ===================================================================== */
INT32 Usage() {
    PIN_ERROR("This tool instruments primality testing functions\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
int main(int argc, char * argv[]) {
    if (PIN_Init(argc, argv)) return Usage();

    TraceFile.open(KnobOutputFile.Value().c_str());
    TraceFile << "Prime Algorithm Performance Profiler (Minimal Version)" << std::endl;
    TraceFile << "========================================" << std::endl;

    RTN_AddInstrumentFunction(Routine, 0);
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);
    
    PIN_StartProgram();
    
    return 0;
}