/*Usage:
nWays obj-name(size,assoc)

for random-eviction:
	obj-name.refer_random(tag);
for lru-eviction:
	obj-name.refer_LRU(tag);
for tournament:
	obj-name.tournament(tag);

for displaying cache performance:
	obj-name.write_to(tag)
*/
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <vector>
//#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define BLOCK_SIZE      64
#define CNSTTNCY_OFFSET 64
#define CNSTTNCY_BITS    5
#define THRESHOLD      512
#define LRU              0
#define MRU              1
#define FOLLOWER         2
#define LRU_POLICY       0
#define RAN_POLICY       1
#define TOURN_POLICY     2

struct block{
  unsigned  tag;
  short lru_age = -1;
};

struct set{
  std::vector<block> b;
};

class nWays{

  unsigned long long index_mask, cacheSize, numSets;
  unsigned psel;
  int ways,policy;
  std::unordered_map < int, set > Table;
  std::vector <unsigned> blockType;

 public:
  unsigned long long hits, misses, cm, conflict, trace_count;
  unsigned long long p1,p2;

  nWays(unsigned long long, int, int);
  void refer_LRU(unsigned long long);
  void refer_random(unsigned long long);
  void tournament(unsigned long  long);
  void write_to(FILE *);
}; //end of nWays

nWays::nWays(unsigned long long _cacheSize, int _ways, int _policy){

  unsigned long long mask;
  unsigned long long set_1,set_2, set_3;
  p1 = p2 = psel = 0;
  srand(time(NULL));
  ways = _ways;
  policy =_policy;
  cacheSize = _cacheSize;

  numSets = (cacheSize/(64*ways));
  index_mask = numSets;

  blockType.resize(numSets);

  trace_count = misses = hits = cm = conflict = 0;
  mask = CNSTTNCY_OFFSET - 1;
  //initializes set for tournament eviction
  for (unsigned i = 0; i < numSets; i++) {
    set_1 = (i >> CNSTTNCY_BITS) & mask;
    set_2 = (~i) & mask;
    set_3 = i & mask;
    if(set_1 == set_2){
      blockType[i] = MRU;
    }
    else if(set_1 == set_3){
      blockType[i] = LRU;
    }
    else{
      blockType[i] = FOLLOWER;
    }
  }
}

void nWays::write_to(FILE *trace){
switch (policy) {
  case LRU_POLICY: fprintf(trace,"set %d-way %s ",ways,"LRU");
      break;
  case RAN_POLICY: fprintf(trace,"set %d-way %s ",ways,"Random");
      break;
  case TOURN_POLICY: fprintf(trace,"set %d-way %s ",ways,"tournament");
      break;
  default: fprintf(trace,"INVALID");
      break;
}
 fprintf(trace,"\t%llu B ",cacheSize);
 fprintf(trace,"H: %llu \tM: %llu ",hits,misses);
 fprintf(trace,"\tHR: %f",(double) 100*hits/trace_count);
 fprintf(trace,"\tMR: %f \n",(double) 100*misses/trace_count);

 }

