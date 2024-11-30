#include "Application.h"
#include "CPUstress.h"
#include "RAMstress.h"
#include "OS.h"

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

template <class T> T toNumber(const std::string& _string, bool& _failed) {
	std::stringstream ss(_string);
	T v;
	ss >> v;
	_failed = false;
	if (ss.fail()) { _failed = true; }
	std::string rest;
	ss >> rest;
	if (!rest.empty()) { _failed = true; }
	return v;
}

// ####################################################################################################

Application::Application() :
	m_isExiting(false),
	m_cpuToStart(20),
	m_ramToStart(5),
	m_cpuCores(0),
	m_physicalMemoryTotal(0),
	m_virtualMemoryTotal(0),
	m_timeLimitActive(false) {}

Application::~Application() {}

// Função para iniciar o temporizador
void Application::startTimer() {
	if (m_timeLimitActive) {
		m_errorMessage = "O temporizador já está em execução!";
		return;
	}

	m_timeLimitActive = true;

	// Mensagem de início do temporizador
	std::cout << "Temporizador iniciado com " << m_timerDuration << " segundos." << std::endl;

	m_timerThread = std::thread([this]() {
		std::this_thread::sleep_for(std::chrono::seconds(m_timerDuration));

		if (m_timeLimitActive) {
			m_timeLimitActive = false;
			stopAll();  
			m_infoMessage = "Estresse encerrado após o tempo limite de " + std::to_string(m_timerDuration) + " segundos.";

			m_errorMessage.clear();
		}
		});

	m_timerThread.detach();
}





void Application::setTimer(unsigned int durationSeconds) {
	m_timerDuration = durationSeconds;  // Atualiza a duração do temporizador
}

void Application::cancelTimer() {
    if (!m_timeLimitActive) {
        m_errorMessage = "Nenhum temporizador está em execução para cancelar.";
        return;
    }
    m_timeLimitActive = false;
    m_infoMessage = "Temporizador cancelado.";
}

