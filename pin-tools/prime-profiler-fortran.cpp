#include "pin.H"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <cstdlib>
#include <x86intrin.h>

using std::cerr;
using std::endl;
using std::string;
using std::map;

// Output file
std::ofstream outfile;

// Function statistics
struct FunctionStats {
    string name;
    string display_name;  // Name to display (without underscores for Fortran)
    UINT64 calls;
    UINT64 instructions;
    UINT64 memory_reads;
    UINT64 memory_writes;
    UINT64 branches;
    UINT64 total_cycles;
    UINT64 entry_cycles;
    bool in_function;
    
    FunctionStats() : calls(0), instructions(0), memory_reads(0), memory_writes(0), 
                     branches(0), total_cycles(0), entry_cycles(0), in_function(false) {}
};

map<string, FunctionStats> function_stats;

// Function address ranges (from environment variables)
ADDRINT naive_prime_start = 0, naive_prime_end = 0;
ADDRINT naive_prime_squares_start = 0, naive_prime_squares_end = 0;
ADDRINT less_naive_prime_start = 0, less_naive_prime_end = 0;

// Get language from environment (C or Fortran)
string language = "C";

VOID get_addresses_from_env() {
    const char* naive_addr = getenv("NAIVE_PRIME_ADDR");
    const char* naive_size = getenv("NAIVE_PRIME_SIZE");
    const char* naive_sq_addr = getenv("NAIVE_PRIME_SQUARES_ADDR");
    const char* naive_sq_size = getenv("NAIVE_PRIME_SQUARES_SIZE");
    const char* less_naive_addr = getenv("LESS_NAIVE_PRIME_ADDR");
    const char* less_naive_size = getenv("LESS_NAIVE_PRIME_SIZE");
    const char* lang = getenv("LANGUAGE");
    
    if (lang) {
        language = string(lang);
    }
    
    if (naive_addr && naive_size) {
        naive_prime_start = strtoul(naive_addr, NULL, 16);
        naive_prime_end = naive_prime_start + strtoul(naive_size, NULL, 16);
        
        // Set up function stats
        if (language == "Fortran") {
            function_stats["naive_prime_"] = FunctionStats();
            function_stats["naive_prime_"].name = "naive_prime_";
            function_stats["naive_prime_"].display_name = "naive_prime";
        } else {
            function_stats["naive_prime"] = FunctionStats();
            function_stats["naive_prime"].name = "naive_prime";
            function_stats["naive_prime"].display_name = "naive_prime";
        }
    }
    
    if (naive_sq_addr && naive_sq_size) {
        naive_prime_squares_start = strtoul(naive_sq_addr, NULL, 16);
        naive_prime_squares_end = naive_prime_squares_start + strtoul(naive_sq_size, NULL, 16);
        
        if (language == "Fortran") {
            function_stats["naive_prime_squares_"] = FunctionStats();
            function_stats["naive_prime_squares_"].name = "naive_prime_squares_";
            function_stats["naive_prime_squares_"].display_name = "naive_prime_squares";
        } else {
            function_stats["naive_prime_squares"] = FunctionStats();
            function_stats["naive_prime_squares"].name = "naive_prime_squares";
            function_stats["naive_prime_squares"].display_name = "naive_prime_squares";
        }
    }
    
    if (less_naive_addr && less_naive_size) {
        less_naive_prime_start = strtoul(less_naive_addr, NULL, 16);
        less_naive_prime_end = less_naive_prime_start + strtoul(less_naive_size, NULL, 16);
        
        if (language == "Fortran") {
            function_stats["less_naive_prime_"] = FunctionStats();
            function_stats["less_naive_prime_"].name = "less_naive_prime_";
            function_stats["less_naive_prime_"].display_name = "less_naive_prime";
        } else {
            function_stats["less_naive_prime"] = FunctionStats();
            function_stats["less_naive_prime"].name = "less_naive_prime";
            function_stats["less_naive_prime"].display_name = "less_naive_prime";
        }
    }
}

