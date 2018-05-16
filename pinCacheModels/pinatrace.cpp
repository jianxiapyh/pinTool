/*BEGIN_LEGAL
Intel Open Source License

Copyright (c) 2002-2017 Intel Corporation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
/*
 *  This file contains an ISA-portable PIN tool for tracing memory accesses.
 */

#include <stdio.h>
#include "pin.H"
#include <time.h>

//Student Defined Libraries

#include "cache_set.h"
#include "cache_Dmap.h"
#include "lru_2mb.h"
#include "cache_lru_fully.h"
#include "cache_random_fully.h"
#include "fully_tag_conversion.h"

#define CACHE_SIZE 15584512 

//cg.D.x mem size: 8971980 kB
//1/4  = 2296826880 B
//1/8  = 1148413440 B
//1/16 =  574206720 B
//1/32 =  287103360 B


//mg.c.x Mem size: 3491876 kB
// 1/2   = 1787840512 B
// 1/4   =  893920256 B
// 1/8   =  446960128 B
// 1/16  =  223480064 B
// 1/32  =  111740032 B

//cg.C.x mem size: 487016 kB
//1/4  = 124676096 B
//1/8  =  62338048 B
//1/16 =  31169024 B
//1/32 =  15584512 B

//cache Objects
//set-assoc
nWays cache_rand(CACHE_SIZE,4,RAN_POLICY);
nWays cache_lru(CACHE_SIZE,4,LRU_POLICY);
nWays cache_tourn(CACHE_SIZE,4,TOURN_POLICY);
//Direct mapped
dmap cache_D(CACHE_SIZE);

RandomCache_full cache_random_fully(CACHE_SIZE/64);
LRUCache_full cache_lru_fully(CACHE_SIZE/64);
//LRUCache_full cache_lru_fully(CACHE_SIZE);

unsigned long long ins_count = 0, numm = 0;;
time_t start,end;
inline void ramCacheWrapper(VOID * addr)
{/*	This function passes the addre that incurred an LLC miss to all
	the DRAM cache models. It also modifies the address trace to a
	string format for the fully associative models.
	*/
  unsigned long long p,tag;
   string tag_converted;
   VOID *q;
   q = addr;
   p = (unsigned long long)q;
   tag = p/64;
   cache_D.refer(tag);
   cache_rand.refer_random(tag);
   cache_lru.refer_LRU(tag);
   cache_tourn.tournament(tag);
   tag_converted = tag_conversion(addr);
   cache_random_fully.refer(tag_converted);
   cache_lru_fully.refer(tag_converted);
   q = nullptr;
}

FILE * trace;


//	cg.C Inscount threshold : 779225600
//	mg.C Inscount threshold	: 5587001600
// 	cg.D Inscount Threshold : 1435516800

// Print a memory read record
 
VOID RecordMemRead(VOID * ip, VOID * addr)
{
  double seconds = 0;
   int miss = wrapper(addr);
   if(miss){
       ins_count++;
       ramCacheWrapper(addr);
	   //check if threshold reached.
       if(ins_count == 1435516800){
			time(&end);
			seconds = difftime(end,start);
			std::cout<<"Required cache misses observed after"<<seconds<<" s\n"; //<<miss ;
			std::cout<<"Terminating Program";
			//additional instructions if periodic writes needed:
			//cache_D.write_to(trace);
			//cache_rand.write_to(trace);
			//cache_lru.write_to(trace);
			//cache_tourn.write_to(trace);
			//cache_random_fully.display(trace);
			//cache_lru_fully.display(trace);
			//fclose(trace);
			//ins_count = 0;
			//time(&start);
			exit(0);
	 }
   }
   //fprintf(trace,"%p: R %p\n", ip, addr);
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr)
{
  double seconds = 0;
  int miss = wrapper(addr);
  if(miss){
      ins_count++;
      ramCacheWrapper(addr);
    //check if threshold reached.
	if(ins_count == 1435516800){
      time(&end);
      seconds = difftime(end,start);
      std::cout<<"Required cache misses observed after"<<seconds<<" s\n"; //<<miss ;
      std::cout<<"Terminating Program";
      //cache_D.write_to(trace);
      //cache_rand.write_to(trace);
      //cache_lru.write_to(trace);
      //cache_tourn.write_to(trace);
      //cache_random_fully.display(trace);
      //cache_lru_fully.display(trace);
      //fclose(trace);
      //ins_count = 0;
      //time(&start);
      exit(0);
      }
  }//fprintf(trace,"%p: W %p\n", ip, addr);
  
}

// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP
    // prefixed instructions appear as predicated instructions in Pin.
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
                IARG_END);
        }
        // Note that in some architectures a single memory operand can be
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
                IARG_END);
        }
    }
}

VOID Fini(INT32 code, VOID *v)
{
   /*Modified to write computed results for each model to a file,
	when target program is terminated.*/
  cache_D.write_to(trace);
  cache_rand.write_to(trace);
  cache_lru.write_to(trace);
  cache_tourn.write_to(trace);
  cache_random_fully.display(trace);
  cache_lru_fully.display(trace);
  fclose(trace);
	//fprintf(trace, "#eof\n");
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR( "This Pintool prints a trace of memory addresses\n"
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    if (PIN_Init(argc, argv)) return Usage();

    trace = fopen("speed_test.out", "a");
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);
    time(&start);
    // Never returns
    PIN_StartProgram();

    return 0;
}
