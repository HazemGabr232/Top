#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

// TODO: Update this to use std::filesystem
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

float LinuxParser::MemoryUtilization() {
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  string line;
  string memTotal, memFree;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream line_stream(line);
    line_stream >> memTotal >> memTotal;
    std::getline(stream, line);
    line_stream.str(line);
    line_stream.clear();
    line_stream >> memFree >> memFree;
  }
  if (0 != stoi(memTotal))
    return (stoi(memTotal) - stoi(memFree)) / (float)stoi(memTotal);
  else
    return 0;
}

long LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line;
  string time;
  long uptime;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream line_stream(line);
    line_stream >> uptime;
    // uptime = stol(time);
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line, dummy;
  int counter = 14;
  long utime, stime, cutime, cstime;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream iss(line);

    while (--counter) {
      iss >> dummy;
    }
    iss >> utime >> stime >> cutime >> cstime;
  }

  return utime + stime + cutime + cstime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

vector<string> LinuxParser::CpuUtilization() {
  string desc, user, nice, system, idle, iowait, irq, softirq, steal, guest,
      guest_nice;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> desc >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest >> guest_nice;
  }

  return vector<string>{user, nice,    system, idle,  iowait,
                        irq,  softirq, steal,  guest, guest_nice};
}

int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, token;
  int processes;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream line_stream(line);
      line_stream >> token;
      if ("processes" == token) {
        line_stream >> processes;
        return processes;
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, token;
  int processes;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream line_stream(line);
      line_stream >> token;
      if ("procs_running" == token) {
        line_stream >> processes;
        return processes;
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line;
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

string LinuxParser::Ram(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line, token, memSize_s;
  long memSize;
  while (std::getline(stream, line)) {
    std::istringstream line_stream(line);
    while (line_stream >> token) {
      if ("VmSize:" == token) {
        line_stream >> memSize_s;
        memSize = stol(memSize_s);
        return to_string(memSize / 1000);
      }
    }
  }

  return "";
}

string LinuxParser::Uid(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line, token;
  string uid;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream line_stream(line);
      line_stream >> token;
      if ("Uid:" == token) {
        line_stream >> uid;
        return uid;
      }
    }
  }
  return "";
}

string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  string line, token, user;
  while (std::getline(stream, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream line_stream(line);
    line_stream >> user;
    line_stream >> token >> token;
    if (token.compare(uid) == 0) return user;
  }
  return "";
}

long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line, dummy;
  int counter = 22;
  long processStartTime;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream line_stream(line);
    while (--counter) {
      line_stream >> dummy;
    }
    line_stream >> processStartTime;
  }

  return LinuxParser::UpTime() - (processStartTime / sysconf(_SC_CLK_TCK));
}