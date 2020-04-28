#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

using namespace std;

int direct_mapped(long addr, long * cache, int size){
    int index = (addr / 32) % (size / 32);
    
    if(cache[index] == 32 * (addr / 32)){
        return 1;
    }
    else{
        cache[index] = 32 * (addr / 32);
        return 0;
    }   
}

int set_associative(long addr, long **cache, long **lru, int ways, int * lru_count){
    
    int lines = 512 / ways;
    int index = (addr / 32) % lines;
    
    for (int i = 0; i < ways; i++){
        if (cache[index][i] == 32 * (addr / 32)){
            *lru_count = *lru_count + 1;
            lru[index][i] = *lru_count;
            return 1;
        }
    }
    
    int val = lru[index][0];
    int temp = 0;
    for(int i = 0; i < ways; i++){
        if(lru[index][i] < val){
            val = lru[index][i];
            temp = i;
        }
    }
    *lru_count = *lru_count + 1;
    lru[index][temp] = *lru_count;
    cache[index][temp] = 32 * (addr / 32);
    
    return 0;
}

int set_associative_no_alloc(string ls, long addr, long **cache, long **lru, int ways, int * lru_count){
    
    int lines = 512 / ways;
    int index = (addr / 32) % lines;
    
    for (int i = 0; i < ways; i++){
        if (cache[index][i] == 32 * (addr / 32)){
            *lru_count = *lru_count + 1;
            lru[index][i] = *lru_count;
            return 1;
        }
    }
    
    if(ls == "L"){
        int val = lru[index][0];
        int temp = 0;
        for(int i = 0; i < ways; i++){
            if(lru[index][i] < val){
                val = lru[index][i];
                temp = i;
            }
        }
        *lru_count = *lru_count + 1;
        lru[index][temp] = *lru_count;
        cache[index][temp] = 32 * (addr / 32);   
    }
    
    return 0;
}

int set_associative_nlp(long addr, long **cache, long **lru, int ways, int * lru_count){
    
    int lines = 512 / ways;
    int index = (addr / 32) % lines;
    
    int check = 0;
    int ret = 0;
    
    //check first line
    for (int i = 0; i < ways; i++){
        if (cache[index][i] == 32 * (addr / 32)){
            *lru_count = *lru_count + 1;
            lru[index][i] = *lru_count;
            ret = ret + 1;
            check = 1;
        }
    }
    
    if(check == 0){
        int val = lru[index][0];
        int temp = 0;
        for(int i = 0; i < ways; i++){
            if(lru[index][i] < val){
                val = lru[index][i];
                temp = i;
            }
        }
        *lru_count = *lru_count + 1;
        lru[index][temp] = *lru_count;
        cache[index][temp] = 32 * (addr / 32);
    }
    
    //check line + 1
    check = 0;
    int next_index = ((addr / 32) + 1) % lines;
    
    for (int i = 0; i < ways; i++){
        if (cache[next_index][i] == 32 * ((addr / 32) + 1)){
            *lru_count = *lru_count + 1;
            lru[next_index][i] = *lru_count;
            //ret = ret + 1;
            check = 1;
        }
    }
    
    if(check == 0){
        int val = lru[next_index][0];
        int temp = 0;
        for(int i = 0; i < ways; i++){
            if(lru[next_index][i] < val){
                val = lru[next_index][i];
                temp = i;
            }
        }
        *lru_count = *lru_count + 1;
        lru[next_index][temp] = *lru_count;
        cache[next_index][temp] = 32 * ((addr / 32) + 1);
    }
    
    return ret;
}

int set_associative_missnlp(long addr, long **cache, long **lru, int ways, int * lru_count){
    
    int lines = 512 / ways;
    int index = (addr / 32) % lines;
    
    int check = 0;
    
    //check first line
    for (int i = 0; i < ways; i++){
        if (cache[index][i] == 32 * (addr / 32)){
            *lru_count = *lru_count + 1;
            lru[index][i] = *lru_count;
            return 1;
        }
    }
    

    int val = lru[index][0];
    int temp = 0;
    for(int i = 0; i < ways; i++){
        if(lru[index][i] < val){
            val = lru[index][i];
            temp = i;
        }
    }
    *lru_count = *lru_count + 1;
    lru[index][temp] = *lru_count;
    cache[index][temp] = 32 * (addr / 32);
    
    //check line + 1
    check = 0;
    int next_index = ((addr / 32) + 1) % lines;
    
    for (int i = 0; i < ways; i++){
        if (cache[next_index][i] == 32 * ((addr / 32) + 1)){
            *lru_count = *lru_count + 1;
            lru[next_index][i] = *lru_count;
            return 0;
        }
    }
    
    val = lru[next_index][0];
    temp = 0;
    for(int i = 0; i < ways; i++){
        if(lru[next_index][i] < val){
            val = lru[next_index][i];
            temp = i;
        }
    }
    *lru_count = *lru_count + 1;
    lru[next_index][temp] = *lru_count;
    cache[next_index][temp] = 32 * ((addr / 32) + 1);
    
    return 0;
}

