#include "pin.H"
#include <iostream>
#include <fstream>
#include <map>

typedef std::map<string, UINT64> InsnMap;

/* =====================================================================
 * Global Variables
 * ===================================================================== */
static InsnMap instructions;

std::ostream *out = &std::cerr;

/* =====================================================================
 * Instrumentation
 * ===================================================================== */

/* Increment counter. */
VOID docount(VOID *counter) {
    (*static_cast<UINT64 *>(counter))++;
}

/* Called every time a new instruction is encountered 
 * TODO: Update so this is called once per basic block, not once per
 *          instruction */
VOID Instruction(INS ins, VOID *v)
{
    string mnemonic;
    UINT64 *counter;
    InsnMap::iterator lb, ret;
   
    /* Should insert if mnemonic not seen, return existing otherwise */
    mnemonic = INS_Mnemonic(ins);

    lb = instructions.lower_bound(mnemonic);
    if (lb != instructions.end() 
        && !(instructions.key_comp()(mnemonic, lb->first))) {
        //key already exists
        counter = &(lb->second);
    }
    else {
        //key does not exist
        ret = instructions.insert(lb, InsnMap::value_type(mnemonic, 0));
        counter = &(ret->second);
    }

    // Insert a call to count instruction (based on mnemonic)
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount,
                    IARG_PTR, counter, IARG_END);
}

/* Called at end */
VOID Fini(INT32 code, VOID *v)
{
    InsnMap::iterator it;

    *out << "mnemonic,count" << std::endl;
    
    for (it = instructions.begin(); it != instructions.end(); ++it)
        *out << it->first << "," << it->second << std::endl;
}


/* =====================================================================
 * Command line switches & utilities
 * ===================================================================== */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "",
                            "specify file name for DynCount output");

/* Usage */
INT32 Usage(VOID)
{
    std::cerr << "Prints out the number of each instruction" << std::endl
              << "dynamically executed." << std::endl;

    std::cerr << KNOB_BASE::StringKnobSummary() << std::endl;

    return -1;
}

/* =====================================================================
 * main 
 * ===================================================================== */
int main(int argc, char *argv[])
{
    string filename;
    
    /* Returns true if error detected */
    if (PIN_Init(argc, argv)) {
        std::cout << "Error!" << std::endl;
        return 1;
    }

    filename = KnobOutputFile.Value();
    if (!filename.empty()) {
        out = new std::ofstream(filename.c_str());
    }

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    PIN_StartProgram();        

    return 0;
}

