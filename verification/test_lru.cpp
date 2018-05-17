#include <stdlib.h>
#include <iostream>

#define SET_BITS 13
#define SET_NUM 8192
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

   addr_t evict_addr = 0;
   addr_t cache_set;
   cache_set = block_addr & set_mask;
  
   int usage_index;
   usage_index = set_usage[cache_set];

   bool isHit = false;
   for (int i = 0; i <= usage_index; i++) {
	lru_cache[cache_set][i].recency_val++;
	if(lru_cache[cache_set][i].addr_line == block_addr) {
		lru_cache[cache_set][i].recency_val = 0;
		isHit = true;
      	}
   }

   if(isHit)
     return CACHE_HIT;

   std::cout << "Miss, need to insert or evict" << std::endl;
   if (usage_index < SET_SIZE - 1) {
     //	for (int i = 0; i < usage_index; i++) {
     //		lru_cache[cache_set][i].recency_val++;
     //	}
	std::cout << "usage_index = " << usage_index << std::endl;
	usage_index++;
	set_usage[cache_set] = usage_index;
	lru_cache[cache_set][usage_index].addr_line = block_addr;
	lru_cache[cache_set][usage_index].recency_val = 0;
	
	return MISS_NO_EVICT;
	
   } else {
	for (int i = 0; i < set_usage[cache_set]; i++) {
		if ((!evict_addr) && (lru_cache[cache_set][i].recency_val >= set_usage[cache_set])) {
			evict_addr = lru_cache[cache_set][i].addr_line;  
			lru_cache[cache_set][i].addr_line = block_addr;
			lru_cache[cache_set][i].recency_val = 0;
		} //else {
		//	lru_cache[cache_set][i].recency_val++;
		//}
	}
	
   }	

   return evict_addr;
}

static long long unsigned total_misses = 0;

void printSetStatus(const addr_t cache_set)
{
  std::cout << "SET_STATUS\ncache_set = " << cache_set << std::endl;
  for(int i = 0; i < SET_SIZE; ++i)
    {
      std::cout << "set = " << i << ", " << (unsigned long long)lru_cache[cache_set][i].addr_line << "\t" << lru_cache[cache_set][i].recency_val << "\t" << lru_cache[cache_set][i].is_dirty;
      std::cout << std::endl;
    }

  std::cout << std::endl;
}
  

void read(addr_t fetch_addr)
{
  int block_addr = fetch_addr / BLOCK_SIZE;

  int cache_miss = cache_miss = MissCheck(READ_OP, block_addr);

  // TODO: finish (store in array buffer, write to file)
  if(cache_miss == 0) {
    total_misses++;
  } else if (cache_miss == 1) {

  } else {

  }

  printSetStatus(block_addr & set_mask);
    
}

int main()
{


  for(int i = 0; i < SET_NUM; ++i)
    set_usage[i] = -1;
  
  InitMask();
  
  read(9971505);
  read(9971506);
  read(9971507);
  read(9984);
  read(3680000);
  read(7350016);
  read(7874304);
  read(8398592);
  read(8922880);
  read(9447168);
  read(10495744);

  read(7350016);
    read(7350016);
  read(7350016);


  read(11020032);
  read(11544320);
  read(9971456);
  read(12068608);
  read(12592896);
  read(9971456);
  read(9970546);

  return 0;
}
