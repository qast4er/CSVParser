#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include "print_tuple.h"
#include "CSVParser.h"

int main(int argc, char* argv[]) {

	try {
		std::ifstream file(argv[1]);
		CSVParser<int, std::string> parser(file, atoi(argv[2]));
		for (auto rs : parser) {
			std::cout << rs << std::endl;
		}
	}
	catch (std::exception err) {
		std::cout << err.what() << std::endl;
	}

	return 0;
}