void Application::run(void) {
	m_isExiting = false;

	m_cpuCores = std::thread::hardware_concurrency();
	m_physicalMemoryTotal = os::getTotalPhysicalMemory();
	m_virtualMemoryTotal = os::getTotalVirtualMemory();

	if (m_cpuCores > 0) {
		m_cpuToStart = m_cpuCores * 2;
		m_ramToStart = m_cpuCores / 2;
	}
	if (m_ramToStart == 0) m_ramToStart = 1;

	while (!m_isExiting) {
		os::clearConsole();
		os::printBuildInformation();

		// Exibindo o tempo de execução
		std::cout << std::endl << "Informacoes da maquina:" << std::endl;
		if (m_timeLimitActive == true) {
			std::cout << "\tTempo de Execucao EM EXECUCAO:  " << m_timerDuration << std::endl;
		}
		else {
			std::cout << "\tTempo de Execucao PARADO:  " << m_timerDuration << std::endl;
		}
		
		if (m_cpuCores == 0) std::cout << "\tNumero de nucleos da CPU:   <Falha ao detectar>" << std::endl;
		else std::cout << "\tNumero de nucleos da CPU:   " << m_cpuCores << std::endl;

		if (m_physicalMemoryTotal > 0) std::cout << "\tMemoria fisica total: " << memoryToString(m_physicalMemoryTotal) << std::endl;
		else std::cout << "\tMemoria fisica total: <Falha ao detectar>" << std::endl;

		if (m_cpu.empty()) std::cout << "\tEstresse da CPU:            PARADO (" << m_cpuToStart << " threads para iniciar)" << std::endl;
		else std::cout << "\tEstresse da CPU:            EM EXECUCAO (" << m_cpu.size() << " threads)" << std::endl;

		if (m_ram.empty()) std::cout << "\tEstresse da RAM:            PARADO (" << m_ramToStart << " threads para iniciar)" << std::endl;
		else std::cout << "\tEstresse da RAM:            EM EXECUCAO (" << m_ram.size() << " threads)" << std::endl;

		// Mostrar opções
		std::cout << std::endl << "Opcoes:" << std::endl;

		if (m_cpu.empty()) {
			std::cout << "\tcpu-t <numero>  Define o numero de threads que devem ser iniciadas para o estresse da CPU" << std::endl;
			std::cout << "\tcpu" << std::endl;
			std::cout << "\t  -ou-" << std::endl;
			std::cout << "\tcpu-start       Inicia o estresse da CPU" << std::endl;
		}
		else {
			std::cout << "\tcpu" << std::endl;
			std::cout << "\tou" << std::endl;
			std::cout << "\tcpu-stop        Para o estresse da CPU" << std::endl;
		}

		if (m_ram.empty()) {
			std::cout << "\tram-t <numero>  Define o numero de threads que devem ser iniciadas para o estresse da RAM" << std::endl;
			std::cout << "\tram" << std::endl;
			std::cout << "\t  -ou" << std::endl;
			std::cout << "\tram-start       Inicia o estresse da RAM" << std::endl;
		}
		else {
			std::cout << "\tram" << std::endl;
			std::cout << "\t   -ou-" << std::endl;
			std::cout << "\tram-stop        Para o estresse da RAM" << std::endl;
		}

		// Opções de temporizador
		std::cout << "\ttimer-start Inicia o temporarizador." << std::endl;
		std::cout << "\ttimer-set <tempo>    Define o tempo limite (em segundos) para executar os testes." << std::endl;
		std::cout << "\ttimer-cancel           Cancela o temporizador ativo (se houver)" << std::endl;

		if (anythingIsRunning()) {
			std::cout << "\tstop            Para qualquer estresse em execução" << std::endl << std::endl;
		}

		std::cout << "\texit            Sair da aplicacao" << std::endl << std::endl;


		// Mensagem de erro e mensagem de informações
		if (!m_errorMessage.empty()) std::cout << "[error]: " << m_errorMessage << std::endl << std::endl;
		m_errorMessage.clear();
		if (!m_infoMessage.empty()) std::cout << "[info]: " << m_infoMessage << std::endl << std::endl;
		m_infoMessage.clear();

		// Verificar se o temporizador expirou
		if (m_timeLimitActive) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}

		// Leitura da entrada
		std::string inp = readInput();
		if (inp == "exit") {
			m_isExiting = true;
			stopAll();
		}
		else if (inp == "stop") stopAll();
		else if ((inp == "cpu-start" && m_cpu.empty()) || (inp == "cpu" && m_cpu.empty())) {
			if (!m_timeLimitActive) {
				setTimer(m_timerDuration); // Define o temporizador com a duração padrão
				startTimer(); // Inicia o temporizador
			}
			startCpu(); // Inicia o estresse da CPU
		}
		else if ((inp == "cpu-stop" && !m_cpu.empty()) || (inp == "cpu" && !m_cpu.empty())) stopCpu();
		else if (inp.find("cpu-t ") != std::string::npos) setCpuThreads(inp);
		else if ((inp == "ram-start" && m_ram.empty()) || (inp == "ram" && m_ram.empty())) {
			if (!m_timeLimitActive) {
				setTimer(m_timerDuration); // Define o temporizador com a duração padrão
				startTimer(); // Inicia o temporizador
			}
			startRam(); // Inicia o estresse da RAM
		}
		else if ((inp == "ram-stop" && !m_ram.empty()) || (inp == "ram" && !m_ram.empty())) stopRam();
		else if (inp.find("timer-set ") == 0) {
			if (inp.length() <= 10) {
				m_errorMessage = "Uso inválido de 'timer-set': Esperado 'timer-set <tempo>'.";
				continue;
			}
			try {
				std::string timerValueStr = inp.substr(10); // Use 10, pois 'timer-set ' tem 10 caracteres
				int timerValue = std::stoi(timerValueStr);
				if (timerValue > 0) {
					setTimer(timerValue);
				}
				else {
					m_errorMessage = "O valor do temporizador deve ser maior que zero.";
				}
			}
			catch (const std::exception& e) {
				m_errorMessage = std::string("Erro ao processar 'timer-set': ") + e.what();
			}
		}
		else if (inp == "timer-start") startTimer();
		else if (inp == "timer-cancel") cancelTimer();
		else m_errorMessage = "Comando desconhecido, revisar os comandos citados acima!";
	}
}


// ####################################################################################################

bool Application::anythingIsRunning(void) const {
	return !m_cpu.empty() || !m_ram.empty();
}

std::string Application::readInput(void) const {
	std::cout << "Entrada: ";
	std::string inp;
	std::getline(std::cin, inp);
	if (inp.empty()) return readInput();
	else return inp;
}

