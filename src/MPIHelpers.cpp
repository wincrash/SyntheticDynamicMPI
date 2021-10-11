#pragma once
#include "MPIHelpers.hpp"
#include <fstream>

//////////////////////////////////////////////////////////////////////////////////////////
void InitilizeMPI(int argc, char *argv[], MpiInfo &mpiInfo) {
  mpiInfo.commandForNewProcess = std::string(argv[0]);
  MPI_Init(&argc, &argv);
  MPI_Comm_get_parent(&mpiInfo.parentComm);
  mpiInfo.MASTER = mpiInfo.parentComm == MPI_COMM_NULL;
  if (mpiInfo.MASTER)
    MPI_Comm_dup(MPI_COMM_WORLD, &mpiInfo.mainComm);
  else
    MPI_Intercomm_merge(mpiInfo.parentComm, 1, &mpiInfo.mainComm);
  MPI_Comm_rank(mpiInfo.mainComm, &mpiInfo.rank);
  MPI_Comm_size(mpiInfo.mainComm, &mpiInfo.world_size);

  int len;
  char name[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(name, &len);
  mpiInfo.name=std::string(name);
}
//////////////////////////////////////////////////////////////////////////////////////////
std::string CreateHostFile(int count) {
  std::ofstream outfile("hostfile", std::ios::trunc);
  for (int i = 0; i < count; i++)
    outfile << "localhost" << std::endl;

  outfile.close();
  return "hostfile";
}
//////////////////////////////////////////////////////////////////////////////////////////
void ScaleOut(int count, MpiInfo &mpiInfo) {
  if (mpiInfo.MASTER) {
    MPI_Info info = MPI_INFO_NULL;
    MPI_Info_create(&info);
    std::string hostFileName="hostfile";
    MPI_Info_set(info, "add-hostfile", &hostFileName[0]);
    MPI_Comm_spawn(mpiInfo.commandForNewProcess.c_str(), MPI_ARGV_NULL, count, info, 0, MPI_COMM_WORLD, &mpiInfo.childComm,
                   MPI_ERRCODES_IGNORE);
    MPI_Intercomm_merge(mpiInfo.childComm, 0, &mpiInfo.mainComm);
    MPI_Comm_rank(mpiInfo.mainComm, &mpiInfo.rank);
    MPI_Comm_size(mpiInfo.mainComm, &mpiInfo.world_size);
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void ScaleIn(MpiInfo &mpiInfo) {
  if (mpiInfo.mainComm != MPI_COMM_NULL)
    MPI_Comm_free(&mpiInfo.mainComm);
  mpiInfo.mainComm = MPI_COMM_NULL;
  if (mpiInfo.rank == 0) {
    mpiInfo.childComm = MPI_COMM_NULL;
    MPI_Comm_dup(MPI_COMM_WORLD, &mpiInfo.mainComm);
  } else {
    MPI_Finalize();
    exit(0);
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
