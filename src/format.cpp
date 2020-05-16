#include "format.h"

#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  long sec = seconds % 60;
  long minutes = (seconds - sec) / 60;
  long min = minutes % 60;
  long hours = (minutes - min) / 60;
  string formated_str;
  if (hours < 10) {
    formated_str += "0";
  }
  formated_str += std::to_string(hours) + ":";
  if (min < 10) {
    formated_str += "0";
  }
  formated_str += std::to_string(min) + ":";
  if (sec < 10) {
    formated_str += "0";
  }
  formated_str += std::to_string(sec);
  return formated_str;
}