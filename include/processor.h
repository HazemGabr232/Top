#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long _last_user;
  long _last_nice;
  long _last_system;
  long _last_idle;
  long _last_iowait;
  long _last_irq;
  long _last_softirq;
  long _last_steal;
};

#endif