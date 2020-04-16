#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
namespace fs = std::filesystem;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return string();
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
// std::filesystem solution
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  std::regex only_int_regex("[0-9]+");
  for (const auto& p : fs::directory_iterator(kProcDirectory)) {
    if (p.is_directory()) {
      if (std::regex_match(p.path().stem().string(), only_int_regex)) {
        int pid = stoi(p.path().stem().string());
        pids.push_back(pid);
      }
    }
  }
  return pids;
}
// Original Solution
vector<int> LinuxParser::Old_Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
// I'm using the "Red Hat Enterprise Linux 7.1 or later" formula
// Link : https://access.redhat.com/solutions/406773
// memUsed = MemTotal - MemFree - Buffers - Cached - Slab
// memTotal is initialized to 1 to avoid divide by zero if stream doesn't open
float LinuxParser::MemoryUtilization() {
  string key, line;
  float memUsed = 0.0f;
  float memTotal = 1.0f;
  int parsed = 0;
  float temp;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> temp;
      if (key == "MemTotal:") {
        memUsed += temp;
        memTotal = temp;
        parsed++;
      }
      if (key == "MemFree:" || key == "Buffers:" || key == "Cached:" ||
          key == "Slab:") {
        memUsed -= temp;
        parsed++;
      }
      if (parsed == 5)
        break;  // If i got all the information, it's no use to go on
    }
  }
  return memUsed / memTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long uptime = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
// I'm not using this function in the project
long LinuxParser::Jiffies() {
  string line, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  long jiffies = 0, temp;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;  // remove string 'cpu'
    while (linestream >> temp) jiffies += temp;
  }
  return jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line, values;
  long active_jiffies{0}, temp;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 13; i++) linestream >> values;
    linestream >> active_jiffies;  // utime stime cutime cstime
    linestream >> temp;            // stime
    active_jiffies += temp;
    linestream >> temp;  // cutime
    active_jiffies += temp;
    linestream >> temp;  // cstime
    active_jiffies += temp;
  }
  return active_jiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  string line, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  long active_jiffies{0}, temp;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;  // Remove cpu label
    linestream >> temp;   // user
    active_jiffies += temp;
    linestream >> temp;  // nice
    active_jiffies += temp;
    linestream >> temp;  // system
    active_jiffies += temp;
  }
  return active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  long idle_jiffies{0};
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 5; i++) linestream >> value;
    linestream >> idle_jiffies;
  }
  return idle_jiffies;
}

// TODO: Read and return CPU utilization
// This function reads the first line of /proc/stat
// and push all the informations in the returned vector
// Processor class uses these informations to calculate CpuUtilization
vector<string> LinuxParser::CpuUtilization() {
  string line, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::vector<string> cpu_values;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;  // remove string 'cpu'
    while (linestream >> value) cpu_values.emplace_back(value);
  }
  return cpu_values;
}

// Multiprocessor CpuUtilizazion
// This function reads the line number 'n_cpu' of /proc/stat
// and push all the informations in the returned vector
// Processor class uses these informations to calculate CpuUtilization
vector<string> LinuxParser::MultiCpuUtilization(int n_cpu) {
  string line, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::vector<string> cpu_values;
  if (stream.is_open()) {
    for (int i = 0; i <= n_cpu; i++) std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;  // remove string 'cpu'
    while (linestream >> value) cpu_values.emplace_back(value);
  }
  return cpu_values;
}

int LinuxParser::CountCpus() {
  string line, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  int n_cpus = 0;
  if (stream.is_open()) {
    std::getline(stream, line);  // Do not count total cpu
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> value;
      if (value.find("cpu") == std::string::npos) break;
      n_cpus++;
    }
  }
  return n_cpus;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key, line;
  int n_processes = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> n_processes;
      if (key == "processes") return n_processes;
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, line;
  int running_processes = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> running_processes;
      if (key == "procs_running") return running_processes;
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    return line;
  }
  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string key, value, line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") return value;
    }
  }
  return "0";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string key, value, line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") return value;
    }
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::string user, x, uid, line;
  std::string process_uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> uid;
      if (uid == process_uid) return user;
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line, values;
  long uptime = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 21; i++) linestream >> values;
    linestream >> uptime;
  }
  return uptime;
}