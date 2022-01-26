#include "Game/Application.hpp"

#include <stdexcept>
#include <iostream>

int WinMain() {
	try {
		Application app;
		app.run();
	}
	catch (std::exception& e) {
		std::cout << "\nEXCEPTION: " << e.what() << std::endl;
	}
}