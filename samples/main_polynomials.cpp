#include "SortedList.h"
#include "Polynomial.h"
#include <iostream>
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
		"4. The multiplication sign can be omitted.\n";
    std::cout << "Enter the polynomial: ";
    std::getline(std::cin, input_string);
    Polynomial p(input_string);
	
	std::cout << "Result: " << calculate_interactive(p) << std::endl;
}


int main() {
    try {
        input_and_calculate();
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
	return 0;
}