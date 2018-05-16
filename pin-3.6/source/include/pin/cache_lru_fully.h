/********************************************************************************
Objective: Modelling LRU fully associative cache eviction policy.
Author: Meghana Gaopande

Usage:
LRUCache_full obj-name(number_of_cache_lines);

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

using namespace std;
using std::string;
/********************************************************************************************/
/* FULLY ASSOCIATIVE************************************************************************/
/* Note: Heavily borrowed from https://www.geeksforgeeks.org/lru-cache-implementation ******/
/*******************************************************************************************/

class LRUCache_full
{
    // store keys of cache
    //This is storing the tags as strings because STLport doesnt support
    //unordered map with unsigned long long data type
    list < string > dq;

    // store references of key in cache
    unordered_map < string, list < string >::iterator > ma;

    unsigned long long csize; //maximum capacity of cache (number of lines)

    unsigned long long int hit;
    unsigned long long int miss;

public:
    LRUCache_full(unsigned long long);
    void refer( string );
    void display(FILE *trace);
};

//Initializes the cache model to 'n' number of cache lines (64 bytes each)
LRUCache_full::LRUCache_full(unsigned long long n)
{
    csize = n;
    hit = 0;
    miss = 0;
}

/* Refers key x with in the LRU cache */
void LRUCache_full::refer(string x)
{
    // not present in cache
    if (ma.find(x) == ma.end())
    {
        miss++;
        // cache is full
        if (dq.size() == csize)
        {
            //delete least recently used element
            string last = dq.back();
            dq.pop_back();
            ma.erase(last);
        }
    }

    // present in cache
    else
    {
        //else add it to the cache as next line, and create a hash map entry
        hit++;
        dq.erase(ma[x]);
    }

    // update reference
    dq.push_front(x);
    ma[x] = dq.begin();
}

// display contents of cache, ie. print to the output file
void LRUCache_full::display(FILE *trace)
{

    fprintf(trace,"LRU Fully ");
    fprintf(trace,"%llu B ",csize*64);
    fprintf(trace,"H: %llu \tM: %llu ",hit,miss);
    fprintf(trace,"\tHR: %f",(double)100*hit/(hit+miss));
    fprintf(trace,"\tMR: %f \n",(double)100*miss/(hit+miss));
}
