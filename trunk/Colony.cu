/****************************************
 * Colony.cu                            *
 * Peter Ahrens                         *
 * Main ACO procedures                  *
 ****************************************/

#include "Colony.h"

//Constructor: Sets defaults and allocates memory.
Colony::Colony(thrust::host_vector<float> newDistances, int newNumCities, int newNumAnts)
{
  //defaults
  beta = 2;
  rho = 0.1;
  //world vars
  reps = 0;
  distances.assign(newDistances.begin(),newDistances.end());
  numCities = newNumCities;
  probabilities = thrust::device_vector<float>(numCities*numCities);
  pheromones = thrust::device_vector<float>(numCities*numCities);
  //ant vars
  numAnts = newNumAnts;
  antDistances = thrust::device_vector<float>(numAnts);
  antVisits = thrust::device_vector<float>(numCities*numAnts);
  toVisit = thrust::device_vector<int>(numCities*numAnts);
  antTours = thrust::device_vector<int>(numCities*numAnts);
  iterBestDist = std::numeric_limits<float>::max() - 1;
  globBestDist = std::numeric_limits<float>::max();
  iterBestTour = thrust::device_vector<int>(numCities);
  globBestTour = thrust::device_vector<int>(numCities);
  //maps and keys
  ACMapF = thrust::device_vector<int>(numAnts);
  ACMapL = thrust::device_vector<int>(numAnts);
  tourMap = thrust::device_vector<int>(numAnts);
  distMap = thrust::device_vector<int>(numCities*numAnts);
  ACKey = thrust::device_vector<int>(numAnts*numCities);
  ARepeatCMap = thrust::device_vector<int>(numAnts*numCities);
  CCKey = thrust::device_vector<int>(numCities*numCities);
  //scratch variables
  AFloat = thrust::device_vector<float>(numAnts);
  CInt = thrust::device_vector<float>(numCities);
  AInt = thrust::device_vector<int>(numAnts);
  ACInt = thrust::device_vector<int>(numAnts*numCities);
  ACInt2 = thrust::device_vector<int>(numAnts*numCities);
  ACInt3 = thrust::device_vector<int>(numAnts*numCities);
  ACFloat = thrust::device_vector<float>(numAnts*numCities);
  CCFloat = thrust::device_vector<float>(numCities*numCities);
  AUnsignedInt = thrust::device_vector<unsigned int>(numAnts);
  //Random numbers
  ARandom = thrust::device_vector<unsigned int>(numAnts);
  ACRandom = thrust::device_vector<unsigned int>(numAnts*numCities);
}

