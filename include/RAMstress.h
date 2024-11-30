#pragma once

#include "AbstractThreadObject.h"

class RAMstress : public AbstractThreadObject {
public:
	RAMstress(bool _start);
	virtual ~RAMstress();

private:
	virtual void startWorker(std::thread *& _thread) override;
	void workerFunction(void);

	RAMstress() = delete;
	RAMstress(RAMstress&) = delete;
	RAMstress& operator = (RAMstress&) = delete;
};