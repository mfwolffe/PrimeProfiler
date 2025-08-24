#include "pin.H"

// Global counters - keep it absolutely minimal
static UINT64 total_instructions = 0;
static UINT64 naive_calls = 0;
static UINT64 squares_calls = 0;
static UINT64 less_naive_calls = 0;

// Analysis functions
VOID count_instruction() {
    total_instructions++;
}

VOID count_naive() { naive_calls++; }
VOID count_squares() { squares_calls++; }
VOID count_less_naive() { less_naive_calls++; }

// Instrumentation
VOID Instruction(INS ins, VOID *v) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_instruction, IARG_END);
}

VOID Routine(RTN rtn, VOID *v) {
    RTN_Open(rtn);
    
    if (RTN_Name(rtn) == "naive_prime") {
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)count_naive, IARG_END);
    }
    else if (RTN_Name(rtn) == "naive_prime_squares") {
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)count_squares, IARG_END);
    }
    else if (RTN_Name(rtn) == "less_naive_prime") {
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)count_less_naive, IARG_END);
    }
    
    RTN_Close(rtn);
}

VOID Fini(INT32 code, VOID *v) {
    // Use PIN_ERROR as a simple output method to avoid stdio issues
    PIN_ERROR("=== INSTRUMENTATION RESULTS ===\n" +
              "Total Instructions: " + decstr(total_instructions) + "\n" +
              "naive_prime calls: " + decstr(naive_calls) + "\n" +
              "naive_prime_squares calls: " + decstr(squares_calls) + "\n" +
              "less_naive_prime calls: " + decstr(less_naive_calls) + "\n");
}

INT32 Usage() {
    PIN_ERROR("Ultra-minimal Pin tool for prime algorithm instrumentation\n");
    return -1;
}

int main(int argc, char * argv[]) {
    if (PIN_Init(argc, argv)) return Usage();
    
    RTN_AddInstrumentFunction(Routine, 0);
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);
    
    PIN_StartProgram();
    return 0;
}