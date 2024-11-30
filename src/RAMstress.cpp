#include "RAMstress.h"
#include "OS.h"

#include <list>
#include <thread>

#define BLOCK_SIZE 128
#define RAM_LIMIT 70.

#if defined (WIN32)
#define MAX_DATA_SIZE 0xFFF0
#else
#define MAX_DATA_SIZE 0xFFFFFFF0
#endif

RAMstress::RAMstress(bool _start) {
	if (_start) start();
}

RAMstress::~RAMstress() {}

void RAMstress::startWorker(std::thread *& _thread) {
	_thread = new std::thread(&RAMstress::workerFunction, this);
}

void RAMstress::workerFunction(void) {
    std::list<unsigned long long*> data;

    try {
        // Aloca blocos de memória enquanto a execução continua e o limite não é atingido.
        while (checkContinueRunning() && os::physicalMemoryUsageInPercent() < RAM_LIMIT && data.size() < MAX_DATA_SIZE) {
            data.push_back(new unsigned long long[BLOCK_SIZE]);
        }

        // Enquanto `checkContinueRunning` for verdadeiro, realiza operações nos blocos.
        while (checkContinueRunning()) {
            for (auto e : data) {
                volatile unsigned long long sum = 0;
                for (size_t i = 0; i < BLOCK_SIZE; ++i) {
                    sum += e[i]; // Realiza leituras nos blocos.
                }
            }

            // Pausa breve para evitar consumo excessivo da CPU.
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    catch (const std::bad_alloc&) {
        // Evita falhas críticas caso a alocação exceda os limites do sistema.
    }

    // Libera todos os blocos de memória imediatamente após sair do loop principal.
    for (auto e : data) delete[] e;
    data.clear();
}