int fully_associative_lru(long addr, long * cache, long * lru, int * lru_count){

    addr = 32 * (addr / 32);
    
    for(int i=0; i < 512; i++){
        if(cache[i] == addr){
            *lru_count = *lru_count + 1;
            lru[i] = *lru_count;
            return 1;
        }
    }
    
    int index = 0;
    int temp = lru[0];
    for(int i = 0; i < 512; i++){
        if(lru[i] < temp){
            temp = lru[i];
            index = i;
        }
    }
    *lru_count = *lru_count + 1;
    lru[index] = *lru_count;
    cache[index] = addr;
    
    return 0;
}

int fully_associative_hc(long addr, long * cache, long * hc_bits){

    addr = 32 * (addr / 32);
    
    for(int i=0; i < 512; i++){
        if(cache[i] == addr){
            //update bits here
            return 1;
        }
    }
    
    
    //cache[cold bit index] = tag
    //update bits here
    
    //***below does not work
    int index = 512 / 2;
    while(hc_bits[index] != 0){
        if(hc_bits[index] == 0) hc_bits[index] = 1;
        else hc_bits[index] = 0;
        index = index / 2;
    }
    cache[index] = addr;
    
    return 0;
}


int main(int argc, char *argv[]){

    //direct mapped cache
    long * direct_cache1 = new long[32];
    long * direct_cache4 = new long[128];
    long * direct_cache16 = new long[512];
    long * direct_cache32 = new long[1024];
    int direct_Count1 = 0;
    int direct_Count4 = 0;
    int direct_Count16 = 0;
    int direct_Count32 = 0;
    
    //set associative cache
    long **assoc_cache2 = new long *[256];
    long **assoc_lru2 = new long *[256]; 
    for(int i=0; i < 256; i++){
        assoc_cache2[i] = new long[2];
        assoc_lru2[i] = new long[2];
    }
    
    long **assoc_cache4 = new long *[128];           
    long **assoc_lru4 = new long *[128];          
    for(int i=0; i < 128; i++){
        assoc_cache4[i] = new long[4];
        assoc_lru4[i] = new long[4];
    }
    
    long **assoc_cache8 = new long *[64];         
    long **assoc_lru8 = new long *[64];         
    for(int i=0; i < 64; i++){
        assoc_cache8[i] = new long[8];
        assoc_lru8[i] = new long[8];
    }
    
    long **assoc_cache16 = new long *[32];          
    long **assoc_lru16 = new long *[32];            
    for(int i=0; i < 32; i++){
        assoc_cache16[i] = new long[16];
        assoc_lru16[i] = new long[16];
    }
    int lru_count2 = 0;
    int lru_count4 = 0;
    int lru_count8 = 0;
    int lru_count16 = 0;
    int assoc_count2 = 0;
    int assoc_count4 = 0;
    int assoc_count8 = 0;
    int assoc_count16 = 0;
    
    //set associative cache (no allocation on a write miss)
    long **assoc_cache_na_2 = new long *[256];
    long **assoc_lru_na_2 = new long *[256]; 
    for(int i=0; i < 256; i++){
        assoc_cache_na_2[i] = new long[2];
        assoc_lru_na_2[i] = new long[2];
    }
    
    long **assoc_cache_na_4 = new long *[128];      
    long **assoc_lru_na_4 = new long *[128];           
    for(int i=0; i < 128; i++){
        assoc_cache_na_4[i] = new long[4];
        assoc_lru_na_4[i] = new long[4];
    }
    
    long **assoc_cache_na_8 = new long *[64];      
    long **assoc_lru_na_8 = new long *[64];          
    for(int i=0; i < 64; i++){
        assoc_cache_na_8[i] = new long[8];
        assoc_lru_na_8[i] = new long[8];
    }
    
    long **assoc_cache_na_16 = new long *[32];         
    long **assoc_lru_na_16 = new long *[32];               
    for(int i=0; i < 32; i++){
        assoc_cache_na_16[i] = new long[16];
        assoc_lru_na_16[i] = new long[16];
    }
    int lru_count_na_2 = 0;
    int lru_count_na_4 = 0;
    int lru_count_na_8 = 0;
    int lru_count_na_16 = 0;
    int assoc_count_na_2 = 0;
    int assoc_count_na_4 = 0;
    int assoc_count_na_8 = 0;
    int assoc_count_na_16 = 0;
    
    //set associative cache (next line prefetching)
    long **assoc_cache_nlp_2 = new long *[256];
    long **assoc_lru_nlp_2 = new long *[256]; 
    for(int i=0; i < 256; i++){
        assoc_cache_nlp_2[i] = new long[2];
        assoc_lru_nlp_2[i] = new long[2];
    }
    
    long **assoc_cache_nlp_4 = new long *[128];         
    long **assoc_lru_nlp_4 = new long *[128];           
    for(int i=0; i < 128; i++){
        assoc_cache_nlp_4[i] = new long[4];
        assoc_lru_nlp_4[i] = new long[4];
    }
    
    long **assoc_cache_nlp_8 = new long *[64];    
    long **assoc_lru_nlp_8 = new long *[64];      
    for(int i=0; i < 64; i++){
        assoc_cache_nlp_8[i] = new long[8];
        assoc_lru_nlp_8[i] = new long[8];
    }
    
    long **assoc_cache_nlp_16 = new long *[32];        
    long **assoc_lru_nlp_16 = new long *[32];              
    for(int i=0; i < 32; i++){
        assoc_cache_nlp_16[i] = new long[16];
        assoc_lru_nlp_16[i] = new long[16];
    }
    int lru_count_nlp_2 = 0;
    int lru_count_nlp_4 = 0;
    int lru_count_nlp_8 = 0;
    int lru_count_nlp_16 = 0;
    int assoc_count_nlp_2 = 0;
    int assoc_count_nlp_4 = 0;
    int assoc_count_nlp_8 = 0;
    int assoc_count_nlp_16 = 0;
    
    //set associative cache (next line prefetching on miss)
    long **assoc_cache_missnlp_2 = new long *[256];
    long **assoc_lru_missnlp_2 = new long *[256]; 
    for(int i=0; i < 256; i++){
        assoc_cache_missnlp_2[i] = new long[2];
        assoc_lru_missnlp_2[i] = new long[2];
    }
    
    long **assoc_cache_missnlp_4 = new long *[128];         
    long **assoc_lru_missnlp_4 = new long *[128];           
    for(int i=0; i < 128; i++){
        assoc_cache_missnlp_4[i] = new long[4];
        assoc_lru_missnlp_4[i] = new long[4];
    }
    
    long **assoc_cache_missnlp_8 = new long *[64];         
    long **assoc_lru_missnlp_8 = new long *[64];         
    for(int i=0; i < 64; i++){
        assoc_cache_missnlp_8[i] = new long[8];
        assoc_lru_missnlp_8[i] = new long[8];
    }
    
    long **assoc_cache_missnlp_16 = new long *[32];       
    long **assoc_lru_missnlp_16 = new long *[32];            
    for(int i=0; i < 32; i++){
        assoc_cache_missnlp_16[i] = new long[16];
        assoc_lru_missnlp_16[i] = new long[16];
    }
    int lru_count_missnlp_2 = 0;
    int lru_count_missnlp_4 = 0;
    int lru_count_missnlp_8 = 0;
    int lru_count_missnlp_16 = 0;
    int assoc_count_missnlp_2 = 0;
    int assoc_count_missnlp_4 = 0;
    int assoc_count_missnlp_8 = 0;
    int assoc_count_missnlp_16 = 0;
    
    //fully associative lru
    long * fully_assoc_cache_lru = new long[512];
    long * fully_assoc_lru = new long[512];
    int fully_assoc_count_lru = 0;
    int lru_count_fa = 0;
    
    //fully associative hot cold bits
    long * hot_cold = new long[512];
    long * fully_assoc_cache_hc = new long[512];
    int fully_assoc_count_hc = 0;
    
    //ifstream infile("test_input.txt");
    ifstream infile(argv[1]);
    
    //ofstream outfile
    FILE * output;
    output = fopen(argv[2], "w");
    
    //Temporary Variables
    string flag, line;
    long addr;

    int trace_counter = 0;
    
    while(getline(infile, line)) {
        stringstream s(line);
        s >> flag >> std::hex >> addr;
    
        //direct mapped counters
        direct_Count1 += direct_mapped(addr, direct_cache1, 1024);
        direct_Count4 += direct_mapped(addr, direct_cache4, 4096);
        direct_Count16 += direct_mapped(addr, direct_cache16, 16384);
        direct_Count32 += direct_mapped(addr, direct_cache32, 32768);
    
        //set associative counters
        assoc_count2 += set_associative(addr, assoc_cache2, assoc_lru2, 2, &lru_count2);
        assoc_count4 += set_associative(addr, assoc_cache4, assoc_lru4, 4, &lru_count4);
        assoc_count8 += set_associative(addr, assoc_cache8, assoc_lru8, 8, &lru_count8);
        assoc_count16 += set_associative(addr, assoc_cache16, assoc_lru16, 16, &lru_count16);
        
        //set associative counters (no allocation on write miss)
        assoc_count_na_2 += set_associative_no_alloc(flag, addr, assoc_cache_na_2, assoc_lru_na_2, 2, &lru_count_na_2);
        assoc_count_na_4 += set_associative_no_alloc(flag, addr, assoc_cache_na_4, assoc_lru_na_4, 4, &lru_count_na_4);
        assoc_count_na_8 += set_associative_no_alloc(flag, addr, assoc_cache_na_8, assoc_lru_na_8, 8, &lru_count_na_8);
        assoc_count_na_16 += set_associative_no_alloc(flag, addr, assoc_cache_na_16, assoc_lru_na_16, 16, &lru_count_na_16);
        
        //set associative counters (next line prefetching) 
        assoc_count_nlp_2 += set_associative_nlp(addr, assoc_cache_nlp_2, assoc_lru_nlp_2, 2, &lru_count_nlp_2);
        assoc_count_nlp_4 += set_associative_nlp(addr, assoc_cache_nlp_4, assoc_lru_nlp_4, 4, &lru_count_nlp_4);
        assoc_count_nlp_8 += set_associative_nlp(addr, assoc_cache_nlp_8, assoc_lru_nlp_8, 8, &lru_count_nlp_8);
        assoc_count_nlp_16 += set_associative_nlp(addr, assoc_cache_nlp_16, assoc_lru_nlp_16, 16, &lru_count_nlp_16);
        
        //set associative counters (next line prefetching on cache miss)
        assoc_count_missnlp_2 += set_associative_missnlp(addr, assoc_cache_missnlp_2, assoc_lru_missnlp_2, 2, &lru_count_missnlp_2);
        assoc_count_missnlp_4 += set_associative_missnlp(addr, assoc_cache_missnlp_4, assoc_lru_missnlp_4, 4, &lru_count_missnlp_4);
        assoc_count_missnlp_8 += set_associative_missnlp(addr, assoc_cache_missnlp_8, assoc_lru_missnlp_8, 8, &lru_count_missnlp_8);
        assoc_count_missnlp_16 += set_associative_missnlp(addr, assoc_cache_missnlp_16, assoc_lru_missnlp_16, 16, &lru_count_missnlp_16);
        
        //fully associative cache (LRU)
        fully_assoc_count_lru += fully_associative_lru(addr, fully_assoc_cache_lru, fully_assoc_lru, &lru_count_fa);
        
        //fully associative cache (HC)
        fully_assoc_count_hc += fully_associative_hc(addr, fully_assoc_cache_hc, hot_cold);
        
        trace_counter++;
    }
    
    fprintf(output, "%d,%d; %d,%d; %d,%d; %d,%d;\n", direct_Count1, trace_counter, direct_Count4, trace_counter, direct_Count16, trace_counter, direct_Count32, trace_counter);
    fprintf(output, "%d,%d; %d,%d; %d,%d; %d,%d;\n", assoc_count2, trace_counter, assoc_count4, trace_counter, assoc_count8, trace_counter, assoc_count16, trace_counter);
    fprintf(output, "%d,%d;\n", fully_assoc_count_lru, trace_counter);
    fprintf(output, "%d,%d;\n", fully_assoc_count_hc, trace_counter);
    fprintf(output, "%d,%d; %d,%d; %d,%d; %d,%d;\n", assoc_count_na_2, trace_counter, assoc_count_na_4, trace_counter, assoc_count_na_8, trace_counter, assoc_count_na_16, trace_counter);
    fprintf(output, "%d,%d; %d,%d; %d,%d; %d,%d;\n", assoc_count_nlp_2, trace_counter, assoc_count_nlp_4, trace_counter, assoc_count_nlp_8, trace_counter, assoc_count_nlp_16, trace_counter);
    fprintf(output, "%d,%d; %d,%d; %d,%d; %d,%d;\n", assoc_count_missnlp_2, trace_counter, assoc_count_missnlp_4, trace_counter, assoc_count_missnlp_8, trace_counter, assoc_count_missnlp_16, trace_counter);

    fclose(output);
    
    return 0;
}