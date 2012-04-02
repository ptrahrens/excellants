/****************************************
 * Setup.cpp                            *
 * Peter Ahrens                         *
 * Sets up the ACO                      *
 ****************************************/

#include "RankBasedAntSystem.h"
#include "TSPReader.h"
#include "Comm.h"
#include "Writer.h"
#include <iostream>
#include <unistd.h>
#include <string>
#include <cctype>
#include <ctime>
using namespace std;

//Setup: The main control loop to the whole program.
int main(int argc, char* argv[]){
  //declare variables
  cout << "|SETUP|\n";
  string antHillType = "RBAS";
  int m = -1;
  int ranks = 6;
  int maxTime = 0;
  int maxIter = 0;
  int maxReps = 0;
  int reps = 0;
  bool stopping = false;
  bool graphics = false;
  char* filen;
  Writer O; //writes output to stdout and an optional file
  //Read initially neccesary command-line arguments.
  for(int i = 0; i < argc;i++){
    if (string(argv[i]) == "-ras"){
      antHillType = "RBAS";
      if(i + 1 < argc){
	if(string(argv[i])[0] != '-'){
	  ranks = atoi(argv[i+1]);
	}
      }
    }
    if (string(argv[i]) == "-tsp"){
      filen = argv[i+1];
    }
    if (string(argv[i]) == "-gui"){
      graphics = true;
    }
    if (string(argv[i]) == "-m"){
      m = atoi(argv[i+1]);
    }
    if (string(argv[i]) == "-out"){
      if(!O.setFile(argv[i+1])){
	cout << "Unable to open output file\n";
      }
    }
    if (string(argv[i]) == "-maxTime"){
      maxTime = atoi(argv[i+1]);
    }
    if (string(argv[i]) == "-maxIter"){
      maxIter = atoi(argv[i+1]);
    }
    if (string(argv[i]) == "-maxReps"){
      maxReps = atoi(argv[i+1]);
    }
  }
  cout << ">" << flush; //----Checkpoint 1
  if(graphics){
    //If graphics are running, create pipes and processes.
    int parentPipe[] = {-1,-1};  // parent -> child 
    int childPipe[] = {-1,-1};  // child -> parent 
    if ( pipe(parentPipe) < 0  ||  pipe(childPipe) < 0 )
      {
	std::cout << "Failed to create pipe";
	return 1;
      }
#define	PARENT_READ   childPipe[0]
#define	CHILD_WRITE   childPipe[1]
#define CHILD_READ    parentPipe[0]
#define PARENT_WRITE  parentPipe[1]
    pid_t pID = fork();
    if(pID == 0){//child
      close(PARENT_WRITE);
      close(PARENT_READ);
      //Graphics run in a separate process from the computations so neither is slowed down.
      Comm C(CHILD_READ,CHILD_WRITE);
      cout << ">" << flush; //----Checkpoint 3
      if(!C.send(string("Test"))){
	cout << "\n Interprocess Comm Failed";
	return 1;
      }
    }else if(pID < 0){//fail
      cout << "Failed to fork";
      return 1;
    }else{//parent
      close(CHILD_READ);
      close(CHILD_WRITE);
      //Computations run in a separate process from the graphics so neither is slowed down.
      Comm C(PARENT_READ,PARENT_WRITE);
      cout << ">" << flush;//----Checkpoint 2
      TSPReader t;
      t.read(filen);
      cout << ">" << flush;//----Checkpoint 4
      if(m == -1){
	m = t.getNumNodes();
      }
      RankBasedAntSystem antHill(t.getDistances(),t.getNumNodes(),m);
      cout << ">" << flush;//----Checkpoint 5
      //If any parameters need to be changed, they are modified from their defaults here.
      for(int i = 0; i < argc;i++){
	if (string(argv[i]) == "-b"){
	  antHill.setBeta(atof(argv[i+1]));
	}
	if (string(argv[i]) == "-r"){
	  antHill.setRho(atof(argv[i+1]));
	}
      }
      cout << ">" << flush;//----Checkpoint 6
      antHill.initialize();
      if(C.recieve() == "Test"){ //A check to find out if the comm is working.
	cout << ">\n" << flush;//----Checkpoint 7
      }else{
	cout << "\n Interprocess Comm Failed";
	return 1;
      }
      O.writeHeader(antHill.getBeta(),antHill.getRho(),antHill.getNumAnts(),antHillType,t.getName());
      clock_t t1, t2, t3;
      t1 = t2 = t3 = clock();
      //Main control sequence.
      for(int i = 0; !stopping; i++){
	t2 = t3;
	antHill.forage();
	t3 = clock();
	O.write(i, antHill.getIterBestDist(), antHill.getGlobBestDist(), (double)(t3 - t1) / CLOCKS_PER_SEC, (double)(t3 - t2) / CLOCKS_PER_SEC);
	if(maxTime != 0){
	  if((int)((double)(t3 - t1) / CLOCKS_PER_SEC)> maxTime){
	    stopping = true;
	  }
	}
	if(maxIter != 0){
	  if(i >= maxIter){
	    stopping = true;
	  }
	}
	if(maxReps != 0){
	  if(antHill.getReps() >= maxReps){
	    stopping = true;
	  }
	}
      }
    }
  }else{
    cout << ">" << flush;//----Checkpoint 2
    TSPReader t;
    t.read(filen);
    cout << ">" << flush;//----Checkpoint 3
    if(m == -1){
      m = t.getNumNodes();
    }
    RankBasedAntSystem antHill(t.getDistances(),t.getNumNodes(),m);
    //If any parameters need to be changed, they are modified from their defaults here.
    cout << ">" << flush;//----Checkpoint 4
    for(int i = 0; i < argc;i++){
      if (string(argv[i]) == "-b"){
	antHill.setBeta(atof(argv[i+1]));
      }
      if (string(argv[i]) == "-r"){
	antHill.setRho(atof(argv[i+1]));
      }
    }
    cout << ">" << flush;//----Checkpoint 5
    antHill.initialize();
    cout << ">>\n" << flush;//----Checkpoint 6/7
    O.writeHeader(antHill.getBeta(),antHill.getRho(),antHill.getNumAnts(),antHillType,t.getName());
    clock_t t1, t2, t3;
    t1 = t2 = t3 = clock();
    //Main control sequence.
    for(int i = 0; !stopping; i++){
      t2 = t3;
      antHill.forage();
      t3 = clock();
      O.write(i, antHill.getIterBestDist(), antHill.getGlobBestDist(), (double)(t3 - t1) / CLOCKS_PER_SEC, (double)(t3 - t2) / CLOCKS_PER_SEC);
      if(maxTime != 0){
	if((double)(t3 - t1) / CLOCKS_PER_SEC > maxTime){
	  stopping = true;
	}
      }
      if(maxIter != 0){
	if(i >= maxIter){
	  stopping = true;
	}
      }
      if(maxReps != 0){
	if(antHill.getReps() >= maxReps){
	  stopping = true;
	}
      }
    }
  }		
}

//Copyright (c) 2012, Peter Ahrens
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//
//    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//    Neither the name of Excellants nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