//initialize: Initializes data, creates maps and keys, performs standard ACO initialization steps etc. 
void Colony::initialize()
{
  //seed the random numbers
  thrust::transform(thrust::make_counting_iterator(0),
		    thrust::make_counting_iterator(numAnts*numCities),
		    thrust::make_counting_iterator(time(NULL)),ACRandom.begin(),thrust::multiplies<int>());
  thrust::transform(thrust::make_counting_iterator(0),
		    thrust::make_counting_iterator(numAnts),
		    thrust::make_counting_iterator(time(NULL)),
		    ARandom.begin(),
		    thrust::multiplies<int>());
  //constant seeds
  //thrust::transform(thrust::make_counting_iterator(0),thrust::make_counting_iterator(numAnts*numCities),thrust::make_constant_iterator(1),ACRandom.begin(),thrust::multiplies<int>());
  //thrust::transform(thrust::make_counting_iterator(0),thrust::make_counting_iterator(numAnts),thrust::make_constant_iterator(1),ARandom.begin(),thrust::multiplies<int>());
  thrust::transform(ACRandom.begin(),
		    ACRandom.end(),
		    ACRandom.begin(),
		    randStep());
  thrust::transform(ARandom.begin(),
		    ARandom.end(),
		    ARandom.begin(),
		    randStep());
  //create maps and keys
  //CCKey
  thrust::sequence(ACInt.begin(),
		   ACInt.begin() + numCities, 
		   0,
		   numCities);
  thrust::scatter(thrust::make_constant_iterator(1,0),
		  thrust::make_constant_iterator(1,numCities),
		  ACInt.begin(),
		  CCKey.begin());
  thrust::inclusive_scan(CCKey.begin(),
			 CCKey.end(),
			 CCKey.begin());
  //ACMapF
  thrust::sequence(ACMapF.begin(),
		   ACMapF.end(),
		   0,
		   numCities);
  //ACMapL
  thrust::transform(ACMapF.begin(),
		    ACMapF.end(),
		    thrust::make_constant_iterator(numCities-1),
		    ACMapL.begin(), 
		    thrust::plus<int>());
  //ACKey
  thrust::scatter(thrust::make_constant_iterator(1,0),
		  thrust::make_constant_iterator(1,numAnts),
		  ACMapF.begin(),
		  ACKey.begin());
  thrust::inclusive_scan(ACKey.begin(),
			 ACKey.end(),
			 ACKey.begin());
  thrust::transform(ACKey.begin(), 
		    ACKey.end(), 
		    thrust::make_constant_iterator(-1), 
		    ACKey.begin(),
		    thrust::plus<int>());
  //distMap
  thrust::fill(distMap.begin(),
	       distMap.end(),
	       1);
  thrust::inclusive_scan_by_key(ACKey.begin(),
				ACKey.end(),
				distMap.begin(), 
				distMap.begin());
  thrust::scatter(thrust::make_constant_iterator(0,0),
		  thrust::make_constant_iterator(0,numAnts),
		  ACMapL.begin(),
		  distMap.begin());
  thrust::transform(ACKey.begin(),
		    ACKey.end(),
		    distMap.begin(),
		    distMap.begin(),
		    saxpy_functor(numCities));
  //ARepeatCMap
  thrust::exclusive_scan_by_key(ACKey.begin(),
				ACKey.end(),
				thrust::make_constant_iterator(1),
				ARepeatCMap.begin());
  //ACO Initialize
  computeParameters();
  thrust::fill(pheromones.begin(),
	       pheromones.end(),
	       initialPheromone);
  computeProbabilities();
}

//forage: Main ACO loop. Performs the solution constructruction step, then updates distances, pheromones, probabilities.
void Colony::forage()
{ 
  //initialize variables and select start cities
  toVisit.assign(ARepeatCMap.begin(),ARepeatCMap.end());
  ACInt2.assign(ACKey.begin(),ACKey.end());
  thrust::fill(antVisits.begin(),
	       antVisits.end(),
	       0);
  thrust::sequence(tourMap.begin(),
		   tourMap.end(),
		   0,
		   numCities);
  thrust::transform(ARandom.begin(), 
		    ARandom.end(),
		    ARandom.begin(),
		    randStep());
  thrust::transform(ARandom.begin(),
		    ARandom.end(),
		    thrust::make_constant_iterator(numCities), 
		    thrust::make_permutation_iterator(antTours.begin(),tourMap.begin()), 
		    thrust::modulus<unsigned int>());
  thrust::transform(ACMapF.begin(),
		    ACMapF.end(),
		    thrust::make_permutation_iterator(antTours.begin(),tourMap.begin()),
		    AInt.begin(),
		    thrust::plus<int>());
  for(int x = 1; x < numCities; x++)
    {
      //update antVisits
      thrust::scatter(thrust::make_constant_iterator(x,0),
		      thrust::make_constant_iterator(x,numAnts), 
		      AInt.begin(),
		      antVisits.begin());
      thrust::remove_if(thrust::make_zip_iterator(thrust::make_tuple(toVisit.begin(),
								     ACInt2.begin())), 
			thrust::make_zip_iterator(thrust::make_tuple(toVisit.begin() + ((numCities-x + 1) * numAnts),
								     ACInt2.begin()+ ((numCities-x + 1) * numAnts))),
			antVisits.begin(),isX(x));
      //get probabilities
      thrust::transform(thrust::make_permutation_iterator(thrust::make_permutation_iterator(antTours.begin(),tourMap.begin()),ACInt2.begin()),
			thrust::make_permutation_iterator(thrust::make_permutation_iterator(antTours.end(),tourMap.end()),ACInt2.begin()+ ((numCities-x) * numAnts)),
			toVisit.begin(),
			ACInt.begin(),
			saxpy_functor(numCities));
      //update tour map
      thrust::transform(tourMap.begin(),
			tourMap.end(),
			tourMap.begin(),
			unaryPlus(1));
      //update random numbers
      thrust::transform(ACRandom.begin(),
			ACRandom.begin() + ((numCities-x + 1) * numAnts), 
			ACRandom.begin(),
			randStep());
      //select cities
      thrust::reduce_by_key(ACInt2.begin(),
			    ACInt2.begin()+ ((numCities-x) * numAnts),
			    thrust::make_zip_iterator(thrust::make_tuple(thrust::make_counting_iterator(0),
									 thrust::make_permutation_iterator(probabilities.begin(),ACInt.begin()),
									 ACRandom.begin())),
			    thrust::make_discard_iterator(),
			    thrust::make_zip_iterator(thrust::make_tuple(AInt.begin(),
									 AFloat.begin(),
									 AUnsignedInt.begin())),
			    thrust::equal_to<int>(),
			    treeSelect());
      thrust::gather(AInt.begin(),
		     AInt.end(),
		     toVisit.begin(),
		     thrust::make_permutation_iterator(antTours.begin(),tourMap.begin()));
    }
  computeAntDistances();
  updatePheromones();
  computeProbabilities();
}

