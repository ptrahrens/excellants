#
# Makefile for Excellants
#
# Compiler: nvcc
# Peter Ahrens
# 

all: Ants	

Ants: Colony.o RankBasedAntSystem.o TSPReader.o Writer.o Comm.o Setup.o
	nvcc Setup.o Comm.o Writer.o TSPReader.o Colony.o RankBasedAntSystem.o -O3 -o Ants

Setup.o: Setup.cpp
	nvcc Setup.cpp -c -O3

Comm.o: Comm.cpp
	nvcc Comm.cpp -c -O3

Writer.o: Writer.cpp
	nvcc Writer.cpp -c -O3

TSPReader.o: TSPReader.cu
	nvcc TSPReader.cu -c -O3

RankBasedAntSystem.o: RankBasedAntSystem.cu
	nvcc RankBasedAntSystem.cu -c -O3

Colony.o: Colony.cu
	nvcc Colony.cu -c -O3

clean:
	- rm Colony.o RankBasedAntSystem.o TSPReader.o Writer.o Comm.o Setup.o Ants

#Copyright (c) 2012, Peter Ahrens
#All rights reserved.
#
#Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#
#    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#    Neither the name of Excellants nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
