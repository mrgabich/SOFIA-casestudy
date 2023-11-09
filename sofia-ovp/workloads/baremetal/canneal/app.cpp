// main.cpp
//
// Created by Daniel Schwartz-Narbonne on 13/04/07.
// Modified by Christian Bienia
//
// Copyright 2007-2008 Princeton University
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "annealer_types.h"
#include "annealer_thread.h"
#include "netlist.h"
#include "rng.h"
#include "FIM.h"

using namespace std;
int num_threads = 0;
int swaps_per_temp = 0;
int start_temp = 0;
int number_temp_steps = -1;

void* entry_pt(void*);

int main (int argc, char * const argv[]) {

	srandom(3);
	//argument 1 is numthreads
	num_threads = NUMTHREADS;
	//argument 2 is the num moves / temp
	swaps_per_temp = SWAPSPERTEMPERATURE;
	//argument 3 is the start temp
	start_temp =  STARTTEMPERATURE;
	
	//argument 4 is the netlist filename
	string filename(INPUTFILE);
	//argument 5 (optional) is the number of temperature steps before termination
	number_temp_steps = MINTEMPERATURESTEPS;
    FIM_Instantiate();

	//now that we've read in the commandline, run the program
	netlist my_netlist(filename);
	

	annealer_thread a_thread(&my_netlist,num_threads,swaps_per_temp,start_temp,number_temp_steps);


	a_thread.Run();

    FIM_exit();
	return 0;
}

void* entry_pt(void* data)
{
	annealer_thread* ptr = static_cast<annealer_thread*>(data);
	ptr->Run();
}
