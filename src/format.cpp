#include <string>
#include <cmath>
#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  int hours, minutes;
  string h_string, m_string, s_string;
  hours = floor(seconds/3600);
  minutes = (seconds%3600)/60;
  seconds = (seconds%3600)%60;
  
  h_string = std::to_string(hours);
  
  if (minutes >= 10) {
    m_string = std::to_string(minutes);
  }
  else if (minutes >= 0 && minutes < 10) {
    m_string = '0' + std::to_string(minutes); 
  }
  if (seconds >= 10) {
    s_string = std::to_string(seconds);
  }
  else if (seconds >= 0 && seconds < 10) {
    s_string = '0' + std::to_string(seconds); 
  }
  
  string time = h_string + ':' + m_string + ':' + s_string;
  
  return time; 
}