//computeAntDistances: Computes the distances of each ant's tour, then updates records.
void Colony::computeAntDistances()
{
  //compute distances
  thrust::transform(antTours.begin(),
		    antTours.end(), 
		    thrust::make_permutation_iterator(antTours.begin(),distMap.begin()),
		    ACInt.begin(),
		    saxpy_functor(numCities));	
  thrust::gather(ACInt.begin(),
		 ACInt.end(),
		 distances.begin(),
		 ACFloat.begin());
  thrust::reduce_by_key(ACKey.begin(),
			ACKey.end(),
			ACFloat.begin(),
			thrust::make_discard_iterator(),
			antDistances.begin());
  //update bests
  int i = thrust::min_element(antDistances.begin(),
			      antDistances.end()) - antDistances.begin();
  thrust::gather(thrust::make_counting_iterator(i*numCities),
		 thrust::make_counting_iterator((i+1)*numCities),
		 antTours.begin(),
		 iterBestTour.begin());
  iterBestDist = antDistances[i];
  if(iterBestDist < globBestDist){
    reps = 0;
    globBestDist = iterBestDist;
    globBestTour.assign(iterBestTour.begin(),iterBestTour.end());
  }else{
    reps++;
  }
}

//greedyDistance: Returns the value of a simple greedy solution starting at city 0.
float Colony::greedyDistance()
{
  float distance;
  int i = 0;
  int init = i;
  thrust::device_vector<int> visits(numCities);
  thrust::fill(visits.begin(),
	       visits.end(),
	       1);
  thrust::device_vector<float> Cfloat(numCities);
  int j;
  for(int x = 1; x < numCities; x++){
    visits[i] = 0;
    thrust::transform(visits.begin(),
		      visits.end(),
		      thrust::make_permutation_iterator(distances.begin(),thrust::make_counting_iterator(i*numCities)),
		      Cfloat.begin(),
		      thrust::divides<float>());
    j = thrust::max_element(Cfloat.begin(),
			    Cfloat.end()) - Cfloat.begin();
    distance += distances[numCities * i + j];
    i = j;
  }
  distance += distances[numCities * i + init];
  return distance;
}

//computeProbabilities: Computes the probabilities from the distances and pheromones.
void Colony::computeProbabilities()
{
  thrust::transform(pheromones.begin(),
		    pheromones.end(),
		    distances.begin(),
		    probabilities.begin(),
		    prob_functor(beta));
}

void Colony::setBeta(float newBeta)
{
  beta = newBeta;
}

void Colony::setRho(float newRho)
{
  rho = newRho;
}

double Colony::getBeta()
{
  return beta;
}

double Colony::getRho()
{
  return rho;
}

int Colony::getNumAnts()
{
  return numAnts;
}

double Colony::getIterBestDist()
{
  return iterBestDist;
}

double Colony::getGlobBestDist()
{
  return globBestDist;
}

int Colony::getReps()
{
  return reps;
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
