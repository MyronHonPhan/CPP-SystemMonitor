#include "processor.h"
#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  long total_jiff = LinuxParser::Jiffies();
  long active_jiff = LinuxParser::ActiveJiffies();
  long idle_jiff = LinuxParser::IdleJiffies();
  
  float total_diff = total_jiff - total_;
  float idle_diff = idle_jiff - idle_;
  float util = (total_diff - idle_diff)/total_diff;
  
  total_ = total_jiff;
  idle_ = idle_jiff;
  active_ = active_jiff;
  
  return util; 
}
