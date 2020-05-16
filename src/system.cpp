#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  set<Process> helperSet;

  for (unsigned int i = 0; i < pids.size(); ++i) {
    Process proc(pids[i]);
    helperSet.insert(proc);
  }
  for (auto it = helperSet.rbegin(); it != helperSet.rend(); ++it) {
    this->processes_.emplace_back(*it);
  }
  return this->processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

float System::MemoryUtilization() { LinuxParser::MemoryUtilization(); }

// TODO: fix bug
long int System::UpTime() { LinuxParser::UpTime(); }