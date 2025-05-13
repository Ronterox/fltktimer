#include "app.cpp"
#include <cstdio>
#include <sys/stat.h>

#define MAIN_FILENAME "tasks"
#define TASKLIST_KWORD "tasklist"
#define starts_with(str, prefix) (str.rfind(prefix, 0) == 0)

int main(int argc, char *argv[]) {
	std::string filename = argc > 1 ? argv[argc - 1] : MAIN_FILENAME;

	if (filename == MAIN_FILENAME) {
		if (mkdir(TASKLIST_KWORD, 0777) == -1 && errno != EEXIST) {
			std::cerr << "Error creating tasks directory: " TASKLIST_KWORD << std::endl;
			return 1;
		}

		const std::string BASIC_FILE = std::string(TASKLIST_KWORD) + "/tasks";

		list<std::string> files = {BASIC_FILE};
		std::ofstream file(BASIC_FILE);

		foreach_line(filename, line, {
			if (starts_with(line, TASKLIST_KWORD)) {
				file.close();

				LOG("Creating file: " << line);
				std::string tasklist_name = line.substr(line.find_first_of(' ') + 1);
				file = std::ofstream(std::string(TASKLIST_KWORD) + "/" + tasklist_name);

				files.push_back(std::string(TASKLIST_KWORD) + "/" + tasklist_name);
			} else {
				file << line << '\n' << std::endl;
			}
		});

		file.close();

		std::ostringstream command;
		for (const auto &filename : files) {
			for (int i = 0; i < argc; i++) command << '"' << argv[i] << "\" ";
			command << '"' << filename << "\" & ";
		}

		LOG("Running all tasklists: " << command.str());
		if (system(command.str().c_str()) != 0) {
			std::cerr << "Failed to start processes" << std::endl;
		}

		return 0;
	}

	return run_app(filename, argc, argv);
}
