/****************************************
 * Comm.h                               *
 * Peter Ahrens                         *
 * Communicates over a pipe             *
 ****************************************/

#ifndef COMM_H
#define COMM_H
#include <iostream>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <cctype>
using namespace std;

//Comm: A class used to simplify the data transfer over a pipe.
class Comm
{
 public:
  Comm(int read, int write); //constructor: Takes as arguments the neccesary pipes to work with.
  ~Comm();
  string recieve(); //recieve: Looks for data on the pipe. If there is some, it is returned. If not, an empty string is returned.
  bool send(string message); //send: Puts the given data on the pipe
 private:
  int tagLength;
  int readPipe;
  int writePipe;
  string intToString(int t, int padding); //intToString: Converts an int to a specified size string with 0s as padding.
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
