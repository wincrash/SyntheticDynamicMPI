#pragma once
#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>
#include <fstream>

typedef struct {
  bool MASTER = false;
  std::string commandForNewProcess;
  MPI_Comm mainComm = MPI_COMM_NULL;
  MPI_Comm parentComm = MPI_COMM_NULL;
  MPI_Comm childComm = MPI_COMM_NULL;
  int rank = 0;
  int world_size = 0;
} MpiInfo;

typedef struct 
{
  int cpu_count=1;
  int sleepTime=1;
  std::vector<std::string> hosts;
}MPILoad;

