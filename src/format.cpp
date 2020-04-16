#include "format.h"
#include <iomanip>
#include <sstream>
#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hours, minutes;
  std::ostringstream out;
  hours = seconds / 3600;
  seconds = seconds % 3600;
  minutes = seconds / 60;
  seconds = seconds % 60;

  out << std::setw(2) << std::setfill('0') << std::to_string(hours) << ":";
  out << std::setw(2) << std::setfill('0') << std::to_string(minutes) << ":";
  out << std::setw(2) << std::setfill('0') << std::to_string(seconds);
  return out.str();
}