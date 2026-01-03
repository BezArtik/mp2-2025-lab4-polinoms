#include "SortedList.h"
#include "Polynomial.h"
#include <iostream>
#include <chrono>
#include <string>

double calculate_interactive(const Polynomial& poly) {
	SortedList<char> variables = poly.get_variables();

	SortedList<Polynomial::VariableValue, Polynomial::VariableValueCompare> values;
	for (char var : variables) {
		std::cout << "Enter value for " << var << ": ";
		double val;
		std::cin >> val;
		values.insert({ var, val });
	}
	return poly.calculate(values);
}


void input_and_calculate() {
    std::string input_string;
	std::cout << "============RULES=============\n" <<
		"1. A polynomial has an arbitrary number of variables.\n" <<
		"2. The order of input of terms and variables can be arbitrary.\n" <<
		"3. Degrees of variables are integers\n" <<
		"4. The polynomial can only be entered as: a1 * x1^b1 * x2^b2 * x3^b3 * ...  + ...\n";
    std::cout << "Enter the polynomial: ";
    std::getline(std::cin, input_string);
    Polynomial p(input_string);
	
	std::cout << "Result: " << calculate_interactive(p) << std::endl;
}


int main() {
	//while (true) {
	//	const auto start{ std::chrono::steady_clock::now() };
	//	Polynomial p;
	//	for (int i = 0; i < 500; ++i) {
	//		p += Polynomial::Monom{ 1.0, {{'x',i},{'y',i + 1},{'z',i+2}}};
	//	}
	//	const auto finish{ std::chrono::steady_clock::now() };
	//	const std::chrono::duration<double> elapsed_seconds{ finish - start };
	//	std::cout << elapsed_seconds.count() << "s\n";
	//}

    try {
        input_and_calculate();
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
	return 0;
}