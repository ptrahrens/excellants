/****************************************
 * RankBasedAntSystem.h                 *
 * Peter Ahrens                         *
 * Performs specific RBAS procedures    *
 ****************************************/

#ifndef RANKBASEDANTSYSTEM_H
#define RANKBASEDANTSYSTEM_H
#include "Colony.h"

//RankBasedAntSystem: Provides the neccesary extensions to Colony to create a Rank-Based Ant System
class RankBasedAntSystem : Colony{
 public:
  RankBasedAntSystem(thrust::host_vector<float> newDistances, int newNumCities, int newNumAnts); // Allocates memory and sets defaults.
  void initialize(); // Runs the Colony initialize, then creates additional maps and keys.
  void computeParameters(); // Simply computes neccesary parameters.
  void forage(); // Runs Colony forage.
  void setRho(float newRho);
  void setBeta(float newBeta);
  void setW(int newW);
  int getW();
  double getRho();
  double getBeta();
  int getNumAnts();
  double getIterBestDist();
  double getGlobBestDist();
  int getReps();
  std::string getTour();
 private:
  void computeInitialPheromone(); // Computes the initial pheromone level with the formula described by Marco Dorigo.
  void updatePheromones(); // Evaporates, then the ants lay pheromone at levels corresponding to their rank, judged by the distances of their tours.
  int w;
  thrust::device_vector<float> RBASWeight;
  thrust::device_vector<int> RBASMap;
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
