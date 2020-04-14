#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <vector>
#include <string>

using std::vector;
using std::string;

class Processor {
 public:
  float Utilization();

 private:
  long prevTotalJiffies;
  long prevActiveJiffies;
  vector<string> cpuJiffies;

  long Jiffies();
  long IdleJiffies();
  long ActiveJiffies();
};

#endif