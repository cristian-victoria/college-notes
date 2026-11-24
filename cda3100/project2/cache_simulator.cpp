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
    int lruCounter;
    vector <int> dataSet;
};

// THE FOLLOWING IS RELATED TO CACHE
class Cache 
{
public:
    // Initialize cache with invalid block state
    Cache(int blockSizeInWords, int numberOfSets, int blocksPerSet, stateType *s){
        this->blockSizeInWords = blockSizeInWords;
        this->numberOfSets = numberOfSets;
        this->blocksPerSet = blocksPerSet;
        totalNumWords = blockSizeInWords * numberOfSets * blocksPerSet;
        state = s;

        // Resize the outer vector (the sets)
        cache.resize(numberOfSets);

        // Resize the inner vector (the blocks) and initialize each cacheBlock
        for (int i = 0; i < numberOfSets; i++) {
            cache[i].resize(blocksPerSet);
            for (int j = 0; j < blocksPerSet; j++) {
                cache[i][j].isValidState = false;
                cache[i][j].dirtyBit = 0;
                cache[i][j].tag = 0;
                cache[i][j].lruCounter = 0;
                cache[i][j].dataSet.resize(blockSizeInWords);
            }
        }
    }
    
    // Function for processor simulation to access addresses in cache
    int getValue(int address){
        // Check if the value exists in the cache, and return that value if true
        int blockOffset = address % blockSizeInWords;
        int setIndex = (address / blockSizeInWords) % numberOfSets;
        int tag = address / blockSizeInWords;

        bool cacheHit = false;
        int hitIndex = -1;

        // Search for valid cache state and recognize hit if found
        for(int i = 0; i < blocksPerSet; ++i){
            if( cache[setIndex][i].isValidState && cache[setIndex][i].tag == tag ){
                cacheHit = true;
                hitIndex = i;
                break;
            }
        }

        if(cacheHit){
            // Increment lruCounter for all of the blocks but the one it hit
            for( int i = 0; i < blocksPerSet; i++ ){
                if(i != hitIndex){
                    cache[setIndex][i].lruCounter++;
                }
            }

            printAction(address, 1, cacheToProcessor);

            return cache[setIndex][hitIndex].dataSet[blockOffset];
        }

        // If cache miss, increment lruCounter for all of the blocks
        for(int i=0; i < blocksPerSet; ++i){
            cache[setIndex][i].lruCounter++;
        }

        // Find an invalid (empty) block to store the new data
        int specificBlock = -1;
        bool foundInvalid = false;
        for(int b = 0; b < blocksPerSet; b++){
            if(!cache[setIndex][b].isValidState){           
                cache[setIndex][b].dirtyBit = 0;
                cache[setIndex][b].tag = tag;
                cache[setIndex][b].lruCounter = 0;
                cache[setIndex][b].isValidState = true;
                foundInvalid = true;
                specificBlock = b;
                break;
            }
        }
        if(!foundInvalid){
            // If all are valid, find a block with highest lru counter to store the new data
            int lruIndex = 0;
            int lruCounterValue = cache[setIndex][0].lruCounter;
            for(int i = 0; i < blocksPerSet; i++){
                if(cache[setIndex][i].lruCounter > lruCounterValue){
                    lruCounterValue = cache[setIndex][i].lruCounter;
                    lruIndex = i;
                }
            }
            // If dirty bit == 1, evict data block from cache to memory
            if(cache[setIndex][lruIndex].dirtyBit == 1){
                int memAddressStore = storeDataBitCache(setIndex,lruIndex);
                printAction(memAddressStore, blockSizeInWords, cacheToMemory);
            }
            cache[setIndex][lruIndex].dirtyBit = 0;
            cache[setIndex][lruIndex].tag = tag;
            cache[setIndex][lruIndex].lruCounter = 0;
            cache[setIndex][lruIndex].isValidState = true;
            specificBlock = lruIndex;
        }

        // Write block from memory to cache
        int startAddress = (address/blockSizeInWords) * blockSizeInWords;
        for(int i = 0; i < blockSizeInWords; i++){
            cache[setIndex][specificBlock].dataSet[i] = state->mem[startAddress+i];
        }
        printAction(startAddress, blockSizeInWords, memoryToCache);

        getValue(address);
    }

    void storeValue(int regB, int address){
        int blockOffset = address % blockSizeInWords;
        int setIndex = (address / blockSizeInWords) % numberOfSets;
        int tag = address / blockSizeInWords;

        // Find an invalid (empty) block to store the new data
        int specificBlock = -1;
        bool foundInvalid = false;
        for(int b = 0; b < blocksPerSet; b++){
            if(!cache[setIndex][b].isValidState){
                cache[setIndex][b].dirtyBit = 1;
                cache[setIndex][b].tag = tag;
                cache[setIndex][b].lruCounter = 0;
                cache[setIndex][b].isValidState = true;
                foundInvalid = true;
                specificBlock = b;
                break;
            }
        }
        if(!foundInvalid){
            // If all are valid, find a block with highest lru counter to store the new data
            int lruIndex = 0;
            int lruCounterValue = cache[setIndex][0].lruCounter;
            for(int i = 0; i < blocksPerSet; i++){
                if(cache[setIndex][i].lruCounter > lruCounterValue){
                    lruCounterValue = cache[setIndex][i].lruCounter;
                    lruIndex = i;
                }
            }
            cache[setIndex][lruIndex].dirtyBit = 1;
            cache[setIndex][lruIndex].tag = tag;
            cache[setIndex][lruIndex].lruCounter = 0;
            cache[setIndex][lruIndex].isValidState = true;
            specificBlock = lruIndex;
        }

        // Write data block from memory to cache
        int startAddress = (address/blockSizeInWords) * blockSizeInWords;
        for(int i = 0; i < blockSizeInWords; i++){
            cache[setIndex][specificBlock].dataSet[i] = state->mem[startAddress+i];
        }
        printAction(startAddress, blockSizeInWords, memoryToCache);

        // Store data from processor to cache
        cache[setIndex][specificBlock].dataSet[blockOffset] = regB;
        printAction(address, 1, processorToCache);
    }

    int storeDataBitCache(int s, int b){
        // Write block from cache to memory when data bit is 1
        int setIndex = s;
        int specificBlock = b;

        int address = cache[setIndex][specificBlock].tag * blockSizeInWords;
        int startAddress = (address/blockSizeInWords) * blockSizeInWords;
        for(int i = 0; i < blockSizeInWords; i++){
            state->mem[startAddress+i] = cache[setIndex][specificBlock].dataSet[i];
        }

        return address;
    }

    void printAction(int address, int size, enum actionType type){
        cout << "@@@ transferring word [" << address << "-" << (address + size - 1) << "] ";

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
    bool alive = true;
    int countSteps = 0;

    while(alive) {        
        int instruction = cache->getValue(state->pc);

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
                    state->reg[regB] = cache->getValue(state->reg[regA]+immediate);
                    state->pc += 1;
                } else{
                    cout << "Error: Invalid memory offset.\n";
                    exit(1);
                }
                break;
            case 3:
                // Perform STORE WORD
                if(immediate >= 0 && immediate < NUMMEMORY){
                    cache->storeValue(state->reg[regB], state->reg[regA]+immediate);
                    state->pc += 1;
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
                alive = false;
                break;
            case 7:
                // NO OP
                break;
            default:
                // HALT if any other opcode
                alive = false;
                exit(1);
                break;
        }
    }
}

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
    if (argc != 5) { // Accept only 5 arguments, else output error message
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
