#include "CpuUsage.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
	#include "windows.h"
	#include <atlbase.h>
	#include <comdef.h>
#else
	#include <iterator>
	#include <unistd.h>	
	#include <linux/types.h>
	#include <linux/times.h>
#endif
#include <cassert>

using namespace std::chrono_literals;

#ifdef _WIN32	

double CCpuUsage::GetProcessCpuUsageWin32()
{
    SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	int numProcessors = sysInfo.dwNumberOfProcessors;

	FILETIME ftime, fsys, fuser;
	GetSystemTimeAsFileTime(&ftime);
	LARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	memcpy(&lastCPU, &ftime, sizeof(FILETIME));

	// Sample cpu usage
	HANDLE hProc = GetCurrentProcess();
	GetProcessTimes(hProc, &ftime, &ftime, &fsys, &fuser);
	memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

	// Pause for 100 ms
	std::this_thread::sleep_for(100ms);

	// Resample cpu usage
	ULARGE_INTEGER now, sys, user;
	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));
	GetProcessTimes(hProc, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	double percent = (sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart);
	auto diff = now.QuadPart - lastCPU.QuadPart;
	if (diff != 0 && numProcessors != 0)
	{
		percent /= (now.QuadPart - lastCPU.QuadPart);
		percent /= numProcessors;
		return percent * 100;
	}
	else
	{
		return 0;
	}
}
#endif

#ifndef _WIN32	

// System uptime in seconds
double CCpuUsage::GetUptime()
{
	double result = 0;
	std::ifstream fs("/proc/uptime");
	std::vector<std::string> results(std::istream_iterator<std::string>{fs}, std::istream_iterator<std::string>());
	if (!results.empty())
	{
		result = std::atof(results[0].c_str());
	}
	return result;
}

// Get cpu time for process
double CCpuUsage::GetCpuTime(std::string cpuPath)
{
	std::ifstream fs(cpuPath);
	std::vector<std::string> results(std::istream_iterator<std::string>{fs}, std::istream_iterator<std::string>());
	if(results.size() > 21)
	{
		// #14 CPU time spent in user code, measured in clock ticks
		double utime = std::atof(results[13].c_str());

		// #15 CPU time spent in kernel code, measured in clock ticks
		double stime = std::atof(results[14].c_str());

		// #16 Waited - for children's CPU time spent in user code (in clock ticks)
		double cutime = std::atof(results[15].c_str());

		// #17 Waited for children's CPU time spent in kernel code (in clock ticks)
		double cstime = std::atof(results[16].c_str());

		double pidTotalTime = utime + stime + cutime + cstime;
		return pidTotalTime;		
	}
	return 0;
}

double CCpuUsage::GetProcessCpuUsageLinux()
{
	auto pid = ::getpid();	
	std::stringstream buffer;
	buffer << "/proc/" << pid << "/stat";
	std::string pidStat = buffer.str();

	auto cpuUage1 = GetCpuTime(pidStat);

	// Pause for 100 ms
	std::this_thread::sleep_for(100ms);

	auto cpuUage2 = GetCpuTime(pidStat);

	long hertz = sysconf(_SC_CLK_TCK);
	assert(hertz > 0);
	if (hertz > 0)
	{
		float result = (cpuUage2 - cpuUage1) / 100 * 100.0;
		return result;
	}

	return 0;
}
#endif

// Get CPU usage for current process
double CCpuUsage::GetProcessCpuUsage()
{
#ifdef _WIN32	
	CCpuUsage cpu;
    return cpu.GetProcessCpuUsageWin32();
#else
	CCpuUsage cpu;
    return cpu.GetProcessCpuUsageLinux();
#endif
}