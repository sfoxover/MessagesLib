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
	#include <unistd.h>	
	#include <linux/types.h>
	#include <linux/times.h>
#endif

#ifdef _WIN32	
double GetProcessCpuUsageWin32()
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
double GetProcessCpuUsageLinux()
{

    FILE *in;
    char buff[512];
    if(!(in = popen("ps -C codeblocks -o %cpu,%mem | tail -n 2", "r"))){
        return 1;
    }
    while(fgets(buff, sizeof(buff), in)!=NULL)
    {
        cout << buff;
    }
    pclose(in);


    std::ifstream fileStat("/proc/stat");

    FILE* file;
	struct tms timeSample;
	char line[128];

	long lastCPU = times(&timeSample);
	long lastSysCPU = timeSample.tms_stime;
	long lastUserCPU = timeSample.tms_utime;

	file = fopen("/proc/cpuinfo", "r");
	int numProcessors = 0;
	while (fgets(line, 128, file) != NULL) {
		if (strncmp(line, "processor", 9) == 0) numProcessors++;
	}
	fclose(file);

	struct tms timeSample;
	clock_t now;
	double percent;

	now = times(&timeSample);
	if (now <= lastCPU || timeSample.tms_stime < lastSysCPU || timeSample.tms_utime < lastUserCPU) 
	{
		//Overflow detection. Just skip this value.
		percent = -1.0;
	}
	else {
		percent = (timeSample.tms_stime - lastSysCPU) + (timeSample.tms_utime - lastUserCPU);
		percent /= (now - lastCPU);
		percent /= numProcessors;
		percent *= 100;
	}
	lastCPU = now;
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;

	return percent;
}
#endif

// Get CPU usage for current process
double CCpuUsage::GetProcessCpuUsage()
{
#ifdef _WIN32	
    return GetProcessCpuUsageWin32();
#else
    return GetProcessCpuUsageLinux();
#endif
}