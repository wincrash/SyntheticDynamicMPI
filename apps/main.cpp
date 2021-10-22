#include "MPIHelpers.hpp"
#include "SystemCommand.hpp"
#include <algorithm>
#include <deque>
#include <numeric>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include "SystemCommand.hpp"
void Check(MPI_Comm &comm) {
  MPI_Barrier(comm);

  int rank = 0;
  int world_size = 0;
  int len;
  char name[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(name, &len);
  std::string pavadinimas = std::string(name);
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &world_size);
  std::vector<int> array;
  array.resize(world_size, -1);
  MPI_Gather(&rank, 1, MPI_INT, &array[0], 1, MPI_INT, 0, comm);
  int rez = 0;
  if (rank == 0)
    rez = world_size;
  MPI_Bcast(&rez, 1, MPI_INT, 0, comm);

  if (rank == 0)
    std::cout << "----------------------------\n";
  MPI_Barrier(comm);
  std::cout << "size=" << world_size << " rank=" << rank << " rez=" << rez << "\n";
  MPI_Barrier(comm);
  if (rank == 0)
    std::cout << "----------------------------\n";
  MPI_Barrier(comm);
}

int main(int argc, char **argv) {

std::cout<<"Mano pid="<<  getpid()<<"\n";

  MpiInfo mpiInfo;
  InitilizeMPI(argc, argv, mpiInfo);

  int id=0;
  while(id!=-1)
  {
    if(mpiInfo.MASTER)
    {
      //std::cin>>id;
      id=0;
      std::string res=SystemCommand("curl -s http://localhost:5000/status");
      std::cout<<"res="<<res<<"\n";
      if(res.compare("ADD")==0) id=1;
      if(res.compare("REMOVE")==0) id=-2;
      if(res.compare("EXIT")==0) id=-1;
    }
    MPI_Bcast( &id,1,MPI_INT,0,mpiInfo.mainComm);
    sleep(1);
    MPI_Barrier(mpiInfo.mainComm);
    std::cout<<id<<" "<<mpiInfo.rank<<" "<<mpiInfo.world_size<<"\n";
    Check(mpiInfo.mainComm);
    MPI_Barrier(mpiInfo.mainComm);
    if(id==1)
    {
      ScaleOut(1, mpiInfo, "localhost");
      id=0;
    }
    if(id==-2)
    {
      ScaleIn(mpiInfo);
      id=0;
    }
  }

  while(mpiInfo.world_size!=1)
  {
    
    ScaleIn(mpiInfo); 
  }
  std::cout<<"FINAL\n";
  MPI_Finalize();
  exit(0);
  return 0;
}