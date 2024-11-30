#pragma once

// WINDOWS
#if defined(_WIN32) || defined(_WIN64)
//! Windows
#define OS_WINDOWS

// UNIX
#elif defined(unix) || defined(__unix) || defined (__unix__)
#define OS_UNIX

// APPLE
#elif defined(__APPLE__) || defined(__MACH__)
#define OS_MAC

// LINUX
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define OS_LINUX
#endif

namespace os {

	void printBuildInformation(void);

	void clearConsole(void);

	unsigned long long getTotalPhysicalMemory(void);
	unsigned long long getAvailablePhysicalMemory(void);
	double physicalMemoryUsageInPercent(void);

	unsigned long long getTotalVirtualMemory(void);
	unsigned long long getAvailableVirtualMemory(void);
	double virtualMemoryUsageInPercent(void);
}