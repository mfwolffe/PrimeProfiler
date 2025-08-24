#include "pin.H"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <x86intrin.h>
#include <cstdlib>

/* Global Variables */
std::ofstream TraceFile;

// Per-function comprehensive statistics
struct FunctionMetrics {
    UINT64 call_count;
    UINT64 instruction_count;
    UINT64 memory_reads;
    UINT64 memory_writes;
    UINT64 branch_count;
    UINT64 cycles_total;
    UINT64 cycles_start;
    bool in_function;
    
    FunctionMetrics() : call_count(0), instruction_count(0), memory_reads(0), 
                       memory_writes(0), branch_count(0), cycles_total(0), 
                       cycles_start(0), in_function(false) {}
};

std::map<std::string, FunctionMetrics> metrics;

// Function address ranges
ADDRINT naive_prime_start = 0, naive_prime_end = 0;
ADDRINT less_naive_prime_start = 0, less_naive_prime_end = 0;
ADDRINT naive_prime_squares_start = 0, naive_prime_squares_end = 0;

/* Commandline Switches */
KNOB<std::string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "dynamic_profile.out", "specify trace file name");

// Get function name from address
std::string get_function_name(ADDRINT addr) {
    if (addr >= naive_prime_start && addr < naive_prime_end) return "naive_prime";
    if (addr >= less_naive_prime_start && addr < less_naive_prime_end) return "less_naive_prime";
    if (addr >= naive_prime_squares_start && addr < naive_prime_squares_end) return "naive_prime_squares";
    return "";
}

// Function entry tracking
VOID function_entry(ADDRINT addr) {
    std::string func = get_function_name(addr);
    if (!func.empty()) {
        metrics[func].call_count++;
        metrics[func].cycles_start = __rdtsc();
        metrics[func].in_function = true;
        TraceFile << "ENTER: " << func << " (call #" << metrics[func].call_count << ")" << std::endl;
    }
}

// Function exit tracking  
VOID function_exit(ADDRINT addr) {
    std::string func = get_function_name(addr);
    if (!func.empty() && metrics[func].in_function) {
        UINT64 cycles = __rdtsc() - metrics[func].cycles_start;
        metrics[func].cycles_total += cycles;
        metrics[func].in_function = false;
        TraceFile << "EXIT: " << func << " (cycles this call: " << cycles << ")" << std::endl;
    }
}

// Instruction counting
VOID count_instruction(ADDRINT addr) {
    std::string func = get_function_name(addr);
    if (!func.empty()) {
        metrics[func].instruction_count++;
    }
}

// Memory read tracking
VOID count_memory_read(ADDRINT addr) {
    std::string func = get_function_name(addr);
    if (!func.empty()) {
        metrics[func].memory_reads++;
    }
}

// Memory write tracking
VOID count_memory_write(ADDRINT addr) {
    std::string func = get_function_name(addr);
    if (!func.empty()) {
        metrics[func].memory_writes++;
    }
}

// Branch tracking
VOID count_branch(ADDRINT addr) {
    std::string func = get_function_name(addr);
    if (!func.empty()) {
        metrics[func].branch_count++;
    }
}

// Instruction instrumentation
VOID instruction_instrumentation(INS ins, VOID *v) {
    ADDRINT addr = INS_Address(ins);
    
    // Insert instruction counter for every instruction
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_instruction,
                   IARG_INST_PTR, IARG_END);
    
    // Track memory operations
    if (INS_IsMemoryRead(ins)) {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_memory_read,
                       IARG_INST_PTR, IARG_END);
    }
    
    if (INS_IsMemoryWrite(ins)) {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_memory_write,
                       IARG_INST_PTR, IARG_END);
    }
    
    // Track branches and calls
    if (INS_IsBranch(ins) || INS_IsCall(ins)) {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_branch,
                       IARG_INST_PTR, IARG_END);
    }
    
    // Track function entries and exits at specific addresses
    if (addr == naive_prime_start || addr == less_naive_prime_start || addr == naive_prime_squares_start) {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)function_entry,
                       IARG_INST_PTR, IARG_END);
    }
    
    // Function exits (ret instructions within our functions)
    if (INS_IsRet(ins)) {
        std::string func = get_function_name(addr);
        if (!func.empty()) {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)function_exit,
                           IARG_INST_PTR, IARG_END);
        }
    }
}

