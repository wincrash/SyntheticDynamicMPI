#include "MPIHelpers.hpp"
#include <fstream>

//////////////////////////////////////////////////////////////////////////////////////////
void InitilizeMPI(int argc, char *argv[], MpiInfo &mpiInfo) {
  mpiInfo.commandForNewProcess = std::string(argv[0]);
  mpiInfo.mainComm = MPI_COMM_NULL;

  MPI_Init(&argc, &argv);
  MPI_Comm parentComm = MPI_COMM_NULL;
  MPI_Comm_get_parent(&parentComm);
  mpiInfo.MASTER = parentComm == MPI_COMM_NULL;
  if (mpiInfo.MASTER)
    MPI_Comm_dup(MPI_COMM_WORLD, &mpiInfo.mainComm);
  else
    MPI_Intercomm_merge(parentComm, 1, &mpiInfo.mainComm);
  MPI_Comm_rank(mpiInfo.mainComm, &mpiInfo.rank);
  MPI_Comm_size(mpiInfo.mainComm, &mpiInfo.world_size);

  int len;
  char name[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(name, &len);
  mpiInfo.name = std::string(name);
  if (mpiInfo.MASTER)
    std::cout << "Master started with rank=0\n";
  else
    std::cout << "Child with rank=" << mpiInfo.rank << " world_size=" << mpiInfo.world_size << " node=" << mpiInfo.name << "\n";
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
void ScaleOut(int count, MpiInfo &mpiInfo, std::string nodename) {
  std::string pathAddHost = "hostfile";
  std::ofstream outfile(pathAddHost, std::ios::trunc);
  outfile << nodename << std::endl;
  outfile.close();
  MPI_Info info = MPI_INFO_NULL;
  MPI_Info_create(&info);
  MPI_Info_set(info, "add-hostfile", &pathAddHost[0]);
  MPI_Comm childComm = MPI_COMM_NULL;
  MPI_Comm_spawn(mpiInfo.commandForNewProcess.c_str(), MPI_ARGV_NULL, count, info, 0, mpiInfo.mainComm, &childComm, MPI_ERRCODES_IGNORE);
  MPI_Intercomm_merge(childComm, 1, &mpiInfo.mainComm);
  MPI_Comm_rank(mpiInfo.mainComm, &mpiInfo.rank);
  MPI_Comm_size(mpiInfo.mainComm, &mpiInfo.world_size);
}
//////////////////////////////////////////////////////////////////////////////////////////
void ScaleIn(MpiInfo &mpiInfo) {
  bool isRemove = false;
  isRemove = mpiInfo.rank == (mpiInfo.world_size - 1);
  MPI_Comm newCommunicator;
  MPI_Comm_split(mpiInfo.mainComm, isRemove, mpiInfo.rank, &newCommunicator);
  MPI_Comm_free(&mpiInfo.mainComm);
  mpiInfo.mainComm = MPI_COMM_NULL;
  MPI_Comm_dup(newCommunicator, &mpiInfo.mainComm);
  if (isRemove) {
    std::cout<<"Remove child "<<mpiInfo.rank<<"\n";
    //MPI_Comm_free(&newCommunicator);
    //MPI_Comm_free(&mpiInfo.mainComm);
    MPI_Finalize();
    exit(0);
  }
  MPI_Comm_rank(mpiInfo.mainComm, &mpiInfo.rank);
  MPI_Comm_size(mpiInfo.mainComm, &mpiInfo.world_size);
  MPI_Barrier(mpiInfo.mainComm);
}
//////////////////////////////////////////////////////////////////////////////////////////
