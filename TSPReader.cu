/****************************************
 * TSPReader.cu                         *
 * Dustin Tauxe and Peter Ahrens        *
 * Reads .tsp files                     *
 ****************************************/

#include "TSPReader.h"

//destructor
TSPReader::~TSPReader()
{	
  delete[] cityNames;
  delete[] Xcoords;
  delete[] Ycoords;
}

//read: Reads a given tsp file and extracts data.
bool TSPReader::read(char* filen)
{	
  ifstream infile(filen, ios_base::in);
  if(!infile){
    cout << "\n" << "Unable to open file: " << filen << "\n";
    return false;
  }
  string line;
  string tag;
  string value;
  while(infile.good()){
    getline(infile,line);
    if(line.length() > 1){
      if(!isprint(line[line.length()-1])) line.erase(line.length()-1,1);
      if(line.find(":") != string::npos){
	tag = line.substr(0,line.find(":"));
	value = line.substr(line.find(":") + 1, line.length() - line.find(":") - 1);
      }else{
	tag = line;
	value = "";
      }
      while(tag.find(" ") != string::npos){
	tag.replace(tag.find(" "),1,"");
      }
      while(value.find(" ") != string::npos){
	value.replace(value.find(" "),1,"");
      }
      if(tag == "NAME"){
	name = value;
      }else if(tag == "TYPE"){
	if(value != "TSP" && value != "STSP"){
	  cout << "\n" << "Invalid problem type: " << value << "\n";
	  return false;
	}
      }else if(tag == "DIMENSION"){
	numCities = atoi(value.c_str());
      }else if(tag == "EDGE_WEIGHT_TYPE"){
	if(value != "EUC_2D"){
	  cout << "\n" << "Invalid edge weight type: " << value << "\n";
	  return false;
	}
      }else if(tag == "NODE_COORD_SECTION"){
	//Set coord arrays to appropriate lengths
	cityNames = new string [numCities];
	Xcoords = new float [numCities];
	Ycoords = new float [numCities];
	for(int i = 0; infile.good() && i < numCities; i++){
	  getline(infile,line);
	  if(!isprint(line[line.length()-1])) line.erase(line.length()-1,1);
	  if(line == "EOF"){
	    return false;
	  }
	  cityNames[i] = line.substr(0,line.find(" "));
	  Xcoords[i] = atof(line.substr(line.find(" ") + 1, line.find_last_of(" ") - line.find(" ") - 1).c_str());
	  Ycoords[i] = atof(line.substr(line.find_last_of(" ") + 1, line.length() - line.find_last_of(" ") - 1).c_str());
	}
      }
						 
    }
  }
  calculateDistances();
  return true;
}

//calculateDistances: Calculates distances on the CPU.
void TSPReader::calculateDistances(){
  distances = thrust::host_vector<float> (numCities*numCities);
  float k;
  for(int i = 0; i < numCities; i++){
    for(int j = 0; j < numCities; j++){
      k = sqrt(pow(Xcoords[i]-Xcoords[j],2)+pow(Ycoords[i]-Ycoords[j],2));
      if(i == j)
	k = std::numeric_limits<float>::max();
      distances[i * numCities + j] = k;
    }
  }
}
	
string TSPReader::getName()
{ 
  return name; 
}
	
float* TSPReader::getXcoords()
{ 
  return Xcoords; 
}
	
float* TSPReader::getYcoords()
{ 
  return Ycoords; 
}
	
int TSPReader::getNumNodes()
{ 
  return numCities;
}
	
thrust::host_vector<float> TSPReader::getDistances()
{ 
  return distances; 
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
