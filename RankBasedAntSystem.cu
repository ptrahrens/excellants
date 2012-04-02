/****************************************
 * RankBasedAntSystem.cu                *
 * Peter Ahrens                         *
 * Performs specific RBAS procedures    *
 ****************************************/

#include "RankBasedAntSystem.h"

//constructor: Allocates memory and sets defaults.
RankBasedAntSystem::RankBasedAntSystem(thrust::host_vector<float> newDistances, int newNumCities, int newNumAnts)
  : Colony(newDistances, newNumCities, newNumAnts)
{
  w = 6;//default
  RBASWeight = thrust::device_vector<float>(numAnts);
  RBASMap = thrust::device_vector<int>(numAnts*numCities);
}

//initialize: Runs the Colony initialize, then creates additional maps and keys.
void RankBasedAntSystem::initialize()
{
  Colony::initialize();
  thrust::fill(RBASWeight.begin(),RBASWeight.end(),0);
  thrust::copy_n(thrust::make_reverse_iterator(thrust::make_counting_iterator(w)),
		 w,
		 RBASWeight.begin());
  thrust::fill(ACInt.begin(),
	       ACInt.end(),
	       0);
  ACInt[numCities] = 1;
  thrust::inclusive_scan(ACInt.begin(),
			 ACInt.end(),
			 ACInt.begin());
  thrust::exclusive_scan_by_key(ACInt.begin(),
				ACInt.end(),
				thrust::make_constant_iterator(1,0),
				RBASMap.begin());
}

//computeParameters: Simply computes neccesary parameters.
void RankBasedAntSystem::computeParameters()
{
  if (numCities < w){
    w = numCities;
  }
  computeInitialPheromone();
}

//computeInitialPheromone: Computes the initial pheromone level with the formula described by Marco Dorigo.
void RankBasedAntSystem::computeInitialPheromone()
{
  initialPheromone = 0.5*w*(w-1)/(rho * Colony::greedyDistance());
}

//updataPheromones: Evaporates, then the ants lay pheromone at levels corresponding to their rank, judged by the distances of their tours.
void RankBasedAntSystem::updatePheromones()
{
  //evaporate
  thrust::transform(pheromones.begin(),
		    pheromones.end(),
		    thrust::make_constant_iterator(1.0f-rho),
		    pheromones.begin(),
		    thrust::multiplies<float>());
  //determine ant pheromone levels
  thrust::stable_sort_by_key(thrust::make_permutation_iterator(antDistances.begin(),ACKey.begin()),
			     thrust::make_permutation_iterator(antDistances.end(),ACKey.end()),
			     antTours.begin());
  thrust::transform(RBASWeight.begin(),
		    RBASWeight.end(),
		    antDistances.begin(),
		    AFloat.begin(),
		    thrust::divides<float>());
  //AFloat[w-1] = w/globBestDist;
  AFloat[w-1] = w/iterBestDist;//for a simple rankbased, without global pheromone
  ACInt.assign(antTours.begin(),antTours.end());
  //thrust::scatter(globBestTour.begin(),globBestTour.end(),thrust::make_counting_iterator(numCities*(w-1)),ACInt.begin()); 
  thrust::scatter(iterBestTour.begin(),
		  iterBestTour.end(),
		  thrust::make_counting_iterator(numCities*(w-1)),
		  ACInt.begin()); //for a simple rankbased, without global pheromone
  thrust::transform(ACInt.begin(),
		    ACInt.end(),
		    thrust::make_permutation_iterator(ACInt.begin(),distMap.begin()), 
		    ACInt2.begin(),
		    saxpy_functor(numCities));
  //lay Pheromone 
  for(int i = 0; i < numCities*w; i += numCities){
    thrust::transform(thrust::make_permutation_iterator(pheromones.begin(),ACInt2.begin() + i),
		      thrust::make_permutation_iterator(pheromones.end(),ACInt2.begin() + i + numCities), 
		      thrust::make_permutation_iterator(AFloat.begin(),ACKey.begin() + i),
		      thrust::make_permutation_iterator(pheromones.begin(),ACInt2.begin() + i),thrust::plus<float>());
  }
}

//forage: Runs Colony forage.
void RankBasedAntSystem::forage()
{
  Colony::forage();
}

void RankBasedAntSystem::setRho(float newRho)
{
  Colony::setRho(newRho);
}

void RankBasedAntSystem::setBeta(float newBeta)
{
  Colony::setBeta(newBeta);
}

void RankBasedAntSystem::setW(int newW)
{
  w = newW;
}

int RankBasedAntSystem::getW()
{
  return w;
}

double RankBasedAntSystem::getRho()
{
  return Colony::getRho();
}

double RankBasedAntSystem::getBeta()
{
  return Colony::getBeta();
}

int RankBasedAntSystem::getNumAnts()
{
  return Colony::getNumAnts();
}

double RankBasedAntSystem::getIterBestDist()
{
  return Colony::getIterBestDist();
}

double RankBasedAntSystem::getGlobBestDist()
{
  return Colony::getGlobBestDist();
}

int RankBasedAntSystem::getReps()
{
  return Colony::getReps();
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
