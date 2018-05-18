#include <stdlib.h>

#define SET_BITS 12
#define SET_NUM 4096
#define SET_SIZE 8
#define BLOCK_SIZE 64

#define CACHE_HIT 1
#define MISS_NO_EVICT 0

#define READ_OP 0
#define WRITE_OP 1

typedef unsigned long long addr_t;

struct Memblock {
    addr_t addr_line;
    int recency_val;
    int is_dirty;
};

inline addr_t MissCheck(const int ins_op, const addr_t block_addr);
inline void InitMask();

struct Memblock lru_cache[SET_NUM][SET_SIZE] = { 0 };
int set_usage[SET_NUM];
addr_t set_mask;

inline void InitMask() {

   set_mask = 0x0;
   for (int i = 0; i < SET_BITS; i++) {
	set_mask = set_mask | (0x1 << i );
   }
}

inline addr_t MissCheck(const int ins_op, const addr_t block_addr) {

   addr_t wb_addr = 0;
   addr_t cache_set;
   cache_set = block_addr & set_mask;
  
   int usage_index;
   usage_index = set_usage[cache_set];

   bool is_hit = false;
   for (int i = 0; i <= usage_index; i++) {
	lru_cache[cache_set][i].recency_val++;
	if(lru_cache[cache_set][i].addr_line == block_addr) {
		lru_cache[cache_set][i].recency_val = 0;
		if (ins_op) {
			lru_cache[cache_set][i].is_dirty = 1;
		}
		is_hit = true;
      	}
   }

   if(is_hit)
     return CACHE_HIT;

   if (usage_index < SET_SIZE - 1) {
	usage_index++;
	set_usage[cache_set] = usage_index;
	lru_cache[cache_set][usage_index].addr_line = block_addr;
	lru_cache[cache_set][usage_index].recency_val = 0;
	if (ins_op) {
		lru_cache[cache_set][usage_index].is_dirty = 1;
	}
	
	return MISS_NO_EVICT;
	
   } else {
	bool is_evicted = false;
	for (int i = 0; i < set_usage[cache_set]; i++) {
		if ((!is_evicted) && (lru_cache[cache_set][i].recency_val >= set_usage[cache_set])) {
			if (lru_cache[cache_set][i].is_dirty) {
				wb_addr = lru_cache[cache_set][i].addr_line;  
			}
			lru_cache[cache_set][i].addr_line = block_addr;
			lru_cache[cache_set][i].recency_val = 0;
			is_evicted = true;
		} 
	}
	
   }	

   return wb_addr;
}