inline void nWays::refer_LRU(unsigned long long address){
    // Implements LRU eviction policy

    int oldAge = -1;
    unsigned long long localTag   = address / index_mask;
    unsigned long long localIndex = address % index_mask;
    set s;
    s.b.resize(ways);
    block lb;
	trace_count++;

    if(Table.find(localIndex) == Table.end()){
      //Address not in Cache. Create and add new set
      misses++;

     //add new set
      s.b[0].tag = localTag;
      s.b[0].lru_age = 0;
      Table[localIndex] = s;
      return ;
    }
      //Address in Cache, check for Tag Match in Set
      s = Table[localIndex];
      for(int i = 0; i < ways ; i++){
         if(s.b[i].tag == localTag){
            hits++;
            //if found, move to MRU position
            oldAge = s.b[i].lru_age;
            s.b[i].lru_age = 0;
              for(int j = 0 ; j < ways; j++){
              //update indicies of all other entries
               if(j!=i && s.b[j].lru_age < oldAge && s.b[j].lru_age != -1 ){
 	               s.b[j].lru_age = s.b[j].lru_age < (ways - 1) ? s.b[j].lru_age + 1 : s.b[j].lru_age;
             }
           }//end for
              Table[localIndex] = s;
              return ;
        }//end tag search
      }//end for

      //address absent, add entry
          misses++;
          for(int i = 0 ; i < ways ; i++){
             if(s.b[i].lru_age == -1){
               //if empty slot found, add entry
               s.b[i].tag = localTag;
               s.b[i].lru_age = 0;
               for(int j = 0 ; j < ways; j++){
                //update indicies of all other entries
                if(j!=i && s.b[j].lru_age != - 1){
  	               s.b[j].lru_age = s.b[j].lru_age < (ways - 1) ? s.b[j].lru_age + 1 : s.b[j].lru_age;
                  }
               }
               //new entry added, stop search
               //return updated block to Table
              Table[localIndex]=s;
             return;
        }
      }

          //no space in set. Time to evict an entry!
           int oldestIndex = -1, oldest = -1;
           for(int i = 0; i < ways; i++){
              // find oldest entry
               if(s.b[i].lru_age > oldest){
                 oldest = s.b[i].lru_age;
                 oldestIndex = i;
               }
              }
               //eviction is essentially overwriting in our current model
      	  s.b[oldestIndex].tag = localTag;
      	  s.b[oldestIndex].lru_age = 0;

          for(int j = 0 ; j < ways; j++){
             //update indicies of all other entries
             if(j!= oldestIndex){
  	               s.b[j].lru_age = s.b[j].lru_age < (ways - 1) ? s.b[j].lru_age + 1 : s.b[j].lru_age;
              }
            }
           //return updated block to Table
          Table[localIndex]=s;
  }//end of refer


inline void nWays::refer_random(unsigned long long address){
   // Implements Random eviction policy

   unsigned long long localTag   = address / index_mask;
   unsigned long long localIndex = address % index_mask;

   int victim;
   set s;
   s.b.resize(ways);
   trace_count++;
   if(Table.find(localIndex) == Table.end()){
     	//Address not in Cache. Create and add new set
     	misses++;
     	//cm++;
     	//add new set and add cacheline to it
     	s.b[0].tag = localTag;
     	// age is used to check whether a block is empty (age = -1) in the set
     	s.b[0].lru_age = 0;
     	Table[localIndex] = s;
	return ;
     }
   //Address in Cache, check for Tag match in Set
   s = Table[localIndex];
   //check for Tag Match
   for(int i = ways - 1; i >= 0 ; i--){
     if(s.b[i].tag == localTag){
       // found = 1;
       hits++;
       return ;
     }
   }

     //address absent, add entry
     misses++;
     //conflict++;
     for(int i = ways-1 ; i >= 0 ; i--){
       if(s.b[i].lru_age == -1){
	 //if empty slot found, add entry
	 // notEmpty = 0;
	 s.b[i].tag = localTag;
	 s.b[i].lru_age = 0;
	 //new entry added, stop search
	 Table[localIndex] = s;
	 return ;
       }
     }

     //no space in set. Time to evict an entry!
     victim = rand() % ways;
     //eviction is essentially overwriting in our current model
     s.b[victim].tag = localTag;
     s.b[victim].lru_age = 0;
     //return updated block to Table
     Table[localIndex] = s;

     // }//if entry not found in index

}//end of refer_random

inline void nWays::tournament(unsigned long long address){
 /* Implements set dueling between LRU and Random Cache
  Eviction Policies. THe model penalizes each policy for
  each miss it takes */

 unsigned long long localIndex = address % index_mask;
 unsigned type = blockType[localIndex];
 unsigned policy, miss;

 policy = (type == FOLLOWER) ? (psel >= THRESHOLD) : type;
 miss = misses;
 switch (policy) {
   case LRU: refer_LRU(address);
 if(miss < misses){
                 psel = psel < 1024 ? psel + 1 : psel;
                 ++p1;
 }
       break;
   case MRU: refer_random(address);
 if(miss < misses){
                 psel = psel > 0 ? psel - 1: psel;
                 ++p2;
 }
       break;
   default: std::cout<<"\n tournament Draw?!";
       break;
 }
}