std::string Application::memoryToString(unsigned long long _memoryInBytes) {
	enum memoryLevel {
		KB = 0,
		MB = 1,
		GB = 2,
		TB = 3,
		PB = 4
	};
	memoryLevel currentLevel = KB;
	double res = _memoryInBytes / 1024.;
	while (currentLevel < PB && res > 1024.) {
		res /= 1024.;
		currentLevel = (memoryLevel)(((int)currentLevel) + 1);
	}
	std::ostringstream retOut;
	retOut.precision(2);
	retOut << std::fixed << res;
	std::string ret = retOut.str();
	switch (currentLevel)
	{
	case KB: ret.append(" KB"); break;
	case MB: ret.append(" MB"); break;
	case GB: ret.append(" GB"); break;
	case TB: ret.append(" TB"); break;
	case PB: ret.append(" PB"); break;
	default: ret.append(" Unidade desconhecida"); break;
	}
	return ret;
}

void Application::startCpu(void) {
	for (unsigned int i = 0; i < m_cpuToStart; i++) m_cpu.push_back(new CPUstress(true));
	m_infoMessage = "CPU stress iniciando com " + std::to_string(m_cpu.size()) + " threads";
}

void Application::stopCpu(void) {
	for (auto c : m_cpu) {
		c->stop(true);
		delete c;
	}
	m_cpu.clear();
	m_infoMessage = "CPU stress finalizado";
}

void Application::setCpuThreads(const std::string& _command) {
	auto ix = _command.find(" ");
	if (ix == std::string::npos) {
		m_errorMessage = "Formato de numero invalido para \"cpu-t <numero>\": <numero> está ausente";
		return;
	}
	std::string nString = _command.substr(ix);
	bool failed = false;
	unsigned int n = toNumber<unsigned int>(nString, failed);
	if (failed) {
		m_errorMessage = "Formato de numero invalido para \"cpu-t <numero>\": Esperado numero positivo";
		return;
	}
	else if (n == 0) {
		m_errorMessage = "Formato de numero invalido para \"cpu-t <numero>\": <numero> deve ser maior que 0";
		return;
	}
	else m_cpuToStart = n;
}

void Application::startRam(void) { 
	for (unsigned int i = 0; i < m_ramToStart; i++) m_ram.push_back(new RAMstress(true));
	m_infoMessage = "RAM stress iniciado com " + std::to_string(m_ram.size()) + " threads";
}
void Application::stopRam(void) {
	std::cout << "Parando o RAM stress" << std::endl;
	for (auto& r : m_ram) r->stop(false);
	m_ram.clear();
	m_infoMessage = "RAM stress finalizado";
}
/*
void Application::stopRam(void) {
	std::cout << "Parando o RAM stress" << std::endl;
	for (auto r : m_ram) r->stop(false);
	for (auto r : m_ram) {
		r->join();
		delete r;
	}
	m_ram.clear();
	m_infoMessage = "RAM stress finalizado";
}
*/

void Application::setRamThreads(const std::string& _command) {
	auto ix = _command.find(" ");
	if (ix == std::string::npos) {
		m_errorMessage = "Formato de número inválido para \"ram-t <número>\": <número> está ausente";
		return;
	}
	std::string nString = _command.substr(ix);
	bool failed = false;
	unsigned int n = toNumber<unsigned int>(nString, failed);
	if (failed) {
		m_errorMessage = "Formato de número inválido para \"ram-t <número>\": Esperado número positivo";
		return;
	}
	else if (n == 0) {
		m_errorMessage = "Formato de número inválido para \"ram-t <número>\": <número> deve ser maior que 0";
		return;
	}
	else m_ramToStart = n;
}

void Application::stopAll(void) {
	if (!anythingIsRunning() && !m_timeLimitActive) return; // Verifica se há algo para parar

	// Finaliza o estresse da RAM
	for (auto r : m_ram) r->stop(false);
	for (auto r : m_ram) {
		r->join();
		delete r;
	}
	m_ram.clear();

	// Finaliza o estresse da CPU
	for (auto c : m_cpu) c->stop(false);
	for (auto c : m_cpu) {
		c->join();
		delete c;
	}
	m_cpu.clear();

	// Desativa o temporizador
	if (m_timeLimitActive) {
		m_timeLimitActive = false;
		m_infoMessage = "Temporizador cancelado.";
	}

	m_infoMessage = "Todos os testes de estresse foram finalizados.";
}
