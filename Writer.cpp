/****************************************
 * Writer.cpp                           *
 * Dustin Tauxe and Peter Ahrens        *
 * Writes output to file and stdout     *
 ****************************************/

#include "Writer.h"

//Constructor: Sets defaults.
Writer::Writer()
{
  writing = false;
}

//Constructor: Sets defaults and opens the given file.
Writer::Writer(char* filen)
{
  setFile(filen);
}

Writer::~Writer()//Destructor.
{
  delete[] fileName;
}

//setFile: Tries to open given file. If it does, it is changed to writing mode.
bool Writer::setFile(char* filen)
{
  fileName = filen;
  writing = true;
  f.open(filen,ios_base::app);
  if (!f){
    writing = false;
  }
  return writing;
}

//writeHeader: Writes a header to the file and (if in writing mode) to the file.
void Writer::writeHeader(float beta, float rho, int numAnts, string ACO, string TSPName)
{
  time_t rawtime;
  time ( &rawtime );
  if(writing){
    f << "\n" << "Date: " << ctime (&rawtime) <<
      "TSP: " << TSPName << "\n" <<
      "ACO: " << ACO << "\n" <<
      "numAnts: " << numAnts << "\n" << 
      "Alpha: 1 " << "Beta: " << beta << " Rho: " << rho << "\n" <<
      "Iteration, Iteration_Best, Global_Best, Time, Iteration_Time\n" << flush;
  }
  cout << "\n" << "Date: " << ctime (&rawtime) <<
    "TSP: " << TSPName << "\n" <<
    "ACO: " << ACO << "\n" <<
    "numAnts: " << numAnts << "\n" << 
    "Alpha: 1 " << "Beta: " << beta << " Rho: " << rho << "\n" <<
    std::left << setw(10) << "Iteration"<< setw(10) << "Iter_Best" << setw(10) << "Glob_Best" << setw(10) << "Time" << setw(10) << "Iter_Time" << "\n";
}

//write: Writes a standard line of output to stdout and (if in writing mode) to the file.
void Writer::write(int iter, double iterBest, double globBest, double time, double iterTime)
{
  if(writing){
    f << iter << "," << iterBest << "," << globBest << "," << time << "," << iterTime << "\n" << flush;
  }
  cout << std::left << setw(10) << iter << setw(10) << iterBest << setw(10) << globBest << setw(10) << time << setw(10) << iterTime << "\n";
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
