#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include "linux_parser.h"

using std::string;

class Processor {
 public:
  Processor(int n_cpu) : n_cpu_(n_cpu) {}
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  // See Linux ManPage Link and StackOverflow Link to understand these variables
  // Linux ManPage Link
  // http://man7.org/linux/man-pages/man5/proc.5.html
  // StackOverflow Link
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  float user_{0}, nice_{0}, system_{0}, idle_{0}, iowait_{0}, irq_{0},
      softirq_{0}, steal_{0};
  float prevuser_{0}, prevnice_{0}, prevsystem_{0}, previdle_{0},
      previowait_{0}, previrq_{0}, prevsoftirq_{0}, prevsteal_{0};

  void ConvertCpuParameters(const std::vector<string>&);
  void SwapActualPrev();
  int n_cpu_;
};

#endif