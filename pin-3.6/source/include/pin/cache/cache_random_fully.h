/********************************************************************************
Objective: Modelling Random fully associative cache eviction policy.
Author: Meghana Gaopande

Usage:
RandomCache_full obj-name(number_of_cache_lines);

Pass a tag to the cache:
obj-name.refer(string_tag);

To display cache results:
obj-name.display(File *file)

**********************************************************************************/
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;
using std::string;
/********************************************************************************************/
/* FULLY ASSOCIATIVE************************************************************************/
/* Note: Heavily borrowed from https://www.geeksforgeeks.org/lru-cache-implementation ******/
/*******************************************************************************************/
class RandomCache_full
{
    // store keys of
    //This is storing the tags as strings because STLport doesnt support
    //unordered map with unsigned long long data type
    vector < string > cache;

    // store references of key in cache
    unordered_map < string, unsigned long > hash;
    unsigned long long int csize; //maximum capacity of cache (number of lines)
    unsigned long long int hit;
    unsigned long long int miss;

public:
    RandomCache_full(unsigned long long);
    void refer(string);
    void display(FILE*);
};

RandomCache_full::RandomCache_full(unsigned long long n)
{
    csize = n;
    hit = 0;
    miss = 0;
}

inline void RandomCache_full::refer(string tag)
{
    unsigned long index;

    // not present in hash map, so not in cache
    if (hash.find(tag) == hash.end())
    {
        miss++;
        // cache is full
        if (cache.size() == csize)
        {
            //Replace a random line with the tag
            srand(time(NULL));
            index = rand() % cache.size();
            hash.erase(cache[index]);
            cache[index] = tag;
            hash[tag] = index;
        }
        else
        {
          //else add it to the cache as next line, and create a hash map entry
          cache.push_back(tag);
          hash[tag] = cache.size() - 1;
        }
    }

    // present in cache
    else
    {
        hit++;
    }

}

//Display the contents of cache ie. print to the output file
void RandomCache_full::display(FILE *trace)
{

    fprintf(trace,"Random Fully ");
    fprintf(trace,"%llu B ",csize*64);
    fprintf(trace,"H: %llu \tM: %llu ",hit,miss);
    fprintf(trace,"\tHR: %f",(double)100*hit/(hit+miss));
    fprintf(trace,"\tMR: %f \n",(double)100*miss/(hit+miss));
}
