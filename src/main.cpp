#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Window.H>
#include <algorithm>
#include <iostream>
#include <vector>

#define LOG(x) std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl
template <typename T> using list = std::vector<T>;

typedef struct Task {
	Fl_Box *box;
	Fl_Button *del;
} Task;

int main(const int argc, char **argv) {
	LOG("Setting up widgets...");

	list<Task> tasks;

	Fl_Window *window = new Fl_Window(640, 360);
	Fl_Input *input = new Fl_Input(10, 10, 600, 30);

	LOG("Setting up events...");
	input->when(FL_WHEN_ENTER_KEY);

	auto data = std::make_pair(&tasks, window);

	input->callback(
		[](Fl_Widget *w, void *data) {
			auto [tasks, window] = *static_cast<std::pair<list<Task> *, Fl_Window *> *>(data);
			Fl_Input *input = static_cast<Fl_Input *>(w);

			std::string s = input->value();
			s.erase(0, s.find_first_not_of(' '));
			s.erase(s.find_last_not_of(' ') + 1);

			if (s.empty()) return;

			window->begin();

			const int y = tasks->size() > 0 ? tasks->back().box->y() + 40 : 50;

			Fl_Box *newbox = new Fl_Box(10, y, 600, 30, strdup(s.c_str()));
			Fl_Button *del = new Fl_Button(600, y, 30, 30, "X");

			del->callback(
				[]([[maybe_unused]] Fl_Widget *w, void *data) {
					list<Task> *tasks = static_cast<list<Task> *>(data);

					bool found = false;
					for (auto it = tasks->begin(); it != tasks->end(); it++) {
						Fl_Box *b = it->box;
						Fl_Button *del = it->del;

						if (found && it != tasks->begin()) {
							b->position(b->x(), b->y() - 20);
							del->position(del->x(), del->y() - 20);
							continue;
						}

						if (it->del == w) {
							std::string s = b->label();
							b->label(strdup(std::string(s.size(), '-').c_str()));
							found = true;

							const int completed = std::count_if(tasks->begin(), tasks->end(),
																[](const Task &t) { return t.box->label()[0] == '-'; });
							LOG("Tasks completed: " + std::to_string(completed));
						}
					}

					w->hide();
				},
				tasks);

			window->end();

			tasks->push_back({newbox, del});

			LOG("Added new task: " + s + " (" + std::to_string(tasks->size()) + ")");

			window->redraw();

			input->value("");
		},
		&data);

	LOG("Loading widgets...");
	window->begin();
	input->show();
	window->end();

	LOG("Loading window...");
	window->show(argc, argv);
	return Fl::run();
}
