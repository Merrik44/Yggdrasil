#ifndef PERFORMANCETIMER_H
#define PERFORMANCETIMER_H

typedef long long int64;
typedef unsigned long long uint64;
int64 GetTimeMs64();
void process_mem_usage(double& vm_usage, double& resident_set);
#endif // PERFORMANCETIMER_H
