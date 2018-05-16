/********************************************************************************
Objective:
This is for converting the address * addr to a string of the
hex representation of the address to be used in the LRU and random
fully associative caches
This is becasue STLport doesnt support unsigned long long int in
unordered maps

Author: Meghana Gaopande

Usage:
string tag_conversion(VOID * addr)

**********************************************************************************/

#include <stdio.h>
#include <string>


string convert[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

/********************************************************************************************/
/* Tag Conversion****************************************************************************/
/*******************************************************************************************/

inline string tag_conversion(VOID * addr)
{
  unsigned long long p, tag;
  string tag_str;
  int index;
  string temp[12];

  VOID *q;
  q = addr;
  p = (unsigned long long)q;
  tag = p/64;

  //Convert the tag to a string of its hex representation
  for(int i=11; i>-1; i--)
  {
    index = tag % 16;
    tag = tag/16;
    temp[i] = convert[index];
  }
  //store the hex representation in string tag_str
  for(int i = 0; i< 12; i++)
  {
    tag_str += temp[i];
  }

  return tag_str;
}
