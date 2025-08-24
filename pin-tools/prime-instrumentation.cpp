#include "pin.H"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iomanip>

/* ===================================================================== */
/* Names of malloc and free */
/* ===================================================================== */
#if defined(TARGET_MAC)
#define MALLOC "_malloc"
#define FREE "_free"
#else
#define MALLOC "malloc"
#define FREE "free"
#endif

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */
std::ofstream TraceFile;

// Per-function statistics
struct FunctionStats {
    UINT64 call_count;
    UINT64 instruction_count;
    UINT64 memory_reads;
    UINT64 memory_writes;
    UINT64 cycles_start;
    UINT64 cycles_end;
    
    FunctionStats() : call_count(0), instruction_count(0), 
                     memory_reads(0), memory_writes(0), 
                     cycles_start(0), cycles_end(0) {}
};

std::map<std::string, FunctionStats> function_stats;
std::string current_function = "";
bool in_prime_function = false;

// Target functions to instrument
const std::string target_functions[] = {
    "naive_prime",
    "naive_prime_squares", 
    "less_naive_prime"
};

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */
KNOB<std::string> KnobOutputFile(KNOB_MODE_WRITEONCE,  "pintool",
    "o", "prime_profile.out", "specify trace file name");

/* ===================================================================== */
/* Utility Functions */
/* ===================================================================== */
bool is_target_function(const std::string& name) {
    for (const auto& target : target_functions) {
        if (name == target) return true;
    }
    return false;
}

/* ===================================================================== */
/* Analysis routines */
/* ===================================================================== */

// Called when entering a target function
VOID function_entry(const std::string* func_name) {
    current_function = *func_name;
    in_prime_function = true;
    
    if (function_stats.find(current_function) == function_stats.end()) {
        function_stats[current_function] = FunctionStats();
    }
    
    function_stats[current_function].call_count++;
    function_stats[current_function].cycles_start = __rdtsc();
    
    TraceFile << "ENTER: " << current_function 
              << " (call #" << function_stats[current_function].call_count << ")"
              << std::endl;
}

// Called when exiting a target function  
VOID function_exit(const std::string* func_name) {
    if (in_prime_function && current_function == *func_name) {
        function_stats[current_function].cycles_end = __rdtsc();
        
        TraceFile << "EXIT:  " << current_function 
                  << " (cycles: " << (function_stats[current_function].cycles_end - 
                                     function_stats[current_function].cycles_start) << ")"
                  << std::endl;
        
        in_prime_function = false;
        current_function = "";
    }
}

// Count instructions in target functions
VOID count_instruction() {
    if (in_prime_function && !current_function.empty()) {
        function_stats[current_function].instruction_count++;
    }
}

// Count memory reads
VOID count_memory_read() {
    if (in_prime_function && !current_function.empty()) {
        function_stats[current_function].memory_reads++;
    }
}

// Count memory writes  
VOID count_memory_write() {
    if (in_prime_function && !current_function.empty()) {
        function_stats[current_function].memory_writes++;
    }
}

/* ===================================================================== */
/* Instrumentation routines */
/* ===================================================================== */

// Instrument function calls
VOID function_instrumentation(IMG img, VOID *v) {
    // Instrument each routine in the image
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn)) {
            std::string func_name = RTN_Name(rtn);
            
            if (is_target_function(func_name)) {
                TraceFile << "Instrumenting function: " << func_name << std::endl;
                
                RTN_Open(rtn);
                
                // Insert call to function_entry at function entry point
                RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)function_entry,
                              IARG_PTR, new std::string(func_name), IARG_END);
                
                // Insert call to function_exit at all function exit points
                RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)function_exit,
                              IARG_PTR, new std::string(func_name), IARG_END);
                
                RTN_Close(rtn);
            }
        }
    }
}

// Instrument instructions
VOID instruction_instrumentation(INS ins, VOID *v) {
    // Count all instructions
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_instruction, IARG_END);
    
    // Count memory operations
    if (INS_IsMemoryRead(ins)) {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_memory_read, IARG_END);
    }
    
    if (INS_IsMemoryWrite(ins)) {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_memory_write, IARG_END);
    }
}

/* ===================================================================== */

VOID Fini(INT32 code, VOID *v) {
    TraceFile << std::endl << "========== FINAL STATISTICS ==========" << std::endl;
    TraceFile << std::left << std::setw(20) << "Function" 
              << std::setw(12) << "Calls"
              << std::setw(15) << "Instructions" 
              << std::setw(12) << "Mem Reads"
              << std::setw(12) << "Mem Writes"
              << std::setw(15) << "Avg Cycles/Call" << std::endl;
    TraceFile << std::string(86, '-') << std::endl;
    
    for (const auto& pair : function_stats) {
        const std::string& func_name = pair.first;
        const FunctionStats& stats = pair.second;
        
        UINT64 avg_cycles = stats.call_count > 0 ? 
            (stats.cycles_end - stats.cycles_start) / stats.call_count : 0;
            
        TraceFile << std::left << std::setw(20) << func_name
                  << std::setw(12) << stats.call_count
                  << std::setw(15) << stats.instruction_count
                  << std::setw(12) << stats.memory_reads  
                  << std::setw(12) << stats.memory_writes
                  << std::setw(15) << avg_cycles << std::endl;
    }
    
    // Output CSV data for easy analysis
    TraceFile << std::endl << "========== CSV DATA ==========" << std::endl;
    TraceFile << "function,calls,instructions,mem_reads,mem_writes,avg_cycles" << std::endl;
    
    for (const auto& pair : function_stats) {
        const std::string& func_name = pair.first;
        const FunctionStats& stats = pair.second;
        
        UINT64 avg_cycles = stats.call_count > 0 ? 
            (stats.cycles_end - stats.cycles_start) / stats.call_count : 0;
            
        TraceFile << func_name << ","
                  << stats.call_count << ","
                  << stats.instruction_count << ","
                  << stats.memory_reads << ","
                  << stats.memory_writes << ","
                  << avg_cycles << std::endl;
    }

    TraceFile.close();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
   
INT32 Usage() {
    PIN_ERROR( "This tool instruments primality testing functions\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char * argv[]) {
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    TraceFile.open(KnobOutputFile.Value().c_str());
    TraceFile << "Prime Algorithm Performance Profiler" << std::endl;
    TraceFile << "Target functions: naive_prime, naive_prime_squares, less_naive_prime" << std::endl;
    TraceFile << std::string(60, '=') << std::endl;

    // Register function instrumentation callback
    IMG_AddInstrumentFunction(function_instrumentation, 0);
    
    // Register instruction instrumentation callback
    INS_AddInstrumentFunction(instruction_instrumentation, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}