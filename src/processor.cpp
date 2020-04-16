#include "processor.h"

void Processor::ConvertCpuParameters(const std::vector<string>& actual_cpu) {
  user_ = std::stof(actual_cpu[0]);
  nice_ = std::stof(actual_cpu[1]);
  system_ = std::stof(actual_cpu[2]);
  idle_ = std::stof(actual_cpu[3]);
  iowait_ = std::stof(actual_cpu[4]);
  irq_ = std::stof(actual_cpu[5]);
  softirq_ = std::stof(actual_cpu[6]);
  steal_ = std::stof(actual_cpu[7]);
}

void Processor::SwapActualPrev() {
  prevuser_ = user_;
  prevnice_ = nice_;
  prevsystem_ = system_;
  previdle_ = idle_;
  previowait_ = iowait_;
  previrq_ = irq_;
  prevsoftirq_ = softirq_;
  prevsteal_ = steal_;
}

// TODO: Return the aggregate CPU utilization
// See the formula on StackOverflow at the following Link
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization() {
  auto actual_cpu = LinuxParser::MultiCpuUtilization(n_cpu_);
  ConvertCpuParameters(actual_cpu);

  float PrevIdle = previdle_ + previowait_;
  float Idle = idle_ + iowait_;

  float PrevNonIdle = prevuser_ + prevnice_ + prevsystem_ + previrq_ +
                      prevsoftirq_ + prevsteal_;
  float NonIdle = user_ + nice_ + system_ + irq_ + softirq_ + steal_;

  float PrevTotal = PrevIdle + PrevNonIdle;
  float Total = Idle + NonIdle;

  float totald = Total - PrevTotal;
  float idled = Idle - PrevIdle;

  SwapActualPrev();

  return (totald - idled) / totald;
}