#include <iostream>
#include <fstream>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string getInput(std::string& var, bool lower) {
	std::getline(std::cin, var);

	if (lower)
		std::transform(var.begin(), var.end(), var.begin(), tolower);

	return var; // Useless but minor abstraction
}

int main(int argc, char* argv[]) {
	json obfuscationOptions = {
		{"encAllStrings", false},
		{"encImportantStrings", false},
		{"noControlFlow", false},
		{"debugInfo", false},
		{"noCompressBS", false},
		{"key", "default_value"}
	};

	std::string input;
	std::fstream savedKey("key.txt", std::fstream::in | std::fstream::out);
	std::fstream output("output.txt", std::fstream::out);
	std::fstream scriptInput("input.txt", std::fstream::in);


	for (auto& option : obfuscationOptions.items()) {
		const std::string idx = option.key();
		
		std::cout << idx << " (y/n)" << std::endl;
		getInput(input, true);

		if (input == "y")
			obfuscationOptions[idx] = true;
	}

	std::cout << "Read key from key.txt? (y/n)" << std::endl;
	getInput(input, true);

	if (input == "y")
		// Ugly string construction/iteration mess
		obfuscationOptions["key"] = std::string((std::istreambuf_iterator<char>(savedKey)), std::istreambuf_iterator<char>());
	else {
		std::cout << "Please enter your key" << std::endl;
		getInput(input, false);

		obfuscationOptions["key"] = input;
	}

	std::cout << "Save key?" << std::endl;
	getInput(input, true);

	if (input == "y") {
		const std::string inputKey = std::string(obfuscationOptions["key"]);
		savedKey.write(inputKey.c_str(), strlen(inputKey.c_str()));
	}

	cpr::Response response = cpr::Post(
		cpr::Url{ "https://ibidk.herokuapp.com/obfuscate-key" },
		cpr::Body{ obfuscationOptions.dump() },
		cpr::Header{ {"Content-Type", "application/json"} }
	);

	output.write(response.text.c_str(), strlen(response.text.c_str()));

	std::cout << "Obfuscated, unless it errored. Check output.txt" << std::endl;
	std::cin.get();
	return 0;
}
