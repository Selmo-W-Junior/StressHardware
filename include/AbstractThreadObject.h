#pragma once

namespace std { class thread; }
namespace std { class mutex; }

class AbstractThreadObject {
public:
	AbstractThreadObject();
	virtual ~AbstractThreadObject();

	void start(void);
	void stop(bool _join);
	void join(void);

protected:
	bool checkContinueRunning(void);
	virtual void startWorker(std::thread*& _thread) = 0;

private:
	bool			m_isStopping;
	std::thread* m_thread;
	std::mutex* m_mutex;

	AbstractThreadObject(AbstractThreadObject&) = delete;
	AbstractThreadObject& operator = (AbstractThreadObject&) = delete;
};