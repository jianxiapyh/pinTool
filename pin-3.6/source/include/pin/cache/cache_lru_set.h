#include <iostream>
#include <cstring>
#include <string>
#include <unordered_map>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <list>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/********************************************************************************************/
/* FULLY ASSOCIATIVE************************************************************************/
/* Note: Heavily borrowed from https://www.geeksforgeeks.org/lru-cache-implementation ******/
/*******************************************************************************************/
class LRUCache_N_way
{
    unordered_map< unsigned long , unsigned long > set_hash;
    vector< vector< unsigned long long > > sets;
    unsigned number_of_sets;
    unsigned number_of_ways; //2way, 4way
    unsigned long long int hit = 0;
    unsigned long long int miss = 0;

public:
    LRUCache_N_way(unsigned long long, unsigned long long);
    void refer(unsigned long long, unsigned long long );
    void display(FILE* trace);

};

LRUCache_N_way::LRUCache_N_way(unsigned long long m, unsigned long long n)
{
    number_of_sets = m;
    number_of_ways = n;
}

void LRUCache_N_way::refer(unsigned long long set_index, unsigned long long tag)
{
    vector<unsigned long long> set1;
    bool found = false;
    unsigned long long index, found_index;
    found_index = 0;
    // Set not present in hash table
    if (set_hash.find(set_index) == set_hash.end())
    {
      if(set_hash.size() < number_of_sets)
      {
        sets.push_back(set1);
        set_hash[set_index] = (sets.size() - 1);
        //cout << "created new set for " << set_index << endl;
      }
      //Create set
    }
    index = set_hash[set_index];

    for (unsigned i  = 0; i < sets[index].size(); i++)
    {
      if(sets[index][i] == tag)
      {
        found = true;
        found_index = i;

      }
    }
    if(found)
    {
      //cout << "hit for tag " << tag << " found at position " << found_index << " in set " << index << endl;
      //Already present. Update recency
      sets[index].erase(sets[index].begin() +found_index);
      sets[index].insert(sets[index].begin(), tag);
      hit++;
      //cout << "hit" << endl;
    }
    else
    {
      miss++;
      //cout << "miss" << endl;
      //Not present. Add if the set is not full
      if(sets[index].size() < number_of_ways)
      {
        sets[index].insert(sets[index].begin(), tag);
        //cout << "miss for tag " << tag << " added in set " << index << "at position 0" << endl;
      }
      else
      {
        //Set is full. Pop the least recent element.
        sets[index].pop_back();
        //cout << "miss for removed last element from set " << index << " and added this one at beginning" << endl;
        sets[index].insert(sets[index].begin(), tag);
      }
    }

}

// display contents of cache
void LRUCache_N_way::display(FILE* trace)
{

    fprintf(trace,"LRU Set ");
    fprintf(trace,"No. of sets used %llu ",(unsigned long long) sets.size() );
    fprintf(trace,"H: %llu \tM: %llu ",hit,miss);
    fprintf(trace,"\tHR: %f",(double)100*hit/(hit+miss));
    fprintf(trace,"\tMR: %f \n",(double)100*miss/(hit+miss));
}
