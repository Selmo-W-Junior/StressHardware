#include "CPUstress.h"

#include <thread>

CPUstress::CPUstress(bool _start)
{
	if (_start) start();
}

CPUstress::~CPUstress() {}

void CPUstress::startWorker(std::thread *& _thread) {
	_thread = new std::thread(&CPUstress::workerFunction, this);
}

void CPUstress::workerFunction(void) {
	double * nr = new double;
	while (checkContinueRunning()) {
		for (int i = 0; i < 1000; i++) (*nr) += 1.144564542315345;
	}
	delete nr;
}