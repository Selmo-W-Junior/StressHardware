#pragma once

#include <list>
#include <string>
#include <atomic>
#include <thread>

class CPUstress;
class RAMstress;

class Application {
public:
    Application();
    virtual ~Application();

    void run(void);

private:
    bool anythingIsRunning(void) const;
    std::string readInput(void) const;
    std::string memoryToString(unsigned long long _memoryInBytes);

    void startCpu(void);
    void stopCpu(void);
    void setCpuThreads(const std::string& _command);

    void startRam(void);
    void stopRam(void);
    void setRamThreads(const std::string& _command);

    void stopAll(void);

    void startTimer(void); // Adicionado
    void setTimer(unsigned int durationSeconds); // Adicionado
    void cancelTimer(); // Adicionado

    unsigned int            m_cpuCores;
    unsigned long long      m_physicalMemoryTotal;
    unsigned long long      m_virtualMemoryTotal;

    bool                    m_isExiting;
    std::string             m_errorMessage;
    std::string             m_infoMessage;

    unsigned int            m_cpuToStart;
    std::list<CPUstress*>   m_cpu;

    unsigned int            m_ramToStart;
    std::list<RAMstress*>   m_ram;

    std::atomic<bool>       m_timeLimitActive; // Indica se o temporizador está ativo
    std::thread             m_timerThread;     // Thread para o temporizador
    int m_timerDuration = 30;
};
