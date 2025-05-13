#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Window.H>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <tuple>
#include <vector>

#define foreach_line(__filename, __line, __body)                                                                            \
	std::ifstream __file(filename);                                                                                         \
	std::string __line;                                                                                                     \
	if (__file.is_open()) {                                                                                                 \
		while (std::getline(__file, __line)) {                                                                              \
			if (__line.empty()) continue;                                                                                   \
			__body                                                                                                          \
		};                                                                                                                  \
		__file.close();                                                                                                     \
	}

const float UPDATE_INTERVAL = 1.0;

#define LOG(x) std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl
template <typename T> using list = std::vector<T>;

typedef std::chrono::time_point<std::chrono::system_clock> time_point;

typedef struct Task {
	Fl_Box *box;
	Fl_Button *del;
	time_point start;
} Task;

struct FileData {
	std::string filename;
	std::time_t *last_modified;
	std::function<void()> onfilechange;
};

bool is_task_completed(const char *start) {
	const char *end = start + strlen(start);
	return std::find_if_not(start, end, [](char c) { return c == '-'; }) == end;
}

void save_tasks(const list<Task> *tasks, const char *filename) {
	std::ofstream file(filename);
	for (const auto &task : *tasks) {
		file << task.box->label() << '\n' << std::endl;
	}
	file.close();
}

std::_Put_time<char> get_timestamp(const time_point &time) {
	time_t current = std::chrono::system_clock::to_time_t(time);
	return std::put_time(std::localtime(&current), "%H:%M:%S");
}

bool has_file_changed(const std::string &filename, std::time_t &last_modified) {
	struct stat file_info;
	if (stat(filename.c_str(), &file_info) != 0) {
		// Handle error (e.g., file not found)
		return false;
	}

	if (file_info.st_mtime > last_modified) {
		last_modified = file_info.st_mtime;
		return true;
	}
	return false;
}

void add_task(list<Task> *tasks, Fl_Window *window, std::string task, const std::string &filename) {
	task.erase(0, task.find_first_not_of(' '));
	task.erase(task.find_last_not_of(' ') + 1);

	if (task.empty()) return;

	window->begin();

	int y = 50;
	if (tasks->size() > 0) {
		Fl_Box *b = tasks->back().box;
		y = is_task_completed(b->label()) ? b->y() + 10 : b->y() + 40;
	}

	const auto now = std::chrono::system_clock::now();
	Fl_Box *newbox = new Fl_Box(10, y, 600, 30, strdup(task.c_str()));
	Fl_Button *del = new Fl_Button(600, y, 30, 30, "X");

	auto data = new std::pair<list<Task> *, const std::string &>(tasks, filename);

	del->callback(
		[](Fl_Widget *w, void *data) {
			const auto [tasks, filename] = *static_cast<std::pair<list<Task> *, const std::string &> *>(data);

			bool found = false;
			for (auto it = tasks->begin(); it != tasks->end(); it++) {
				Fl_Box *b = it->box;
				Fl_Button *del = it->del;

				if (found && it != tasks->begin()) {
					b->position(b->x(), b->y() - 30);
					del->position(del->x(), del->y() - 30);
					continue;
				}

				if (it->del == w) {
					std::string s = b->label();
					b->copy_label(std::string(s.size(), '-').c_str());
					found = true;

					const int completed = std::count_if(tasks->begin(), tasks->end(),
														[](const Task &t) { return is_task_completed(t.box->label()); });

					const time_point now = std::chrono::system_clock::now();
					const int seconds = std::chrono::duration_cast<std::chrono::seconds>(now - it->start).count();

					std::string time_taken;
					if (seconds > 60) {
						time_taken = std::to_string(seconds / 60) + " minutes";
						if (seconds % 60 > 0) {
							time_taken += " and " + std::to_string(seconds % 60) + " seconds";
						} else {
							time_taken += " seconds";
						}
					} else {
						time_taken = std::to_string(seconds) + " seconds";
					}

					LOG("Completed: \"" << s << "\" at " << get_timestamp(it->start) << ", took a total of " << time_taken);
					LOG("Tasks completed: " << std::to_string(completed));
				}
			}

			save_tasks(tasks, filename.c_str());

			w->hide();

			delete static_cast<std::pair<list<Task> *, const std::string &> *>(data);
		},
		data);

	window->end();

	tasks->push_back({newbox, del, now});

	LOG("Added new task: \"" << task << "\" (" << std::to_string(tasks->size()) << ") at " << get_timestamp(now));

	window->redraw();
}

void file_check_callback(void *data) {
	auto filedata = static_cast<FileData *>(data);

	if (has_file_changed(filedata->filename, *filedata->last_modified)) {
		filedata->onfilechange();
	}

	Fl::repeat_timeout(UPDATE_INTERVAL, file_check_callback, data);
}

int run_app(std::string filename, const int argc, char **argv) {
	LOG("Setting up widgets...");
	Fl_Window *window = new Fl_Window(640, 360);
	Fl_Input *input = new Fl_Input(10, 10, 600, 30);

	LOG("Loading tasks...");
	std::time_t last_modified = 0;

	list<Task> tasks;

	foreach_line(filename, line, { add_task(&tasks, window, line, filename); });

	for (const auto &task : tasks) {
		if (is_task_completed(task.box->label())) task.del->hide();
	}

	LOG("Setting up events...");
	input->when(FL_WHEN_ENTER_KEY);

	auto data = std::make_tuple(&tasks, window, &filename);

	input->callback(
		[](Fl_Widget *w, void *data) {
			auto [tasks, window, filename] =
				*static_cast<std::tuple<list<Task> *, Fl_Window *, const std::string &> *>(data);
			Fl_Input *input = static_cast<Fl_Input *>(w);

			add_task(tasks, window, input->value(), filename);
			save_tasks(tasks, filename.c_str());

			input->value("");
		},
		&data);

	LOG("Loading widgets...");
	window->begin();
	input->show();
	window->end();

	LOG("Running file check callback...");

	struct stat file_info;
	if (stat(filename.c_str(), &file_info) == 0) {
		last_modified = file_info.st_mtime;
	}

	FileData file_data{
		filename,
		&last_modified,
		[filename, &tasks, window]() {
			LOG("File changed, reloading tasks...");
			foreach_line(filename, line, {
				if (std::find_if(tasks.begin(), tasks.end(), [line](const Task &t) { return t.box->label() == line; }) ==
					tasks.end()) {
					add_task(&tasks, window, line, filename);
				}
			});
		},
	};

	Fl::add_timeout(UPDATE_INTERVAL, file_check_callback, &file_data);

	LOG("Loading window...");
	window->show(argc, argv);

	return Fl::run();
}
