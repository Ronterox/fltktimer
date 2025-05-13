#include "app.cpp"
#include <sys/wait.h>
#include <unistd.h>

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

		list<std::string> files = {};
		std::ofstream file;

		foreach_line(filename, line, {
			if (starts_with(line, TASKLIST_KWORD)) {
				file.close();

				LOG("Creating file: " << line);
				std::string tasklist_name = line.substr(DIR_PATH.size() + 1);
				std::string tasklist_path = DIR_PATH + "/" + tasklist_name;

				file = std::ofstream(tasklist_path);
				files.push_back(tasklist_path);
			} else if (file.is_open()) {
				file << line << '\n' << std::endl;
			}
		});

		file.close();

		for (const auto &filename : files) {
			pid_t pid = fork();

			if (pid == 0) { // Child process
				// Build arguments: [program, original_args..., file, nullptr]
				list<char *> args;
				for (int i = 0; i < argc - 1; i++) args.push_back(argv[i]);

				args.push_back(const_cast<char *>(filename.c_str())); // Add file
				// args.push_back(nullptr);							  // NULL-terminate

				execv(argv[0], args.data());

				// Only reached if execv fails
				std::cerr << "execv failed for " << filename << std::endl;
				_exit(1);
			} else if (pid < 0) {
				std::cerr << "fork failed for " << filename << std::endl;
			}
			// Parent continues
		}

		while (wait(nullptr) != -1) {
			LOG("Saving to main file: " << filename);

			std::ostringstream tasks;
			for (const auto &filename : files) {
				tasks << "\n" TASKLIST_KWORD " " << filename.substr(DIR_PATH.size() + 1) << '\n' << std::endl;
				foreach_line(filename, line, {
					// if (is_task_completed(line.c_str())) continue;
					tasks << line << '\n' << std::endl;
				});
			}

			file = std::ofstream(filename);
			file << tasks.str();
			file.close();
		}

		return 0;
	}

	return run_app(filename, argc, argv);
}
