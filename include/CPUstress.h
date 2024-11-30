#pragma once

#include "AbstractThreadObject.h"

class CPUstress : public AbstractThreadObject {
public:
	CPUstress(bool _start);
	virtual ~CPUstress();

private:
	virtual void startWorker(std::thread *& _thread) override;
	void workerFunction(void);
	
	CPUstress() = delete;
	CPUstress(CPUstress&) = delete;
	CPUstress& operator = (CPUstress&) = delete;
};