// Get addresses from environment variables (set by the wrapper script)
VOID get_addresses_from_env() {
    const char* naive_addr = getenv("NAIVE_PRIME_ADDR");
    const char* naive_size = getenv("NAIVE_PRIME_SIZE");
    const char* naive_sq_addr = getenv("NAIVE_PRIME_SQUARES_ADDR");
    const char* naive_sq_size = getenv("NAIVE_PRIME_SQUARES_SIZE");
    const char* less_naive_addr = getenv("LESS_NAIVE_PRIME_ADDR");
    const char* less_naive_size = getenv("LESS_NAIVE_PRIME_SIZE");
    
    if (naive_addr && naive_size) {
        naive_prime_start = strtoul(naive_addr, NULL, 16);
        naive_prime_end = naive_prime_start + strtoul(naive_size, NULL, 16);
        TraceFile << "ENV: naive_prime at 0x" << std::hex << naive_prime_start 
                  << " - 0x" << naive_prime_end << " (size: 0x" << (naive_prime_end - naive_prime_start) 
                  << ")" << std::dec << std::endl;
    }
    
    if (naive_sq_addr && naive_sq_size) {
        naive_prime_squares_start = strtoul(naive_sq_addr, NULL, 16);
        naive_prime_squares_end = naive_prime_squares_start + strtoul(naive_sq_size, NULL, 16);
        TraceFile << "ENV: naive_prime_squares at 0x" << std::hex << naive_prime_squares_start 
                  << " - 0x" << naive_prime_squares_end << " (size: 0x" << (naive_prime_squares_end - naive_prime_squares_start) 
                  << ")" << std::dec << std::endl;
    }
    
    if (less_naive_addr && less_naive_size) {
        less_naive_prime_start = strtoul(less_naive_addr, NULL, 16);
        less_naive_prime_end = less_naive_prime_start + strtoul(less_naive_size, NULL, 16);
        TraceFile << "ENV: less_naive_prime at 0x" << std::hex << less_naive_prime_start 
                  << " - 0x" << less_naive_prime_end << " (size: 0x" << (less_naive_prime_end - less_naive_prime_start) 
                  << ")" << std::dec << std::endl;
    }
    
    TraceFile << "Address ranges configured with exact sizes!" << std::endl;
}

// Image load callback
VOID image_load(IMG img, VOID *v) {
    if (IMG_IsMainExecutable(img)) {
        TraceFile << "Main executable: " << IMG_Name(img) << std::endl;
        
        // Get addresses from environment variables
        get_addresses_from_env();
        
        // Initialize metrics for each function
        metrics["naive_prime"] = FunctionMetrics();
        metrics["less_naive_prime"] = FunctionMetrics();
        metrics["naive_prime_squares"] = FunctionMetrics();
        
        TraceFile << "Dynamic address configuration completed!" << std::endl;
    }
}

VOID Fini(INT32 code, VOID *v) {
    TraceFile << std::endl << "========== COMPREHENSIVE PROFILING RESULTS ==========" << std::endl;
    TraceFile << std::left;
    TraceFile.width(20); TraceFile << "Function";
    TraceFile.width(10); TraceFile << "Calls";
    TraceFile.width(15); TraceFile << "Instructions";
    TraceFile.width(12); TraceFile << "Mem Reads";
    TraceFile.width(12); TraceFile << "Mem Writes";
    TraceFile.width(10); TraceFile << "Branches";
    TraceFile.width(15); TraceFile << "Total Cycles";
    TraceFile.width(15); TraceFile << "Avg Cyc/Call";
    TraceFile << std::endl;
    
    TraceFile << std::string(115, '-') << std::endl;
    
    for (auto& pair : metrics) {
        const std::string& name = pair.first;
        const FunctionMetrics& m = pair.second;
        
        UINT64 avg_cycles = (m.call_count > 0) ? (m.cycles_total / m.call_count) : 0;
        
        TraceFile.width(20); TraceFile << name;
        TraceFile.width(10); TraceFile << m.call_count;
        TraceFile.width(15); TraceFile << m.instruction_count;
        TraceFile.width(12); TraceFile << m.memory_reads;
        TraceFile.width(12); TraceFile << m.memory_writes;
        TraceFile.width(10); TraceFile << m.branch_count;
        TraceFile.width(15); TraceFile << m.cycles_total;
        TraceFile.width(15); TraceFile << avg_cycles;
        TraceFile << std::endl;
    }
    
    // CSV output for easy analysis
    TraceFile << std::endl << "========== CSV DATA ==========" << std::endl;
    TraceFile << "function,calls,instructions,mem_reads,mem_writes,branches,total_cycles,avg_cycles_per_call" << std::endl;
    
    for (auto& pair : metrics) {
        const std::string& name = pair.first;
        const FunctionMetrics& m = pair.second;
        UINT64 avg_cycles = (m.call_count > 0) ? (m.cycles_total / m.call_count) : 0;
        
        TraceFile << name << "," << m.call_count << "," << m.instruction_count << ","
                  << m.memory_reads << "," << m.memory_writes << "," << m.branch_count << ","
                  << m.cycles_total << "," << avg_cycles << std::endl;
    }

    TraceFile.close();
}

INT32 Usage() {
    PIN_ERROR("Dynamic prime function profiler\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

int main(int argc, char * argv[]) {
    if (PIN_Init(argc, argv)) return Usage();

    TraceFile.open(KnobOutputFile.Value().c_str());
    TraceFile << "Dynamic Prime Algorithm Profiler" << std::endl;
    TraceFile << "=================================" << std::endl;

    IMG_AddInstrumentFunction(image_load, 0);
    INS_AddInstrumentFunction(instruction_instrumentation, 0);
    PIN_AddFiniFunction(Fini, 0);
    
    PIN_StartProgram();
    return 0;
}