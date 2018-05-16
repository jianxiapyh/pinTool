#ifndef TEST_LRU_HPP
#define TEST_LRU_HPP

#include <stdlib.h>

typedef unsigned long long addr_t;

const int SET_NUM = 8192;
const int SET_SIZE = 4;
const int BLOCK_SIZE = 64;

struct Memblock {
    addr_t addr_line;
    int recency_val;
    int is_dirty;
}

inline int MissCheck(const VOID * addr);

struct Memblock lru_cache[SET_NUM][SET_SIZE] = { 0 };
int set_usage[SET_NUM] = { 0 };


inline addr_t MissCheck(const VOID * addr) 
{

   addr_t cache_set, fetch_addr;
   addr_t evict_addr;
 
   fetch_addr = (unsigned long long)addr;
   fetch_addr = fetch_addr / BLOCK_SIZE;

   // TODO: use set_mask (use & instead of %)
   cache_set = block addr % SET_NUM;
   
   int usage_index = set_usage[cache_set];
   
   for (int i = 0; i <= usage_index; i++) {
	if(lru_cache[cache_set][i].addr_line == fetch_addr) {
		int most_recent_prev_index = lru_cache[cache_set][i].recency_val;
		lru_cache[cache_set][i].recency_val = 0;	
	        // TODO: fix this	
		for (int i = 0; i <= usage_index; i++) {
			if (lru_cache[cache_set][i].recency_val < most_recent_prev_index) {
				lru_cache[cache_set][i].recency_val++;
			}
		}
	return 
	}
	
   }

   if (usage_index < 3) {
	usage_index++;
	lru_cache[cache_set][usage_index].addr_line = fetch_addr;
	lru_cache[cache_set][usage_index].recency_val = 0;
	set_usage[cache_set] = usage_index;
	
	for (int i = 0; i < usage_index; i++) {
		lru_cache[cache_set][i].recency_val++;
	}
   } else {
	for (int i = 0; i < set_usage[cache_set]; i++) {
		if (lru_cache[cache_set][i] == set_usage[cache_set]) {
			usage_index = i;
			lru_cache[cache_set][i].recency_val = 0;
		} else {
			lru_cache[cache_set][i].recency_val++;
		}
	}
	
	lru_cache[cache_set][usage_index].addr_line = fetch_addr;
   }	


}



#endif /* TEST_LRU_HPP */
