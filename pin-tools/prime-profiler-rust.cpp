#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iomanip>
#include <x86intrin.h>
#include "pin.H"

using std::string;
using std::map;
using std::cout;
using std::endl;
using std::ofstream;
using std::setw;

// Structure to hold function statistics
struct FunctionStats {
    string name;
    string display_name;
    ADDRINT start_addr;
    ADDRINT end_addr;
    UINT64 instruction_count;
    UINT64 call_count;
    UINT64 mem_read_count;
    UINT64 mem_write_count;
    UINT64 branch_count;
    UINT64 total_cycles;
    UINT64 cycle_start;
    bool in_function;
    
    FunctionStats() : instruction_count(0), call_count(0), mem_read_count(0), 
                     mem_write_count(0), branch_count(0), total_cycles(0), 
                     cycle_start(0), in_function(false) {}
};

// Global map to store function statistics
static map<string, FunctionStats> function_stats;
static string language = "Rust";

// Get environment variable for language detection
string get_language() {
    const char* lang = getenv("LANGUAGE");
    return lang ? string(lang) : "Rust";
}

// Analysis routines
VOID FunctionEntry(string* name) {
    if (function_stats.find(*name) != function_stats.end()) {
        FunctionStats& stats = function_stats[*name];
        stats.call_count++;
        stats.in_function = true;
        stats.cycle_start = __rdtsc();
    }
}

VOID FunctionExit(string* name) {
    if (function_stats.find(*name) != function_stats.end()) {
        FunctionStats& stats = function_stats[*name];
        if (stats.in_function) {
            UINT64 cycle_end = __rdtsc();
            stats.total_cycles += (cycle_end - stats.cycle_start);
            stats.in_function = false;
        }
    }
}

VOID InstructionCount(string* name) {
    if (function_stats.find(*name) != function_stats.end()) {
        FunctionStats& stats = function_stats[*name];
        if (stats.in_function) {
            stats.instruction_count++;
        }
    }
}

VOID MemoryRead(string* name) {
    if (function_stats.find(*name) != function_stats.end()) {
        FunctionStats& stats = function_stats[*name];
        if (stats.in_function) {
            stats.mem_read_count++;
        }
    }
}

VOID MemoryWrite(string* name) {
    if (function_stats.find(*name) != function_stats.end()) {
        FunctionStats& stats = function_stats[*name];
        if (stats.in_function) {
            stats.mem_write_count++;
        }
    }
}

VOID BranchTaken(string* name) {
    if (function_stats.find(*name) != function_stats.end()) {
        FunctionStats& stats = function_stats[*name];
        if (stats.in_function) {
            stats.branch_count++;
        }
    }
}

// Check if address is in any of our target functions
string* GetFunctionForAddress(ADDRINT addr) {
    for (auto& pair : function_stats) {
        FunctionStats& stats = pair.second;
        if (addr >= stats.start_addr && addr < stats.end_addr) {
            return new string(pair.first);
        }
    }
    return nullptr;
}

// Instrumentation routine
VOID Instruction(INS ins, VOID *v) {
    ADDRINT addr = INS_Address(ins);
    string* func_name = GetFunctionForAddress(addr);
    
    if (func_name) {
        // Count instruction
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)InstructionCount, 
                      IARG_PTR, func_name, IARG_END);
        
        // Count memory operations
        if (INS_IsMemoryRead(ins)) {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)MemoryRead,
                          IARG_PTR, func_name, IARG_END);
        }
        if (INS_IsMemoryWrite(ins)) {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)MemoryWrite,
                          IARG_PTR, func_name, IARG_END);
        }
        
        // Count branches
        if (INS_IsBranch(ins)) {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)BranchTaken,
                          IARG_PTR, func_name, IARG_END);
        }
    }
}

// Routine to instrument function calls
VOID Routine(RTN rtn, VOID *v) {
    string rtn_name = RTN_Name(rtn);
    
    // Debug: Print all function names containing "prime"  
    if (rtn_name.find("prime") != string::npos) {
        cout << "DEBUG: Found function: " << rtn_name << endl;
    }
    
    // Check for Rust wrapper functions
    if (rtn_name == "rust_naive_prime_wrapper" ||
        rtn_name == "rust_naive_prime_squares_wrapper" ||
        rtn_name == "rust_less_naive_prime_wrapper") {
        
        RTN_Open(rtn);
        
        string* func_name = new string(rtn_name);
        
        // Insert entry and exit instrumentation
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)FunctionEntry,
                      IARG_PTR, func_name, IARG_END);
        RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)FunctionExit,
                      IARG_PTR, func_name, IARG_END);
        
        RTN_Close(rtn);
    }
}

