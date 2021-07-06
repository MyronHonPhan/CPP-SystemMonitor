#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
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
vector<int> LinuxParser::Pids() {
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
float LinuxParser::MemoryUtilization() {
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  string line,key;
  float value;
  float memTotal, memFree;
  if (filestream.is_open()){
    while(std::getline(filestream,line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:"){
        memTotal = value;
      }
      if (key == "MemFree:") {
        memFree = value;
      }
    }
  }
  return ((memTotal-memFree)/memTotal);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long key;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream,line);
    std::istringstream linestream(line);
    linestream>>key;
  }
  return key;
 }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  vector<string> values;
  string line, value;
  long utime, stime, cutime, cstime;
  if (filestream.is_open()) {
    std::getline(filestream,line);
    std::istringstream linestream(line);
    while (linestream >> value) {
     values.emplace_back(value);
    }
  }
  utime = std::stol(values[13]);
  stime = std::stol(values[14]);
  cutime = std::stol(values[15]);
  cstime = std::stol(values[16]);
  return (utime + stime + cutime + cstime)/sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> jiffie = CpuUtilization();
  long total = 0;
  for (int i = 0; i < 7; i++){
   total += std::stol(jiffie[i]); 
  }
  return total; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiffie = CpuUtilization();
  long total = stol(jiffie[3]) + stol(jiffie[4]);
  return total;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> jiffie;
  string cpu, cpuValue, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream,line);
    std::istringstream linestream(line);
    
    linestream >> cpu;
    while (linestream>>cpuValue){
     jiffie.emplace_back(cpuValue); 
    }
    
  }
  return jiffie; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key, line;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
   while(std::getline(filestream,line)){
     std::istringstream linestream(line);
     linestream >> key >> value;
     if (key == "processes") {
      return value; 
     }
   }
  }
  return value; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses()  { 
  string key, line;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
   while(std::getline(filestream,line)){
     std::istringstream linestream(line);
     linestream >> key >> value;
     if (key == "procs_running") {
      return value; 
     }
   }
  }
  return value; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(filestream.is_open()) {
   std::getline(filestream,line);
  }
  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string key, line;
  int value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(filestream.is_open()) {
   while(std::getline(filestream,line)) {
   	 std::replace(line.begin(), line.end(), ':', ' ');
     std::istringstream linestream(line);
     while (linestream >> key >> value) {
      if (key == "VmSize") {
       value = value / 1000;
        return std::to_string(value);
      }
     }
   }
  }
  return std::to_string(value); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string key, line, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(filestream.is_open()) {
   while(std::getline(filestream,line)) {
   	 std::replace(line.begin(), line.end(), ':', ' ');
     std::istringstream linestream(line);
     while (linestream >> key >> value) {
      if (key == "Uid") {
        return value;
      }
     }
   }
  }
  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string key, line, value, user_id;
  string uid = Uid(pid);
  std::ifstream filestream(LinuxParser::kPasswordPath);
  if (filestream.is_open()) {
    while(std::getline(filestream,line)) {
   	 std::replace(line.begin(), line.end(), ':', ' ');
     std::istringstream linestream(line);
     while (linestream >> key >> value >> user_id) {
      if (user_id == uid) {
        return key;
      }
     }
   }
  }
  return key; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  long Uptime = 0;
  string placeholder, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    
    for (int i = 0; i < 21 ; i++) {
      linestream >> placeholder;
    }
    linestream >> Uptime;
  }
  return Uptime/sysconf(_SC_CLK_TCK);
}
