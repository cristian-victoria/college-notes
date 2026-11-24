// Cristian Victora
/* cache simulator for LC3100 */

using namespace std;

#include <iostream>
#include <vector>
#include <climits>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Initialize Memory
#define NUMMEMORY 65536 /* Maximum number of words in memory */
#define NUMREGS 8 /* Number of machine registers */
#define MAXLINELENGTH 1000

// The following refers to the Memory
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

// The following refers to the Cache
enum actionType{
    cacheToProcessor,
    processorToCache, 
    memoryToCache,
    cacheToMemory,
    cacheToNowhere
};

struct cacheBlock{
    bool isValidState;
    int dirtyBit;
    int tag; 
    int data;
    int lruCounter;
};

// THE FOLLOWING IS RELATED TO CACHE
class Cache 
{
public:
    // Initialize cache with invalid block state
    Cache(int blockSizeInWords, int numberOfSets, int blocksPerSet, stateType *s) : 
    blockSizeInWords( blockSizeInWords ), numberOfSets( numberOfSets ), blocksPerSet( blocksPerSet ){
        cache.resize( numberOfSets, vector<cacheBlock> ( blocksPerSet, {false, 0, 0, 0, 0} ) );
        int totalNumWords = blockSizeInWords * numberOfSets * blocksPerSet;
        stateType state = *s;
    }

    // Function for processor simulation to access addresses in cache
    int accessCache(int address, bool isWrite){
        int setIndex = (address / blockSizeInWords) % numberOfSets;
        int tag = address / (blockSizeInWords * numberOfSets);

        bool cacheHit = false;
        int hitIndex = -1;

        int accessData = 0;
        int accessAddress = address;

        // Search for valid cache state and recognize hit if found
        for(int i = 0; i < blocksPerSet; ++i){
            if( cache[setIndex][i].isValidState && cache[setIndex][i].tag == tag ){
                cacheHit = true;
                hitIndex = i;
                break;
            }
        }
    
        if(cacheHit){
            // Update lruCounter for all of the blocks but the one it hit
            for( int i = 0; i < blocksPerSet; i++ ){
                if(i != hitIndex){
                    cache[setIndex][i].lruCounter++;
                }
            }
            if(isWrite){
                
            } else{
                
            }
        } else {
            // If its a cache miss, go to memory
            accessAddress = accessMem(address, isWrite);
            printAction(address, blockSizeInWords, memoryToCache);

            int invalidIndex = 0;
            int lruIndex = 0;
            int lruCounterValue = cache[setIndex][0].lruCounter;

            // Find an invalid (empty) block to store the new data
            for(int i = 1; i < blocksPerSet; ++i){
                if(!cache[setIndex][i].isValidState){
                    invalidIndex = i;
                }
            }
            if(invalidIndex){
                cache[setIndex][invalidIndex].isValidState = true;
                cache[setIndex][invalidIndex].dirtyBit = 0;
                cache[setIndex][invalidIndex].tag = tag;
                //cache[setIndex][invalidIndex].data = accessData;
                cache[setIndex][invalidIndex].lruCounter = 0;
            } else{
                // Find the least recently used block which has the lowest lruCounter
                for(int i = 1; i < blocksPerSet; ++i){
                    if(cache[setIndex][i].lruCounter < lruCounterValue){
                        lruCounterValue = cache[setIndex][i].lruCounter;
                        lruIndex = i;
                    }
                }
                // Once the LRU block is found, update this block to store the new data
                cache[setIndex][lruIndex].isValidState = true;
                cache[setIndex][lruIndex].dirtyBit = 0;
                cache[setIndex][lruIndex].tag = tag;
               //cache[setIndex][lruIndex].data = accessData;
                cache[setIndex][lruIndex].lruCounter = 0;
            }

            // Update lruCounter for all of the blocks
            for(int i = 0; i < blocksPerSet; ++i){
                if(i != lruIndex){
                    cache[setIndex][i].lruCounter++;
                }
            }
        }

        return accessAddress;
    }

    int accessMem(int address, bool isWrite){
        return -1;
    }

