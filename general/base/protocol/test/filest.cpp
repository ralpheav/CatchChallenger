#include <iostream>
#include "../util/File.h"
#include "../util/Dir.h"

int main() {

	int num = 11;
	char data[] = "190";
	std::string value; 

	value = std::string("0x") + std::string(data) + std::to_string(num);
	std::cout << "value: "<< value << std::endl;

	CatchChallenger::File file;
	file.setFileName("data.txt");
	file.open(CatchChallenger::FileMode::WriteOnly);
	file.write(std::string("the number 0x07AB2 address."));
	file.write(std::string("\n"));
	file.write(std::string("Another line"));
	file.close();

	std::string out;
        file.open(CatchChallenger::FileMode::ReadOnly);
        out = file.readAll();
        file.close();

	std::cout << "file reading : " << out << std::endl;

	CatchChallenger::Dir directory;
	std::string dire = directory.appPath();

	std::cout << "app:"<< dire  << std::endl;

	return 0;

}
