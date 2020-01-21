#pragma once

#ifndef _WIN32	
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
const int NUM_CPU_STATES = 10;
#endif // _WIN32	

class CCpuUsage
{
public:
    // Get CPU usage for current process
    static double GetProcessCpuUsage();

#ifndef _WIN32	
	// Get cpu usage % for process
	double GetProcessCpuUsageLinux();

	// Get cpu time for process
	void GetCpuTime(double& work, double& total);

#else
	// Get cpu usage % for process
	double GetProcessCpuUsageWin32();
#endif // _WIN32	

};