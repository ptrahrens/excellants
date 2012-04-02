/****************************************
 * Comm.cpp                             *
 * Peter Ahrens                         *
 * Communicates over a pipe             *
 ****************************************/

#include "Comm.h"

//constructor: Takes as arguments the neccesary pipes to work with.
Comm::Comm(int newReadPipe,int newWritePipe)
{
  readPipe = newReadPipe;
  writePipe = newWritePipe;
  tagLength = 8;
}

Comm::~Comm() //Destructor.
{}

//recieve: Looks for data on the pipe. If there is some, it is returned. If not, an empty string is returned.
string Comm::recieve()
{
  char tag[tagLength];
  int rv = read(readPipe,tag,tagLength);
  if(rv < 0){
    cout << "Read Error 1";
    exit(1);
  }
  if(rv == 0){
    return string("");
  }
  string inputTag = tag;
  int toRead = atoi(inputTag.substr(0,tagLength).c_str());
  char charIn[toRead];
  if(read(readPipe,charIn,toRead) < 0){
    cout << "Read Error 2";
    exit(1);
  }
  string output = charIn;
  output = output.substr(0,toRead);
  return output;
}

//send: Puts the given data on the pipe
bool Comm::send(string message)
{
  message = intToString(message.length(),tagLength) + message;
  return(write(writePipe,message.data(),message.length()) > 0);
}

//intToString: Converts an int to a specified size string with 0s as padding.
string Comm::intToString(int t, int padding)
{
  std::ostringstream oss;
  oss << setfill('0') << setw(padding) << t;
  return oss.str();
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
