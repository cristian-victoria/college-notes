// Cristian Victora
/* instruction-level simulator for LC3100 */

#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* Maximum number of words in memory */
#define NUMREGS 8 /* Number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);

int convertNum(int num) {
/* Converts a 16-bit number into a 32-bit Sun integer */
    if (num & (1 << 15)) {
        num -= (1 << 16);
    }
    return (num);
}

void process(stateType *state) {
    int alive = 1; // boolean
    int countSteps = 0; // DEBUG PURPOSES !!!!!

    while(alive) {
        printState(state);
        int instruction = state->mem[state->pc];
        
        // Get the information
        int opcode = (instruction >> 22) & 7; // Opcode resides in bits 22-24
        int regA = (instruction >> 19) & 7; // Register A resides in bits 19-21
        int regB = (instruction >> 16) & 7; // Register B resides in bits 16-18
        int destReg = instruction & 7; // Destination register resides in bits 0-2
        // Immediate needs to go through a sign extend
        int immediate = convertNum(instruction & 65535); // Immediate resides in bits 0-15

        // DEBUG PURPOSES !!!!!!
        printf("INFORMATION\n");
        printf("instruction: %d\n", instruction);
        printf("opcode: %d\n", opcode);
        printf("regA: %d\n", regA);
        printf("regB: %d\n", regB);
        printf("destReg: %d\n", destReg);
        printf("Immediate: %d\n\n", immediate);
        countSteps++;
        if(countSteps > 20) exit(1);
        // !!!!!!!!!!!!!!!!!!!!
        
        // Perform instruction based on opcode
        switch(opcode){
            case 0:
                // Perform ADD
                state->reg[destReg] = state->reg[regA] + state->reg[regB];
                state->pc += 1;
                break;
            case 1:
                // Perform NAND
                state->reg[destReg] = ~(state->reg[regA] & state->reg[regB]);
                state->pc += 1;
                break;
            case 2:
                // Perform LOAD WORD
                state->reg[regB] = state->mem[state->reg[regA] + immediate];
                state->pc += 1;
                break;
            case 3:
                // Perform STORE WORD
                state->mem[state->reg[regA] + immediate] = state->reg[regB]; 
                state->pc += 1;
                break;
            case 4:
                // Perform BRANCH
                if(state->reg[regA] == state->reg[regB]){
                    state->pc += 1 + immediate;
                } else{
                    state->pc += 1;
                }
                break;
            case 6:
                // HALT
                state->pc += 1;
                printf("Machine halted \n");
                printf("Total of %d instructions \n", countSteps);
                // Print the final state of the machine
                printf("Final state of machine: \n");
                printState(state);
                
                alive = 0;
                break;
            case 7:
                // NO OP
                break;
            default:
                // HALT if any other opcode
                alive = 0;
                exit(1);
                break;
        }
    }
}

int main(int argc, char *argv[]){
    char line[MAXLINELENGTH];
    stateType state; // Declare state
    FILE *filePtr; // Declare File

    // Checks for correct number of arguments
    if (argc != 2) {
	    printf("Error: usage: %s <machine-code file>\n", argv[0]);
	    exit(1);
    }

    filePtr = fopen(argv[1], "r"); // Opens the file
    if (filePtr == NULL) {
	    printf("Error: can't open file %s", argv[1]);
	    perror("fopen");
	    exit(1);
    }

    // Read in the entire machine-code file into memory
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
	    if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
	        printf("Error in reading address %d\n", state.numMemory);
	        exit(1);
	    }
	    printf("Memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    // Initialize 8 registers
    int i;
    for(i=0; i<NUMREGS; i++) state.reg[i]=0;

    // Process the instructions
    process(&state);
    
    return(0);
}

void printState(stateType *statePtr){
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("end state\n");
}
