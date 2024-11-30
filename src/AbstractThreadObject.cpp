#include "AbstractThreadObject.h"

#include <thread>
#include <mutex>

AbstractThreadObject::AbstractThreadObject() : m_isStopping(false), m_thread(nullptr), m_mutex(nullptr) {}

AbstractThreadObject::~AbstractThreadObject() {
	stop(true);
}

void AbstractThreadObject::start(void) {
	if (m_thread) return;
	m_isStopping = false;
	m_mutex = new std::mutex;
	startWorker(m_thread); 
}

void AbstractThreadObject::stop(bool _join) {
	if (m_thread == nullptr) return;

	{
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_isStopping = true;
	}

	// Aguarda a thread terminar, se solicitado.
	if (_join) join();
}
/*
void AbstractThreadObject::stop(bool _join) {
	if (m_thread == nullptr) return;
	
	m_mutex->lock();
	m_isStopping = true;
	m_mutex->unlock();
	
	if (_join) join();
}
*/
void AbstractThreadObject::join(void) {
	if (m_thread && m_thread->joinable()) {
		m_thread->join();
		delete m_thread;
		m_thread = nullptr;
	}
}

/*
void AbstractThreadObject::join(void) {
	if (m_thread == nullptr) return;

	m_thread->join();
	delete m_thread;
	delete m_mutex;
	m_thread = nullptr;
	m_mutex = nullptr;
}*/

bool AbstractThreadObject::checkContinueRunning(void) {
    if (m_mutex == nullptr) return false;
    std::lock_guard<std::mutex> lock(*m_mutex);
    return !m_isStopping;
}


/*
bool AbstractThreadObject::checkContinueRunning(void) {
	if (m_mutex == nullptr) return false;
	bool ret = false;
	m_mutex->lock();
	ret = !m_isStopping;
	m_mutex->unlock();
	return ret;
}
*/
