#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return this->pid_; }

float Process::CpuUtilization() const {
  return (LinuxParser::ActiveJiffies(this->pid_) / sysconf(_SC_CLK_TCK)) /
         (float)LinuxParser::UpTime(this->pid_);
}

string Process::Command() { return LinuxParser::Command(this->pid_); }

string Process::Ram() { return LinuxParser::Ram(this->pid_); }

string Process::User() { return LinuxParser::User(this->pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(this->pid_); }

bool Process::operator<(Process const& a) const {
  if (this->CpuUtilization() < a.CpuUtilization())
    return true;
  else
    return false;
}