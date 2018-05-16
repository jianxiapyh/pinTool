/**************************************************************************
Objective: Modelling Direct Mapped Cache policy
Author: Devang Khamar

Usage: 
dmap obj-name(size)

for eviction: 
	obj-name.refer(tag);
	
for displaying cache performance: 
	obj-name.write_to();

***************************************************************************/



#include <stdio.h>
#include <unordered_map>

class dmap{
  unsigned long long cacheSize, num_blocks;
  unsigned long long index_mask, tag_mask;
  std::unordered_map <unsigned,unsigned> Table;

 public:
  unsigned long long hits, misses, trace_count;

  dmap(long long _cacheSize){/*
     INITIALIZES THE HASH TABLE. THE NUMBER OF ENTRIES IN THE HASH
     TABLE DEPENDS ON THE SIZE OF THE DRAM DATA CACHE SIZE. ASSUMING
     THAT DATA TRANSACTIONS AHPPEN AT BLOCK GRANULARITY, THE NUMBER OF
     ENTIRES IN THE CACHE WOULD BE _CACHESIZE(IN MB)/64B.
     E.G.: FOR 512MB CACHE, NUM_ENTRIES = 512MB/64B  = 8 MILLION ENTRIES
     NOTE: ROW BUFFER SIZE = 1 TO 8KB. SHOULD THE GRANULARITY OF DATA
     STORED IN DRAM CACHE BE 1 BLOCK OR 1 TO 8KB?
   */
    cacheSize = (_cacheSize);
    num_blocks = (cacheSize/64);
    index_mask =  num_blocks ;
    trace_count = misses = hits = 0;
   }


inline  void refer(unsigned long long address){
    /*
      This function operates the direct map mechanism. it checks
      whether the given entry is present in the cache and performs
      the necessary management. in case of an index hit, tag is checked .
      in case of a miss, cache entry at position is (over)written.
      we do no construct all entries of the cache, they are dynamically
      created, but otherwise the operation is very similar to a
      directly mapped cache.
     */

    unsigned localIndex = address % index_mask;
    unsigned localTag   = address / index_mask;

    trace_count++;

    if(Table.find(localIndex) == Table.end()) {
      misses++;
      //add new entry
      Table[localIndex] = localTag;
      return ;
    }
      //block already cached at index. Overwrite entry
    if((Table[localIndex] != localTag)){
      misses++;
      Table[localIndex] = localTag;
      return ;
    }
  hits++;

 }

 void write_to(FILE *trace){
	/*
	Writes the necessary statistics such as the hits, misses, hit rate,
	miss rate and cache size to a file.
	*/
	fprintf(trace,"DMap ");
	fprintf(trace,"%llu B ",cacheSize);
  	fprintf(trace,"H: %llu \tM: %llu ",hits,misses);
	fprintf(trace,"\tHR: %f",(double) 100*hits/trace_count);
	fprintf(trace,"\tMR: %f \n",(double) 100*misses/trace_count);
	}
};