string get_function_name(ADDRINT addr) {
    if (addr >= naive_prime_start && addr < naive_prime_end) {
        return (language == "Fortran") ? "naive_prime_" : "naive_prime";
    }
    if (addr >= naive_prime_squares_start && addr < naive_prime_squares_end) {
        return (language == "Fortran") ? "naive_prime_squares_" : "naive_prime_squares";
    }
    if (addr >= less_naive_prime_start && addr < less_naive_prime_end) {
        return (language == "Fortran") ? "less_naive_prime_" : "less_naive_prime";
    }
    return "";
}

VOID on_function_entry(ADDRINT addr) {
    string func_name = get_function_name(addr);
    if (!func_name.empty() && function_stats.find(func_name) != function_stats.end()) {
        FunctionStats& stats = function_stats[func_name];
        stats.calls++;
        stats.entry_cycles = __rdtsc();
        stats.in_function = true;
        
        outfile << "ENTER: " << stats.display_name << " (call #" << stats.calls << ")" << endl;
    }
}

VOID on_function_exit(ADDRINT addr) {
    string func_name = get_function_name(addr);
    if (!func_name.empty() && function_stats.find(func_name) != function_stats.end()) {
        FunctionStats& stats = function_stats[func_name];
        if (stats.in_function) {
            UINT64 exit_cycles = __rdtsc();
            UINT64 call_cycles = exit_cycles - stats.entry_cycles;
            stats.total_cycles += call_cycles;
            stats.in_function = false;
            
            outfile << "EXIT: " << stats.display_name << " (cycles this call: " << call_cycles << ")" << endl;
        }
    }
}

VOID count_instruction(ADDRINT addr) {
    string func_name = get_function_name(addr);
    if (!func_name.empty() && function_stats.find(func_name) != function_stats.end()) {
        function_stats[func_name].instructions++;
    }
}

VOID count_memory_read(ADDRINT addr) {
    string func_name = get_function_name(addr);
    if (!func_name.empty() && function_stats.find(func_name) != function_stats.end()) {
        function_stats[func_name].memory_reads++;
    }
}

VOID count_memory_write(ADDRINT addr) {
    string func_name = get_function_name(addr);
    if (!func_name.empty() && function_stats.find(func_name) != function_stats.end()) {
        function_stats[func_name].memory_writes++;
    }
}

VOID count_branch(ADDRINT addr) {
    string func_name = get_function_name(addr);
    if (!func_name.empty() && function_stats.find(func_name) != function_stats.end()) {
        function_stats[func_name].branches++;
    }
}

VOID instruction_callback(INS ins, VOID *v) {
    ADDRINT addr = INS_Address(ins);
    string func_name = get_function_name(addr);
    
    if (!func_name.empty()) {
        // Count instructions
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_instruction,
                      IARG_INST_PTR, IARG_END);
        
        // Count memory operations
        if (INS_IsMemoryRead(ins)) {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_memory_read,
                          IARG_INST_PTR, IARG_END);
        }
        if (INS_IsMemoryWrite(ins)) {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_memory_write,
                          IARG_INST_PTR, IARG_END);
        }
        
        // Count branches
        if (INS_IsBranch(ins)) {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_branch,
                          IARG_INST_PTR, IARG_END);
        }
        
        // Function entry (first instruction of function)
        if ((addr == naive_prime_start) || (addr == naive_prime_squares_start) || (addr == less_naive_prime_start)) {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)on_function_entry,
                          IARG_INST_PTR, IARG_END);
        }
        
        // Function exit (return instructions)
        if (INS_IsRet(ins)) {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)on_function_exit,
                          IARG_INST_PTR, IARG_END);
        }
    }
}

KNOB<string> knob_output_file(KNOB_MODE_WRITEONCE, "pintool", "o", "prime_profile.out", 
                             "specify output file name");

