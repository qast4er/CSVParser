//обалденная, прикольная тема!
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include "tuple_print.h"
#include "CSVparser.h"


int main(int argc, char* argv[]) {
	/*std::ifstream file("test.csv");

	CSVParser<int, std :: string> parser(file, 0);

	for (std :: tuple <int, std :: string> rs : parser) {
		std::cout << rs << std::endl;
	}*/

	std::tuple<double, char, std::string> k = std::make_tuple(3.8, 'A', "Lisa Simpson");
	std::cout << k;

	return 0;
}