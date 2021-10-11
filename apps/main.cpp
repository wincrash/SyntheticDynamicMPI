#include "MPIHelpers.hpp"
#include "SystemCommand.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <deque>

std::deque<MPILoad> CreateLoad()
{
  std::deque<MPILoad> loads;
  MPILoad l;
  l.cpu_count=1;
  l.sleepTime=5;
  loads.push_back(l);

  
  l.cpu_count=2;
  l.sleepTime=4;
  loads.push_back(l);

  
  l.cpu_count=3;
  l.sleepTime=3;
  loads.push_back(l);


  
  l.cpu_count=4;
  l.sleepTime=2;
  loads.push_back(l);


  
  l.cpu_count=5;
  l.sleepTime=1;
  loads.push_back(l);
return loads;
}

int main(int argc, char **argv) {
  MPILoad mpiLoad;
  MpiInfo mpiInfo;
  InitilizeMPI(argc, argv, mpiInfo);
  std::deque<MPILoad> loads=CreateLoad();
  while(true)
  {
    if(mpiInfo.MASTER)
    {
      if(loads.size()==0)
        break;
      mpiLoad=loads[0];
      ScaleOut(loads[0].cpu_count,mpiInfo);
      loads.pop_front();
    }
    MPI_Bcast( &mpiLoad.sleepTime,1,MPI_INT,0,mpiInfo.mainComm);
    std::cout<<"Rank="<<mpiInfo.rank<<" Size="<<mpiInfo.world_size<<" sleep="<<mpiLoad.sleepTime<<"\n";
    sleep(mpiLoad.sleepTime);
    ScaleIn(mpiInfo);
  }
  MPI_Finalize();
  return 0;
}