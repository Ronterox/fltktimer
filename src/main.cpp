#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Window.H>
#include <iostream>

#define LOG(x) std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl

int main(const int argc, char **argv) {
	LOG("Setting up widgets...");

	Fl_Window *window = new Fl_Window(640, 360);
	Fl_Input *input = new Fl_Input(10, 10, 600, 30);
	Fl_Box *box = new Fl_Box(10, 50, 600, 300, "Hi mom!");
	box->labelsize(32);

	LOG("Setting up events...");
	input->when(FL_WHEN_CHANGED);
	input->callback(
		[](Fl_Widget *w, void *data) {
			Fl_Input *input = (Fl_Input *)w;
			Fl_Box *box = (Fl_Box *)data;

			box->label(input->value());
		},
		box);

	LOG("Loading widgets...");
	window->begin();
	input->show();
	box->show();
	window->end();

	LOG("Loading window...");
	window->show(argc, argv);
	return Fl::run();
}
