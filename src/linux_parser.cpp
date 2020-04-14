#include <dirent.h>
#include <unistd.h>
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, memKey, memVal;
  long memTotal, memFree;
  bool memInfoFound = false;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> memKey >> memVal) {
        /* store MemTotal and MemFree values */
        if (memKey == "MemTotal:") {
          memTotal = std::stol(memVal);
        } else if (memKey == "MemFree:") {
          memFree = std::stol(memVal);
          memInfoFound = true;
          break;
        }
      }
    }
  }

  /* Return memUsed/memTotal value */
  if (memInfoFound) {
    return (float)(memTotal - memFree)/memTotal;
  } else {
    return 0;
  }
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line, uptime, idletime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    if (linestream >> uptime) {
      return std::stol(uptime);
    }
  }

  return 0;
}

// Read and return the number of jiffies for the system
// Implemented in processor.cpp
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
/* Not needed
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }*/

// Read and return the number of active jiffies for the system
// Implemented in processor.cpp
long LinuxParser::ActiveJiffies() { return 0; }

// Read and return the number of idle jiffies for the system
// Implemented in processor.cpp
long LinuxParser::IdleJiffies() { return 0; }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuJiffies{};
  string line, cpuTag;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    if (linestream >> cpuTag) {
      string tempJiffyVal;
      while (linestream >> tempJiffyVal) {
        cpuJiffies.push_back(tempJiffyVal);
      }
    }
  }
  return cpuJiffies;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string processesKey, processesValue;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> processesKey >> processesValue) {
        if (processesKey == "processes") {
          return std::stoi(processesValue);
        }
      }
    }
  }

  /* file not readable or no "processes" key found */
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string procsRunningKey, procsRunningValue;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> procsRunningKey >> procsRunningValue) {
        if (procsRunningKey == "procs_running") {
          return std::stoi(procsRunningValue);
        }
      }
    }
  }

  /* file not readable or no "procs_running" key found */
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      return line;
    }
  }

  return string();
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, vmSizeKey, vmSizeVal;

  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> vmSizeKey >> vmSizeVal) {
        if (vmSizeKey == "VmSize:") {
          return to_string(std::stol(vmSizeVal)/1024);
        }
      }
    }
  }

  return string();
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, uidKey, uidVal;
  std::ifstream filestream (kProcDirectory + "/" + (to_string(pid)) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> uidKey >> uidVal) {
        if (uidKey == "Uid:") {
          return uidVal;
        }
      }
    }
  }

  return string();
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, tempToken, uidToken, userToken;

  /* get uid for the given pid */
  string uid = Uid(pid);

  std::ifstream filestream(kPasswordPath);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      int count = 0;

      /* split the password file line with ':' as delimiter */
      while (std::getline(linestream, tempToken, ':')) {
        /* first token is the username */
        if (count == 0) {
          userToken = tempToken;
        } else if (count == 2) {
          /* third token is the uid */
          uidToken = tempToken;
          break;
        }
        count++;
      }

      /*
       * if uid token from the password file is same as the uid read from the
       * proc status file, then we have found the right username.
       */
      if (uidToken == uid) {
        return userToken;
      }
    }
  }

  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  /* get process stats */
    const vector<string> procStats = LinuxParser::ProcessStats(pid);

    /*
     * Not a very good safety check, but it protects against reading
     * empty/invalid files atleast.
     */
    if (procStats.size() >= 22) {
        return UpTime() - (std::stol(procStats[21])/sysconf(_SC_CLK_TCK));
    } else {
        return 0;
    }
}

const vector<string> LinuxParser::ProcessStats(int pid) {
  string line, token;
  vector<string> procStats;

  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> token) {
        procStats.push_back(token);
      }
    }
  }

  return procStats;
}