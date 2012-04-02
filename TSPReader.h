/****************************************
 * TSPReader.h                          *
 * Dustin Tauxe and Peter Ahrens        *
 * Reads .tsp files                     *
 ****************************************/

#ifndef TSPREADER_H
#define TSPREADER_H
#include <iostream>	// Used for command line I/O
#include <fstream>	// Used for file Input
#include <string>
#include <math.h>
#include <cctype>
#include <float.h> // Used to find maximum float
#include <thrust/host_vector.h>
using namespace std;

//TSPReader: Used to read .tsp files.
class TSPReader
{	
  string name;			// TSP name - max length 16
  int numCities;		// Number of cities (dimension)
  float* Xcoords;		// X coords
  float* Ycoords;		// Y coords
  string* cityNames;
  thrust::host_vector<float> distances;	// Distances between cities
	
 public:
  //Constructors/Destructors
  TSPReader(){}
  ~TSPReader();
	

  bool read(char* filen);	// Reads a given tsp file and extracts data.
  string getName();
  float* getXcoords();
  float* getYcoords();
  int getNumNodes();
  thrust::host_vector<float> getDistances();
 private:
  void calculateDistances(); // Calculates distances on the CPU.
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
