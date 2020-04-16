#include <unistd.h>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpu_; }

// Calculating process' CPU usage according to the following link from
// StackOverflow
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
void Process::calculateCpu() {
  float total_time = LinuxParser::ActiveJiffies(pid_);
  float starttime = start_time_;
  auto Hertz = sysconf(_SC_CLK_TCK);

  float seconds = sys_uptime_ - (starttime / Hertz);

  cpu_ = ((total_time / Hertz) / seconds);
}

// TODO: Return the command that generated this process
string Process::Command() const { return command_; }

// TODO: Return this process's memory utilization
// Convert Ram to MB
string Process::Ram() const { return ram_; }

void Process::calculateRam() {
  int ram_mb = std::stoi(LinuxParser::Ram(pid_)) / 1000;
  ram_ = std::to_string(ram_mb);
}

// TODO: Return the user (name) that generated this process
string Process::User() const { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const { return uptime_; }

void Process::calculateUpTime() {
  sys_uptime_ = LinuxParser::UpTime();
  uptime_ = sys_uptime_ - (start_time_ / sysconf(_SC_CLK_TCK));
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return this->cpu_ > a.cpu_; }

bool Process::operator==(Process const& a) const {
  return this->pid_ == a.pid_;
}

Process::Process(int pid)
    : pid_(pid),
      user_(LinuxParser::User(pid)),
      command_(LinuxParser::Command(pid)),
      start_time_(LinuxParser::UpTime(pid)) {
  calculateUpTime();
  calculateRam();
  calculateCpu();
}

void Process::update() {
  calculateUpTime();
  calculateRam();
  calculateCpu();
}