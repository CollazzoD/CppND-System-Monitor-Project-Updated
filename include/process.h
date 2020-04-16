#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include "linux_parser.h"

/*
Basic class for Process representation
It contains relevant attributes as shown below

This class creates a sort of "snapshot" of the process when it is instantiated
*/
class Process {
 public:
  Process(int);
  int Pid() const;                         // TODO: See src/process.cpp
  std::string User() const;                // TODO: See src/process.cpp
  std::string Command() const;             // TODO: See src/process.cpp
  float CpuUtilization() const;            // TODO: See src/process.cpp
  std::string Ram() const;                 // TODO: See src/process.cpp
  long int UpTime() const;                 // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

  bool operator==(Process const& a) const;
  void update();

  // TODO: Declare any necessary private members
 private:
  int pid_;
  std::string user_;
  std::string command_;
  float cpu_;
  std::string ram_;
  long int uptime_;
  long int sys_uptime_;
  long int start_time_;

  void calculateCpu();
  void calculateUpTime();
  void calculateRam();
};

#endif