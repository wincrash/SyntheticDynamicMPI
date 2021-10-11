#include "SystemCommand.hpp"
#include <stdexcept>
#include <string>

std::string SystemCommand(std::string cmd) {
  char buffer[128];
  std::string result = "";
  FILE *pipe = popen(cmd.c_str(), "r");
  if (!pipe)
    throw std::runtime_error("popen() failed!");
  try {
    while (fgets(buffer, sizeof buffer, pipe) != NULL) {
      result += buffer;
    }
  } catch (...) {
    pclose(pipe);
    throw;
  }
  pclose(pipe);
  return result;
}