    void printAction(int address, int size, enum actionType type){
        cout << "@@ transferring word [" << address << "-" << (address + size - 1) << "] ";

        switch (type){
            case cacheToProcessor:
                // Reading data from cache to the processor
                cout << "from the cache to the processor\n";
                break;
            case processorToCache:
                // Writing data from the processor to the cache
                cout << "from the processor to the cache\n";
                break;
            case memoryToCache:
                // Reading data from the memory to the cache
                cout << "from the memory to the cache\n";
                break;
            case cacheToMemory:
                // Evicting cache data by writing it to the memory
                cout << "from the cache to the memory\n";
                break;
            case cacheToNowhere:
                // Evicting cache data by throwing it away
                cout << "from the cache to nowhere\n";
                break;
            default:
                break;
        
        }
    }   

private:
    int blockSizeInWords;
    int numberOfSets;
    int blocksPerSet;
    int totalNumWords;
    vector< vector<cacheBlock> > cache;
    stateType *state;
};

// The following relates to the processor
int convertNum(int num) {
// Converts a 16-bit number into a 32-bit Sun integer
    if (num & (1 << 15)) {
        num -= (1 << 16);
    }
    return (num);
}

void process(stateType *state, Cache *cache) {
    int alive = 1;
    int countSteps = 0;

    while(alive) {        
        int instruction = cache->accessCache(state->pc, false);

        // Get the information
        int opcode = (instruction >> 22) & 7; // Opcode resides in bits 22-24
        int regA = (instruction >> 19) & 7; // Register A resides in bits 19-21
        int regB = (instruction >> 16) & 7; // Register B resides in bits 16-18
        int destReg = instruction & 7; // Destination register resides in bits 0-2
        // Immediate needs to go through a sign extend
        int immediate = convertNum(instruction & 65535); // Immediate resides in bits 0-15

        countSteps++;
        

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
                if(immediate >= 0 && immediate < NUMMEMORY){
                    int memoryAddress = state->reg[regA] + immediate;
                    if(memoryAddress >= 0 && memoryAddress < NUMMEMORY){
                        state->reg[regB] = cache->accessCache(memoryAddress, false);
                        state->pc += 1;
                    } else{
                        cout << "Error: Invalid memory address access.\n";
                        exit(1);
                    }
                } else{
                    cout << "Error: Invalid memory offset.\n";
                    exit(1);
                }
                break;
            case 3:
                // Perform STORE WORD
                if(immediate >= 0 && immediate < NUMMEMORY){
                    int memoryAddress = state->reg[regA] + immediate;
                    if(memoryAddress >= 0 && memoryAddress < NUMMEMORY){
                        memoryAddress = cache->accessCache(memoryAddress, true);
                        state->mem[memoryAddress] = state->reg[regB];
                        state->pc += 1;
                    } else{
                        cout << "Error: Invalid memory address access.\n";
                        exit(1);
                    }
                } else{
                    cout << "Error: Invalid memory offset.\n)";
                    exit(1);
                }
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

// Ensure that blockSizeInWords, numberOfSets, and blocksPerSet are in powers of two
int nearestPowerOfTwo(int value) {
    if( value <= 1 ){
        return 1;
   }
        
    value--;
    value |= value >> 1;
    value |= value >> 2;
    value |= value > 4;
    value |= value >> 8;
    value |= value >> 16;
    value++;

    return value;
}

int main(int argc, char *argv[]) {
    if (argc != 5) { // Accept only 4 arguments, else output error message
        printf("Usage: %s program.mc blockSizeInWords numberOfSets blocksPerSet\n", argv[0]);
        return 1;
    }
    FILE *filePtr; // Declare File
    filePtr = fopen(argv[1], "r"); // Opens the file
    if (filePtr == NULL) {
	    printf("Error: can't open file %s", argv[1]);
	    perror("fopen");
	    exit(1);
    }

    // Read in the entire machine-code file into memory
    stateType state; // Declare state
    char line[MAXLINELENGTH];
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

    // Initialize Cache
    int blockSizeInWords = nearestPowerOfTwo(atoi(argv[2]));
    int numberOfSets = nearestPowerOfTwo(atoi(argv[3]));
    int blocksPerSet = nearestPowerOfTwo(atoi(argv[4]));

    Cache cache(blockSizeInWords, numberOfSets, blocksPerSet, &state);

    process(&state, &cache);   

    return 0;
};


