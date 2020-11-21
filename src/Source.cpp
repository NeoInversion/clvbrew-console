#include <iostream>
#include <fstream>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(int argc, char* argv[]) {
	json obfuscationOptions = {
		{"encAllStrings", false},
		{"encImportantStrings", false},
		{"noControlFlow", false},
		{"debugInfo", false},
		{"noCompressBS", false},
	};

	std::string input;
	std::fstream settings("settings.json", std::fstream::in | std::fstream::out);
	std::fstream output("output.txt", std::fstream::out);
	std::fstream scriptInput("input.txt", std::fstream::in);

	std::cout << "Use saved settings? (y/n)" << std::endl;

	{
		std::getline(std::cin, input);
		std::transform(input.begin(), input.end(), input.begin(), tolower);
		if (input == "y") {
			json savedSettings = json::parse(settings);

			if (savedSettings["valid"] == true) {
				for (auto& i : savedSettings.items())
					obfuscationOptions[i.key()] = i.value();

				std::cout << "Please enter your script" << std::endl;
				std::getline(std::cin, input);
				obfuscationOptions["script"] = input;
			}
			else
				std::cout << "Settings file was not validated. Resuming manual input" << std::endl;
		}
		else {
			for (auto& i : obfuscationOptions.items()) {
				std::cout << i.key() << " (y/n)" << std::endl;
				std::getline(std::cin, input);
				std::transform(input.begin(), input.end(), input.begin(), tolower);

				if (input == "y") {
					obfuscationOptions[i.key()] = true;
				}
			}

			std::cout << "Read key from settings.json? (y/n)" << std::endl;

			std::getline(std::cin, input);
			std::transform(input.begin(), input.end(), input.begin(), tolower);

			if (input == "y")
				obfuscationOptions["key"] = json::parse(settings)["key"];
			else {
				std::cout << "Please enter your key" << std::endl;
				std::getline(std::cin, input);
				obfuscationOptions["key"] = input;
			}

			std::cout << "Read script from input.txt? (y/n)" << std::endl;
			std::getline(std::cin, input);
			std::transform(input.begin(), input.end(), input.begin(), tolower);

			if (input == "y")
				obfuscationOptions["script"] << scriptInput;
			else {
				std::cout << "Please enter your script" << std::endl;
				std::getline(std::cin, input);
				obfuscationOptions["script"] = input;
			}

			std::cout << "Save settings?" << std::endl;

			std::getline(std::cin, input);
			std::transform(input.begin(), input.end(), input.begin(), tolower);
			if (input == "y") {
				obfuscationOptions["valid"] = true;
				settings.write(obfuscationOptions.dump().c_str(), strlen(obfuscationOptions.dump().c_str()));
			}
		}
	}

	cpr::Response response = cpr::Post(
		cpr::Url { "https://ibidk.herokuapp.com/obfuscate-key" },
		cpr::Body {obfuscationOptions.dump()},
		cpr::Header {{"Content-Type", "application/json"}}
	);
	
	output.write(response.text.c_str(), strlen(response.text.c_str()));

	std::cout << "Obfuscated! Check output.txt" << std::endl;

	std::cin.get();
	return 0;
}
