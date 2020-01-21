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

// Get cpu time 
void CCpuUsage::GetCpuTime(double& workJif, double& totalJif)
{
	std::ifstream fs("/proc/stat");
	std::vector<std::string> results(std::istream_iterator<std::string>{fs}, std::istream_iterator<std::string>());
	if(results.size() >= 8)
	{
		workJif = 0;
		for (int n = 1; n < 4; n++)
			workJif += std::atof(results[n].c_str());

		totalJif = workJif;
		for(int n=4; n < 8; n++)
			totalJif += std::atof(results[n].c_str());
	}
}

double CCpuUsage::GetProcessCpuUsageLinux()
{
	std::stringstream buffer;
	buffer << "/proc/stat";

	double workJif1, totalJif1 = 0;
	GetCpuTime(workJif1, totalJif1);

	// Pause for 100 ms
	std::this_thread::sleep_for(100ms);

	double workJif2, totalJif2 = 0;
	GetCpuTime(workJif2, totalJif2);

	double work = workJif2 - workJif1;
	double total = totalJif2 - totalJif1;
	if (total > 0)
	{
		double cpu = work / total * 100;
		return cpu;
	}
	return 0;
}
#endif

// Get CPU usage for all cores on system with 100% max usage
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