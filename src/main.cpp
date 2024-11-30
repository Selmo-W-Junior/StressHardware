#include "Application.h"
#include <iostream>

int main()
{
	try {
		Application app;
		app.run();
	}
	catch (const std::exception& _e) {
		std::cout << std::endl << "[FATAL ERROR]: " << _e.what() << std::endl;
		char * tmp = new char[128];
		std::cin >> tmp;
		delete[] tmp;
	}
	catch (...) {
		std::cout << std::endl << "[FATAL ERROR]: Unknown error" << std::endl;
		char * tmp = new char[128];
		std::cin >> tmp;
		delete[] tmp;
	}
	return 0;
}
