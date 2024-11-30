
#include "OS.h"
#include <iostream>

#if defined(OS_WINDOWS)
#include <Windows.h>
#include <TCHAR.h>
#include <pdh.h>
#elif defined(OS_UNIX)
#include <unistd.h>
#include <cstdlib>
#elif defined(OS_LINUX)
#include "sys/types.h"
#include "sys/sysinfo.h"
#include <cstdlib>
#endif // 

void os::printBuildInformation(void) {
#if defined (WIN32)
	std::cout << "StressTool (Windows 32-bit version)" << std::endl;
#elif defined (OS_WINDOWS)
	std::cout << "StressTool (Windows 64-bit version)" << std::endl;
#else
	std::cout << "StressTool" << std::endl;
#endif // WIN32
}

void os::clearConsole(void) {
#ifdef OS_WINDOWS
	system("cls");
#elif defined(OS_UNIX)
	system("clear");
#elif defined(OS_LINUX)
	system("clear");
#elif defined(OS_MAC)
	system("clear");
#endif
}

unsigned long long os::getTotalPhysicalMemory(void) {
#if defined(OS_WINDOWS)
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullTotalPhys;
#elif defined (OS_UNIX)
	long pages = sysconf(_SC_PHYS_PAGES);
	long page_size = sysconf(_SC_PAGE_SIZE);
	return pages * page_size;
#elif defined (OS_LINUX)
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long totalPhysMem = memInfo.totalram;
	totalPhysMem *= memInfo.mem_unit;
	return totalPhysMem;
#endif
}

unsigned long long os::getAvailablePhysicalMemory(void) {
#if defined(OS_WINDOWS)
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullAvailPhys;
#elif defined (OS_UNIX)
	assert(0);
	return 0;
#elif defined (OS_LINUX)
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long availablePhysMem = memInfo.freeram;
	availablePhysMem *= memInfo.mem_unit;
	return availablePhysMem;
#endif
}

double os::physicalMemoryUsageInPercent(void) {
	return ((double)getAvailablePhysicalMemory() / (double)getTotalPhysicalMemory()) * 100.;
}

unsigned long long os::getTotalVirtualMemory(void) {
#if defined(OS_WINDOWS)
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullTotalVirtual;
#elif defined (OS_UNIX)
	assert(0);
	return 0;
#elif defined (OS_LINUX)
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long totalVirtualMem = memInfo.totalram;
	totalVirtualMem += memInfo.totalswap;
	totalVirtualMem *= memInfo.mem_unit;
	return totalVirtualMem;
#endif
}

unsigned long long os::getAvailableVirtualMemory(void) {
#if defined(OS_WINDOWS)
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullAvailVirtual;
#elif defined (OS_UNIX)
	assert(0);
	return 0;
#elif defined (OS_LINUX)
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long availableVirtualMem = memInfo.freeram;
	availableVirtualMem += memInfo.freeswap;
	availableVirtualMem *= memInfo.mem_unit;
	return availableVirtualMem;
#endif
}

double os::virtualMemoryUsageInPercent(void) {
	return ((double)getAvailableVirtualMemory() / (double)getTotalVirtualMemory()) * 100.;
}
