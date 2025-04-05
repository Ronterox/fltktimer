#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Window.H>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>

#define LOG(x) std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl
template <typename T> using list = std::vector<T>;

int main(const int argc, char **argv) {
	LOG("Setting up widgets...");

	list<Fl_Box *> boxes;

	Fl_Window *window = new Fl_Window(640, 360);
	Fl_Input *input = new Fl_Input(10, 10, 600, 30);

	LOG("Setting up events...");
	input->when(FL_WHEN_ENTER_KEY);

	auto data = std::make_pair(&boxes, window);

	input->callback(
		[](Fl_Widget *w, void *data) {
			auto [boxes, window] =
				*static_cast<std::pair<list<Fl_Box *> *, Fl_Window *> *>(data);
			Fl_Input *input = static_cast<Fl_Input *>(w);

			std::string s = input->value();
			s.erase(0, s.find_first_not_of(' '));
			s.erase(s.find_last_not_of(' ') + 1);

			if (s.empty()) return;

			window->begin();

			const int y = 50 + boxes->size() * 40;

			Fl_Box *newbox = new Fl_Box(10, y, 600, 30, strdup(s.c_str()));
			Fl_Button *del = new Fl_Button(600, y, 30, 30, "X");

			del->callback(
				[]([[maybe_unused]] Fl_Widget *w, void *data) {
					Fl_Box *box = static_cast<Fl_Box *>(data);
					std::string s = box->label();

					box->label(strdup(std::string(s.size(), '-').c_str()));

					w->hide();
				},
				newbox);

			window->end();

			boxes->push_back(newbox);

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
