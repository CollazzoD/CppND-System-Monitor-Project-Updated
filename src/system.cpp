#include <unistd.h>
#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return total_cpu_; }

std::vector<Processor>& System::Cpus() { return cpus_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  auto actual_pids = LinuxParser::Pids();

  // Check if processes have to be removed (no more in /proc folder)
  for (auto& pid : pids_)
    if (std::find(std::begin(actual_pids), std::end(actual_pids), pid) ==
        std::end(actual_pids))
      std::remove(std::begin(processes_), std::end(processes_), pid);

  // Update information of processes
  for (auto& proc : processes_) proc.update();

  // Check if there are new processes
  // New processes come with updated informations
  for (const auto& pid : actual_pids)
    if (std::find(std::begin(pids_), std::end(pids_), pid) == std::end(pids_)) {
      processes_.emplace_back(Process(pid));
      pids_.emplace_back(pid);
    }

  std::sort(std::begin(processes_), std::end(processes_),
            [](const auto& a, const auto& b) { return a < b; });
  return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return kernel_; }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() { return os_; }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }

int System::NCpus() const { return cpus_.size(); }

// Constructor : initialize pids_ and processes_
System::System() : pids_(LinuxParser::Pids()) {
  int n_cpus = (LinuxParser::CountCpus());
  for (int i = 1; i <= n_cpus; i++) cpus_.emplace_back(Processor(i));
  for (const auto& pid : pids_) processes_.emplace_back(Process(pid));
}
