#include "pin.H"
#include <iostream>
#include <fstream>
#include <map>

/* Global Variables */
std::ofstream TraceFile;
std::map<ADDRINT, UINT64> instruction_counts;
std::map<ADDRINT, std::string> addr_to_func;

// Function address ranges (we'll populate these)
ADDRINT naive_prime_start = 0;
ADDRINT naive_prime_end = 0;
ADDRINT less_naive_prime_start = 0; 
ADDRINT less_naive_prime_end = 0;
ADDRINT naive_prime_squares_start = 0;
ADDRINT naive_prime_squares_end = 0;

/* Commandline Switches */
KNOB<std::string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "prime_simple.out", "specify trace file name");

// Check if address is in a target function
std::string get_function_name(ADDRINT addr) {
    if (addr >= naive_prime_start && addr < naive_prime_end) return "naive_prime";
    if (addr >= less_naive_prime_start && addr < less_naive_prime_end) return "less_naive_prime";
    if (addr >= naive_prime_squares_start && addr < naive_prime_squares_end) return "naive_prime_squares";
    return "";
}

// Analysis routine - called for every instruction
VOID count_instruction(ADDRINT ip) {
    std::string func = get_function_name(ip);
    if (!func.empty()) {
        instruction_counts[ip]++;
    }
}

// Instrumentation routine
VOID instruction_instrumentation(INS ins, VOID *v) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_instruction,
                   IARG_INST_PTR, IARG_END);
}

// Image load callback to find function addresses
VOID image_load(IMG img, VOID *v) {
    if (IMG_IsMainExecutable(img)) {
        TraceFile << "Main executable: " << IMG_Name(img) << std::endl;
        
        // Find functions by searching symbols
        for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
            for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn)) {
                std::string name = RTN_Name(rtn);
                ADDRINT start = RTN_Address(rtn);
                ADDRINT end = start + RTN_Size(rtn);
                
                if (name == "naive_prime") {
                    naive_prime_start = start;
                    naive_prime_end = end;
                    TraceFile << "Found naive_prime: 0x" << std::hex << start 
                              << " - 0x" << end << std::dec << std::endl;
                } else if (name == "less_naive_prime") {
                    less_naive_prime_start = start;
                    less_naive_prime_end = end;
                    TraceFile << "Found less_naive_prime: 0x" << std::hex << start 
                              << " - 0x" << end << std::dec << std::endl;
                } else if (name == "naive_prime_squares") {
                    naive_prime_squares_start = start;
                    naive_prime_squares_end = end;
                    TraceFile << "Found naive_prime_squares: 0x" << std::hex << start 
                              << " - 0x" << end << std::dec << std::endl;
                }
            }
        }
        
        // If symbols didn't work, try hardcoded addresses from nm output
        if (naive_prime_start == 0) {
            TraceFile << "Symbols not found via RTN, trying hardcoded addresses from nm..." << std::endl;
            naive_prime_start = 0x4014b8;
            naive_prime_end = 0x401500;  // based on nm showing it's 72 bytes (0x48)
            less_naive_prime_start = 0x40154b;
            less_naive_prime_end = 0x4015fc;  // based on nm showing it's 177 bytes (0xb1)
            naive_prime_squares_start = 0x401500;
            naive_prime_squares_end = 0x40154b;  // based on nm showing it's 75 bytes (0x4b)
            
            TraceFile << "Using hardcoded addresses:" << std::endl;
            TraceFile << "naive_prime: 0x" << std::hex << naive_prime_start << " - 0x" << naive_prime_end << std::dec << std::endl;
            TraceFile << "less_naive_prime: 0x" << std::hex << less_naive_prime_start << " - 0x" << less_naive_prime_end << std::dec << std::endl;
            TraceFile << "naive_prime_squares: 0x" << std::hex << naive_prime_squares_start << " - 0x" << naive_prime_squares_end << std::dec << std::endl;
        }
    }
}

VOID Fini(INT32 code, VOID *v) {
    TraceFile << std::endl << "========== INSTRUCTION COUNTS ==========" << std::endl;
    
    std::map<std::string, UINT64> func_totals;
    
    for (auto& pair : instruction_counts) {
        std::string func = get_function_name(pair.first);
        if (!func.empty()) {
            func_totals[func] += pair.second;
        }
    }
    
    for (auto& pair : func_totals) {
        TraceFile << pair.first << ": " << pair.second << " instructions executed" << std::endl;
    }
    
    TraceFile << std::endl << "========== CSV DATA ==========" << std::endl;
    TraceFile << "function,instruction_count" << std::endl;
    for (auto& pair : func_totals) {
        TraceFile << pair.first << "," << pair.second << std::endl;
    }
    
    TraceFile.close();
}

INT32 Usage() {
    PIN_ERROR("This tool counts instructions in prime functions\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

int main(int argc, char * argv[]) {
    if (PIN_Init(argc, argv)) return Usage();

    TraceFile.open(KnobOutputFile.Value().c_str());
    TraceFile << "Simple Prime Profiler - Instruction Counter" << std::endl;
    TraceFile << "===========================================" << std::endl;

    IMG_AddInstrumentFunction(image_load, 0);
    INS_AddInstrumentFunction(instruction_instrumentation, 0);
    PIN_AddFiniFunction(Fini, 0);
    
    PIN_StartProgram();
    return 0;
}