VOID fini_callback(INT32 code, VOID *v) {
    outfile << endl;
    outfile << "========== COMPREHENSIVE PROFILING RESULTS ==========" << endl;
    outfile << "Language: " << language << endl;
    outfile << "Function            Calls     Instructions   Mem Reads   Mem Writes  Branches  Total Cycles   Avg Cyc/Call   " << endl;
    outfile << "-------------------------------------------------------------------------------------------------------------------" << endl;
    
    for (auto& pair : function_stats) {
        FunctionStats& stats = pair.second;
        UINT64 avg_cycles = (stats.calls > 0) ? (stats.total_cycles / stats.calls) : 0;
        
        outfile << std::left << std::setw(20) << stats.display_name
               << std::setw(10) << stats.calls
               << std::setw(15) << stats.instructions
               << std::setw(12) << stats.memory_reads
               << std::setw(12) << stats.memory_writes
               << std::setw(10) << stats.branches
               << std::setw(15) << stats.total_cycles
               << std::setw(15) << avg_cycles
               << endl;
    }
    
    outfile << endl;
    outfile << "========== CSV DATA ==========" << endl;
    outfile << "function,calls,instructions,mem_reads,mem_writes,branches,total_cycles,avg_cycles_per_call" << endl;
    
    for (auto& pair : function_stats) {
        FunctionStats& stats = pair.second;
        UINT64 avg_cycles = (stats.calls > 0) ? (stats.total_cycles / stats.calls) : 0;
        
        outfile << stats.display_name << "," << stats.calls << "," << stats.instructions << ","
               << stats.memory_reads << "," << stats.memory_writes << "," << stats.branches << ","
               << stats.total_cycles << "," << avg_cycles << endl;
    }
    
    outfile.close();
}

int main(int argc, char *argv[]) {
    PIN_InitSymbols();
    
    if (PIN_Init(argc, argv)) return -1;
    
    string output_filename = knob_output_file.Value();
    outfile.open(output_filename.c_str());
    
    get_addresses_from_env();
    
    outfile << "Dynamic Prime Algorithm Profiler (" << language << ")" << endl;
    outfile << "=================================" << endl;
    outfile << "Main executable: [" << language << " binary]" << endl;
    
    // Report detected addresses
    if (language == "Fortran") {
        if (naive_prime_start != 0) {
            outfile << "ENV: naive_prime_ at 0x" << std::hex << naive_prime_start 
                   << " - 0x" << (naive_prime_end-1) << " (size: 0x" << (naive_prime_end-naive_prime_start) << ")" << std::dec << endl;
        }
        if (naive_prime_squares_start != 0) {
            outfile << "ENV: naive_prime_squares_ at 0x" << std::hex << naive_prime_squares_start 
                   << " - 0x" << (naive_prime_squares_end-1) << " (size: 0x" << (naive_prime_squares_end-naive_prime_squares_start) << ")" << std::dec << endl;
        }
        if (less_naive_prime_start != 0) {
            outfile << "ENV: less_naive_prime_ at 0x" << std::hex << less_naive_prime_start 
                   << " - 0x" << (less_naive_prime_end-1) << " (size: 0x" << (less_naive_prime_end-less_naive_prime_start) << ")" << std::dec << endl;
        }
    } else {
        if (naive_prime_start != 0) {
            outfile << "ENV: naive_prime at 0x" << std::hex << naive_prime_start 
                   << " - 0x" << (naive_prime_end-1) << " (size: 0x" << (naive_prime_end-naive_prime_start) << ")" << std::dec << endl;
        }
        if (naive_prime_squares_start != 0) {
            outfile << "ENV: naive_prime_squares at 0x" << std::hex << naive_prime_squares_start 
                   << " - 0x" << (naive_prime_squares_end-1) << " (size: 0x" << (naive_prime_squares_end-naive_prime_squares_start) << ")" << std::dec << endl;
        }
        if (less_naive_prime_start != 0) {
            outfile << "ENV: less_naive_prime at 0x" << std::hex << less_naive_prime_start 
                   << " - 0x" << (less_naive_prime_end-1) << " (size: 0x" << (less_naive_prime_end-less_naive_prime_start) << ")" << std::dec << endl;
        }
    }
    
    outfile << "Address ranges configured with exact sizes!" << endl;
    outfile << "Dynamic address configuration completed!" << endl;
    
    INS_AddInstrumentFunction(instruction_callback, 0);
    PIN_AddFiniFunction(fini_callback, 0);
    
    PIN_StartProgram();
    
    return 0;
}