// Initialize function statistics from environment variables
VOID InitializeFunctionStats() {
    language = get_language();
    
    // Get function addresses from environment
    const char* naive_addr = getenv("NAIVE_PRIME_ADDR");
    const char* naive_size = getenv("NAIVE_PRIME_SIZE");
    const char* squares_addr = getenv("NAIVE_PRIME_SQUARES_ADDR");
    const char* squares_size = getenv("NAIVE_PRIME_SQUARES_SIZE");
    const char* less_addr = getenv("LESS_NAIVE_PRIME_ADDR");
    const char* less_size = getenv("LESS_NAIVE_PRIME_SIZE");
    
    if (naive_addr && naive_size) {
        ADDRINT addr = strtoul(naive_addr, NULL, 16);
        ADDRINT size = strtoul(naive_size, NULL, 16);
        FunctionStats stats;
        stats.name = "naive_prime_c";
        stats.display_name = "naive_prime";
        stats.start_addr = addr;
        stats.end_addr = addr + size;
        function_stats[stats.name] = stats;
        cout << "ENV: " << stats.display_name << " at 0x" << std::hex << addr 
             << " - 0x" << addr + size << " (size: 0x" << size << ")" << std::dec << endl;
    }
    
    if (squares_addr && squares_size) {
        ADDRINT addr = strtoul(squares_addr, NULL, 16);
        ADDRINT size = strtoul(squares_size, NULL, 16);
        FunctionStats stats;
        stats.name = "naive_prime_squares_c";
        stats.display_name = "naive_prime_squares";
        stats.start_addr = addr;
        stats.end_addr = addr + size;
        function_stats[stats.name] = stats;
        cout << "ENV: " << stats.display_name << " at 0x" << std::hex << addr 
             << " - 0x" << addr + size << " (size: 0x" << size << ")" << std::dec << endl;
    }
    
    if (less_addr && less_size) {
        ADDRINT addr = strtoul(less_addr, NULL, 16);
        ADDRINT size = strtoul(less_size, NULL, 16);
        FunctionStats stats;
        stats.name = "less_naive_prime_c";
        stats.display_name = "less_naive_prime";
        stats.start_addr = addr;
        stats.end_addr = addr + size;
        function_stats[stats.name] = stats;
        cout << "ENV: " << stats.display_name << " at 0x" << std::hex << addr 
             << " - 0x" << addr + size << " (size: 0x" << size << ")" << std::dec << endl;
    }
}

// Print results and generate CSV
VOID Fini(INT32 code, VOID *v) {
    cout << "\n" << string(30, '=') << " Results " << string(30, '=') << endl;
    cout << "Main executable: [" << language << " binary]" << endl;
    cout << "\nResults summary:" << endl;
    cout << string(15, '=') << endl;
    cout << string(115, '-') << endl;
    
    // Print header
    cout << std::left << setw(20) << "Function" 
         << setw(10) << "Calls" 
         << setw(15) << "Instructions" 
         << setw(12) << "Mem Reads" 
         << setw(12) << "Mem Writes" 
         << setw(10) << "Branches" 
         << setw(15) << "Total Cycles" 
         << setw(15) << "Avg Cycles" << endl;
    
    // Open CSV file for output
    ofstream csv_file("prime_profiler_results.csv");
    csv_file << "function,calls,instructions,mem_reads,mem_writes,branches,total_cycles,avg_cycles_per_call\n";
    
    for (auto& pair : function_stats) {
        FunctionStats& stats = pair.second;
        if (stats.call_count > 0) {
            UINT64 avg_cycles = stats.total_cycles / stats.call_count;
            
            cout << std::left << setw(20) << stats.display_name
                 << setw(10) << stats.call_count 
                 << setw(15) << stats.instruction_count 
                 << setw(12) << stats.mem_read_count 
                 << setw(12) << stats.mem_write_count 
                 << setw(10) << stats.branch_count 
                 << setw(15) << stats.total_cycles 
                 << setw(15) << avg_cycles << endl;
                 
            // Write to CSV
            csv_file << stats.display_name << "," 
                    << stats.call_count << ","
                    << stats.instruction_count << ","
                    << stats.mem_read_count << ","
                    << stats.mem_write_count << ","
                    << stats.branch_count << ","
                    << stats.total_cycles << ","
                    << avg_cycles << "\n";
        }
    }
    
    csv_file.close();
    
    cout << "\n" << string(10, '=') << " CSV DATA " << string(10, '=') << endl;
    cout << "function,calls,instructions,mem_reads,mem_writes,branches,total_cycles,avg_cycles_per_call" << endl;
    for (auto& pair : function_stats) {
        FunctionStats& stats = pair.second;
        if (stats.call_count > 0) {
            UINT64 avg_cycles = stats.total_cycles / stats.call_count;
            cout << stats.display_name << "," 
                 << stats.call_count << ","
                 << stats.instruction_count << ","
                 << stats.mem_read_count << ","
                 << stats.mem_write_count << ","
                 << stats.branch_count << ","
                 << stats.total_cycles << ","
                 << avg_cycles << endl;
        }
    }
}

int main(int argc, char * argv[]) {
    if (PIN_Init(argc, argv)) return -1;
    
    InitializeFunctionStats();
    
    RTN_AddInstrumentFunction(Routine, 0);
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);
    
    PIN_StartProgram();
    return 0;
}