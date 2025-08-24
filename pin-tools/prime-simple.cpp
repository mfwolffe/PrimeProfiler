// Ultra-minimal Pin tool to avoid GCC compatibility issues
extern "C" {
#include <stdio.h>
#include <stdlib.h>
}

#include "pin.H"

// Simple counters
static UINT64 total_instructions = 0;
static UINT64 function_calls[3] = {0, 0, 0}; // naive, squares, less_naive
static FILE* output_file;

// Analysis functions - keep it super simple
VOID count_instruction() {
    total_instructions++;
}

VOID count_naive() { function_calls[0]++; }
VOID count_squares() { function_calls[1]++; }  
VOID count_less_naive() { function_calls[2]++; }

// Instrumentation
VOID Instruction(INS ins, VOID *v) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_instruction, IARG_END);
}

VOID Routine(RTN rtn, VOID *v) {
    const char* func_name = RTN_Name(rtn).c_str();
    
    RTN_Open(rtn);
    
    if (strcmp(func_name, "naive_prime") == 0) {
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)count_naive, IARG_END);
    }
    else if (strcmp(func_name, "naive_prime_squares") == 0) {
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)count_squares, IARG_END);
    }
    else if (strcmp(func_name, "less_naive_prime") == 0) {
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)count_less_naive, IARG_END);
    }
    
    RTN_Close(rtn);
}

VOID Fini(INT32 code, VOID *v) {
    fprintf(output_file, "=== Prime Algorithm Instrumentation Results ===\n");
    fprintf(output_file, "Total Instructions: %llu\n", total_instructions);
    fprintf(output_file, "naive_prime calls: %llu\n", function_calls[0]);
    fprintf(output_file, "naive_prime_squares calls: %llu\n", function_calls[1]);
    fprintf(output_file, "less_naive_prime calls: %llu\n", function_calls[2]);
    fclose(output_file);
}

INT32 Usage() {
    PIN_ERROR("Simple Pin tool for prime algorithm instrumentation\n");
    return -1;
}

int main(int argc, char * argv[]) {
    if (PIN_Init(argc, argv)) return Usage();
    
    output_file = fopen("prime_results.txt", "w");
    if (!output_file) {
        fprintf(stderr, "Could not open output file\n");
        return -1;
    }
    
    RTN_AddInstrumentFunction(Routine, 0);
    INS_AddInstrumentFunction(Instruction, 0);  
    PIN_AddFiniFunction(Fini, 0);
    
    PIN_StartProgram();
    return 0;
}