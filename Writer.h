/****************************************
 * Writer.h                             *
 * Dustin Tauxe and Peter Ahrens        *
 * Writes output to file and stdout     *
 ****************************************/

#ifndef WRITER_H
#define WRITER_H
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <time.h>
using namespace std;

class Writer
{ 
 public:
  Writer(); // Sets defaults.
  Writer(char* filen); // Sets defaults and opens the given file.
  ~Writer();
  bool setFile(char* filen); // Tries to open given file. If it does, it is changed to writing mode.
  void writeHeader(float beta, float rho, int numAnts,string ACO, string TSPName); // Writes a header to the file and (if in writing mode) to the file.
  void write(int iter, double iterBest, double globBest, double time, double iterTime); // Writes a standard line of output to stdout and (if in writing mode) to the file.
 private:
  char* fileName;
  ofstream f; // this is the file
  char* temp; // scratch
  bool writing;
};

#endif

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
