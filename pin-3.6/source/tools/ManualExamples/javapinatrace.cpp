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
#include <inttypes.h>
#include <stdint.h>
#include "pin.H"

#include "cache/cpu_cache.h"

#define FILE_LIMIT 31250000

FILE * trace;

char fileName[500];
static long long unsigned fileNum = 0;
static long long unsigned lineCount = 0;

//static long long unsigned total_misses = 0;

//static long long unsigned instcount = 0;

// cache_miss = 1 for hit, 0 for miss with no evictions (cold misses)
// cache_miss = 2 for non-dirty evictions, cache_miss = 3 for dirty evictions

VOID RecordMemRead(VOID * ip, VOID * addr)
{
  //instcount++;
  
  addr_t fetch_addr = (unsigned long long)addr / BLOCK_SIZE;
  addr_t evict_addr;
  int cache_miss = CacheCall(READ_OP, fetch_addr, &evict_addr);

  if(cache_miss != 1) {
    //total_misses++;
    lineCount++;
     if(lineCount >= FILE_LIMIT)
      {
	lineCount = 0;
	++fileNum;
	snprintf(fileName, sizeof(fileName), "%s%llu.out", "trace_", fileNum);
	trace = fopen(fileName, "w");
      }
    fprintf(trace, "R 0x%016" PRIxPTR " %015llx %d\n", (uintptr_t)addr, evict_addr, cache_miss);
  } 
    
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr)
{
  //instcount++;
  addr_t fetch_addr = (unsigned long long)addr / BLOCK_SIZE;
  addr_t evict_addr;
  int cache_miss = CacheCall(WRITE_OP, fetch_addr, &evict_addr);

  if(cache_miss != 1) {
    //total_misses++;
    lineCount++;
    if(lineCount >= FILE_LIMIT)
      {
	lineCount = 0;
	++fileNum;
	snprintf(fileName, sizeof(fileName), "%s%llu.out", "trace_", fileNum);
	trace = fopen(fileName, "w");
      }
    fprintf(trace, "W 0x%016" PRIxPTR " %015llx %d\n", (uintptr_t)addr, evict_addr, cache_miss);
  }

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
  //printf("Instcount = %llu", instcount);
  fprintf(trace, "#eof\n");
  fclose(trace);
   
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
  
  if(PIN_Init(argc, argv))
    return Usage();

  // Initialization for cache
  InitMask();
  InitSet_Usage();
      
  //trace = fopen("trace.out", "w");

  snprintf(fileName, sizeof(fileName), "%s%llu.out", "trace_", fileNum);

  trace = fopen(fileName, "w");

  INS_AddInstrumentFunction(Instruction, 0);
  PIN_AddFiniFunction(Fini, 0);
  // Never returns
  PIN_StartProgram();
    
  return 0;
}
