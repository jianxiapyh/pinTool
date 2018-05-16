/********************************************************************************
Objective:
This is a C model of a 2 Mb 4 way set associative LRU cache.
This is used as LLC with Pin tool.

Author: Meghana Gaopande

Usage:
int wrapper(VOID * addr)

Pass the address to wrapper function. It returns 1 if this address is a LLC miss.
Else, returns 0.

**********************************************************************************/
#include <stdlib.h>

inline int refer(unsigned long long set_index, unsigned long long tag);
inline int wrapper(VOID * addr);
//Hash map
unsigned long long Hash[8192][4];
//Indicates the usage of each set (how many of the 4 cache lines are full?)
int set_usage[8192] = {-1};
//Indicates the recency of each cache line in each set
//0 is the most recent, 3 is least recent
int Recency[8192][4] = {-1};

//8192 sets
//4 ways
// Hash will contain the address of the pointers to each set (Each set is an array of 4)

inline int refer(unsigned long long set_index, unsigned long long tag)
{
    int index, found_index = 0;
    int i;
    int most_recent_prev_index;

    //Get the set usage index
	  index = set_usage[set_index];

    //Look for the tag in the set
    for (i  = index; i >= 0; i--)
    {
      if(Hash[set_index][i] == tag)
      {
        found_index = i;

        //Since the tag is found, update recencies
        most_recent_prev_index = Recency[set_index][found_index];
        Recency[set_index][found_index] = 0;
        for (int i =0; i<= set_usage[set_index]; i++)
        {
          if(Recency[set_index][i] < most_recent_prev_index)
          {
             Recency[set_index][i]++;
          }
        }

	  	return 0;
      }
    }

      //Not present. Add if the set is not full

      if(index < 3)
      {
        //Add tag to next index
      	index = index + 1;
        Hash[set_index][index] = tag;
        set_usage[set_index] = index;
        Recency[set_index][index] = 0;
        //Update recencies
        for (int i =0; i< index; i++)
        {
             Recency[set_index][i]++;
        }
        return 1;
      }
      else
      {
        //Set is full. Pop the least recent element.
        for(int i =0; i <= set_usage[set_index]; i++)
        {
          //set_usage[set_index] will indicate the least recent element's recency
          if(Recency[set_index][i] == set_usage[set_index])
          {
            index = i;
            Recency[set_index][i] = 0;
          }
          else
          {
            //Update recencies for existing elements
            Recency[set_index][i]++;
          }

        }
        //Add tag to cache
        Hash[set_index][index] = tag;

      }
      return 1;

}

/* Wrapper function: This gets the address from Pin tool
and processes it for passing to the refer function*/
inline int wrapper(VOID * addr)
{
  unsigned long long tag, set, p;
  VOID *q;
  int miss;
  q = addr;
  p = (unsigned long long)q;
  //Divide by 64 to obtain tag as cache line = 64 bytes
  tag = p/64;

  //Get the set number by modulo 8192 (number of sets)
  set = tag % 8192;
  //Get the tag by dividing by 8192 (number of sets)
  tag = tag/8192;

  miss = refer(set, tag);

  //Return miss status obtained from the refer function
  return miss;
}
