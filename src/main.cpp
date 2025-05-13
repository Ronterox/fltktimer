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

		const std::string DIR_PATH = std::string(TASKLIST_KWORD);
		const std::string BASIC_FILE = DIR_PATH + "/tasks";

		list<std::string> files = {};
		std::ofstream file(BASIC_FILE);

		foreach_line(filename, line, {
			if (starts_with(line, TASKLIST_KWORD)) {
				file.close();

				LOG("Creating file: " << line);
				std::string tasklist_name = line.substr(DIR_PATH.size() + 1);
				std::string tasklist_path = DIR_PATH + "/" + tasklist_name;

				file = std::ofstream(tasklist_path);
				files.push_back(tasklist_path);
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
		// if (system(command.str().c_str()) != 0) {
		// 	std::cerr << "Failed to start processes" << std::endl;
		// }

		int ret = run_app(BASIC_FILE, argc, argv);

		// TODO: Do this when sub files are modified callback
		if (ret == 0) {
			std::ostringstream tasks;
			files.insert(files.begin(), BASIC_FILE);

			for (const auto &filename : files) {
				tasks << "\n" TASKLIST_KWORD " " << filename.substr(DIR_PATH.size() + 1) << '\n' << std::endl;
				foreach_line(filename, line, {
					// if (is_task_completed(line.c_str())) continue;
					tasks << line << '\n' << std::endl;
				});
			}

			std::ofstream file(filename);
			file << tasks.str();
			file.close();

		} else {
			std::cerr << "Failed to run/close" << std::endl;
		}

		return ret;
	}

	return run_app(filename, argc, argv);
}
