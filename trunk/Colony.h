/****************************************
 * Colony.h                             *
 * Peter Ahrens                         *
 * Main ACO procedures                  *
 ****************************************/

#ifndef COLONY_H
#define COLONY_H
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/copy.h>
#include <thrust/fill.h>
#include <thrust/binary_search.h>
#include <thrust/sequence.h>
#include <thrust/adjacent_difference.h>
#include <thrust/random.h>
#include <thrust/functional.h>
#include <thrust/gather.h>
#include <thrust/iterator/constant_iterator.h>
#include <thrust/iterator/discard_iterator.h>
#include <thrust/scan.h>
#include <thrust/sort.h>
#include <thrust/remove.h>
#include <iostream>
#include <sys/time.h>
#include <math.h>
//saxpy_functor: Performs the operation s = a * x + y, where a is a constant.
struct saxpy_functor
{
  const float a ;
  saxpy_functor ( float _a ) : a ( _a ) {}
  __host__ __device__
  float operator () ( const float & x , const float & y ) const 
  {
    return a * x + y ;
  }
};

//treeSelect: The main function used to reduce two cities that an ant might visit.
struct treeSelect : public thrust::binary_function<thrust::tuple<int,float,unsigned int>,thrust::tuple<int,float,unsigned int>,thrust::tuple<int,float,unsigned int> >
{
  treeSelect() {}
  __host__ __device__
    thrust::tuple<int,float,unsigned int> operator()(const thrust::tuple<int,float,unsigned int> tup1,const thrust::tuple<int,float,unsigned int> tup2) const 
  {
    const float prob = tup1.get<1>() + tup2.get<1>();
    if(tup1.get<2>() * prob / 4294967296 < tup1.get<1>()){
      return thrust::make_tuple(tup1.get<0>(),prob,tup2.get<2>());
    } else{
      return thrust::make_tuple(tup2.get<0>(),prob,tup2.get<2>());
    }
  }
};

//prob_functor: Performs the probabilistic desireability calculation s = (pheromone level)^alpha * (1/distance)^beta
struct prob_functor: public thrust::binary_function<float,float,float>
{
  const float beta;
  prob_functor ( float _beta ) : beta ( _beta ) {}
  __host__ __device__
    float operator () ( const float & pher , const float & dist ) const
  {
    return pher * pow(1 / dist, beta) ;
  }
};

//randStep: Performs one step of a linear congruential generator
struct randStep : public thrust::unary_function<unsigned int, unsigned int>
{
  __host__ __device__
    unsigned int operator()(const unsigned int x) const
  {
	//numerical recipies LCG values
    return ((x * 1664525) + 1013904223) % 4294967296;
  }
};

//unaryMultiplies: Multiplies all the values of an array by a value.
struct unaryMultiplies : public thrust::unary_function<int, int>
{
  const int y;
  unaryMultiplies (int _y ) : y ( _y ) {}
  __host__ __device__
    int operator()(const int x) const
  {
    return x * y;
  }
};

//unaryPlus: Adds a value to all the elements of an array.
struct unaryPlus : public thrust::unary_function<int, int>
{
  const int y;
  unaryPlus ( int _y ) : y ( _y ) {}
  __host__ __device__
    int operator()(const int x) const
  {
    return x + y;
  }
};

//isX: Checks to see if a elements of an array are equal to a given constant.
struct isX
{
  const int y;
isX( int _y ) : y ( _y ) {}
  __host__ __device__
  bool operator()(const int x) const
  {
    return x == y;
  }
};

//Colony: The main ACO functions and data.
class Colony
{
 public:
  Colony(thrust::host_vector<float> newDistances, int newNumCities, int newNumAnts);
  void initialize(); // Initializes data, creates maps and keys, performs standard ACO initialization steps etc.
  void forage(); // Main ACO loop. Performs the solution constructruction step, then updates distances, pheromones, probabilities.
  void computeAntDistances(); // Computes the distances of each ant's tour, then updates records.
  void computeProbabilities(); // Computes the probabilities from the distances and pheromones.
  void setRho(float newRho);
  void setBeta(float newBeta);
  double getRho();
  double getBeta();
  int getNumAnts();
  double getIterBestDist();
  double getGlobBestDist();
  int getReps();
  virtual void computeParameters() = 0; //Implemented differently in each ACO.
 protected:
  float greedyDistance(); // Returns the value of a simple greedy solution starting at city 0.
  virtual void computeInitialPheromone() = 0; //Implemented differently in each ACO.
  virtual void updatePheromones() = 0; //Implemented differently in each ACO.
  //world vars
  int numCities;
  int reps;
  //float alpha = 1, alpha is always 1
  float beta;
  float rho;
  float initialPheromone;
  thrust::device_vector<float> pheromones;
  thrust::device_vector<float> distances;
  thrust::device_vector<float> probabilities;
  //ant vars
  int numAnts;
  float iterBestDist;
  float globBestDist;
  thrust::device_vector<int> iterBestTour;
  thrust::device_vector<int> globBestTour;
  thrust::device_vector<float> antVisits;
  thrust::device_vector<int> toVisit;
  thrust::device_vector<int> antTours;
  thrust::device_vector<float> antDistances;
  thrust::device_vector<float> currentProbabilities;
  thrust::device_vector<int> currentNeighbors;
  //maps and keys
  thrust::device_vector<int> ACMapF;
  thrust::device_vector<int> ACMapL;
  thrust::device_vector<int> tourMap;
  thrust::device_vector<int> distMap;
  thrust::device_vector<int> ACKey;
  thrust::device_vector<int> ARepeatCMap;
  thrust::device_vector<int> ANMapF;
  thrust::device_vector<int> ANMapL;
  thrust::device_vector<int> ANKey;
  thrust::device_vector<int> CCKey;
  thrust::device_vector<int> ARepeatNMap;
  //scratch variables
  thrust::device_vector<float> AFloat;
  thrust::device_vector<int> AInt;
  thrust::device_vector<int> CInt;
  thrust::device_vector<int> ACInt;
  thrust::device_vector<int> ACInt2;
  thrust::device_vector<int> ACInt3;
  thrust::device_vector<float> ACFloat;
  thrust::device_vector<float> CCFloat;
  thrust::device_vector<unsigned int> AUnsignedInt;
  //random numbers
  thrust::device_vector<unsigned int> ARandom;
  thrust::device_vector<unsigned int> ACRandom;
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
