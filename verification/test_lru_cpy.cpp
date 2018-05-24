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


  addr_t wb_addr = MISS_NO_EVICT;
  addr_t cache_set = block_addr & set_mask;
  
  int usage_index = set_usage[cache_set];

  bool is_hit = false;
  for(int i = 0; i <= usage_index; i++)
    {
      lru_cache[cache_set][i].recency_val++;
      if(lru_cache[cache_set][i].addr_line == block_addr)
	{
	  lru_cache[cache_set][i].recency_val = 0;

	  // if(ins_op) is true for WRITE_OP
	  if(ins_op)
	    {
	      lru_cache[cache_set][i].is_dirty = 1;
	    }
	  is_hit = true;
	}
    }

  if(is_hit)
    return CACHE_HIT;

  if (usage_index < SET_SIZE - 1)
    {
      // Insertion
      usage_index++;
      set_usage[cache_set] = usage_index;
      lru_cache[cache_set][usage_index].addr_line = block_addr;
      lru_cache[cache_set][usage_index].recency_val = 0;

      // Bring in cache and mark dirty for WRITPE_OP
      if(ins_op)
	{
	  lru_cache[cache_set][usage_index].is_dirty = 1;
	}
  	
      return MISS_NO_EVICT;
	
    }
  else
    {
	int lruIdx = -1;
	int largestRecencyVal = -1;
	for(int i = 0; i <= usage_index; i++)
	  {
	    //std::cout << lru_cache[cache_set][i].recency_val << " > " << lru << "\n";
	    if(lru_cache[cache_set][i].recency_val > largestRecencyVal)
	      {
		lruIdx = i;
		largestRecencyVal = lru_cache[cache_set][i].recency_val;
	      }
	  }
	//std::cout << "\nlru = " << lru << "\n";
	if(lru_cache[cache_set][lruIdx].is_dirty)
	  {
	    wb_addr = lru_cache[cache_set][lruIdx].addr_line;
	  }

	
	lru_cache[cache_set][lruIdx].addr_line = block_addr;
	lru_cache[cache_set][lruIdx].recency_val = 0;

	if(ins_op)
	  {
	    lru_cache[cache_set][lruIdx].is_dirty = 1;
	  }
	else
	  {
	    lru_cache[cache_set][lruIdx].is_dirty = 0;
	  }
	return wb_addr;
    }

      /*
      bool is_evicted = false;
      for (int i = 0; i <= usage_index; i++)
	{
	  if ((!is_evicted) && (lru_cache[cache_set][i].recency_val >= set_usage[cache_set]))
	    {
	      if (lru_cache[cache_set][i].is_dirty)
		{
		  wb_addr = lru_cache[cache_set][i].addr_line;  
		}
	      lru_cache[cache_set][i].addr_line = block_addr;
	      lru_cache[cache_set][i].recency_val = 0;
	      is_evicted = true;
	    } 
	}
      */
	
  
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

  int cache_miss = MissCheck(READ_OP, block_addr);

  // TODO: finish (store in array buffer, write to file)
  if(cache_miss == 0) {
    total_misses++;
  } else if (cache_miss == 1) {

  } else {

  }

  std::cout << cache_miss << std::endl;
  printSetStatus(block_addr & set_mask);
    
}

void write(addr_t fetch_addr)
{
  int block_addr = fetch_addr / BLOCK_SIZE;

  int cache_miss  = MissCheck(WRITE_OP, block_addr);

  // TODO: finish (store in array buffer, write to file)
  if(cache_miss == 0) {
    total_misses++;
  } else if (cache_miss == 1) {

  } else {

  }
  std::cout << cache_miss << std::endl;
  printSetStatus(block_addr & set_mask);
    
}

int main()
{


  for(int i = 0; i < SET_NUM; ++i)
    set_usage[i] = -1;
  
  InitMask();
  
  read(9971505); //155804
  read(9971506); //155804
  read(9971507); //155804
  write(9984); //156
  write(3680000); //57500
  write(7350016); //114844
  write(7874304); //123036
  write(8398592); //131228
  write(8922880); //139420
  write(9447168); //147612
  
  read(10495744); //163996

  write(7350016); //114844
  write(7350016); //114844
  write(7350016); //114844


  write(11020032); //172188
  write(11544320); //180380
  write(9971456); //155804
  write(12068608); //188572
  write(12592896); //196764
  write(9971456);
  write(9970546);

  return 0;
}
