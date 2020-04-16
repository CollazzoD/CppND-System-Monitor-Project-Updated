#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

class System {
 public:
  System();
  Processor& Cpu();  // TODO: See src/system.cpp
  std::vector<Processor>& Cpus();
  std::vector<Process>& Processes();  // TODO: See src/system.cpp
  float MemoryUtilization();          // TODO: See src/system.cpp
  long UpTime();                      // TODO: See src/system.cpp
  int TotalProcesses();               // TODO: See src/system.cpp
  int RunningProcesses();             // TODO: See src/system.cpp
  std::string Kernel();               // TODO: See src/system.cpp
  std::string OperatingSystem();      // TODO: See src/system.cpp
  int NCpus() const;

  // TODO: Define any necessary private members
 private:
  Processor total_cpu_{0};
  std::vector<Processor> cpus_ = {};
  std::vector<Process> processes_ = {};
  std::vector<int> pids_;
  const std::string os_{LinuxParser::OperatingSystem()};
  const std::string kernel_{LinuxParser::Kernel()};
};

#endif