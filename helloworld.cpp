#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <string>
#include <thread>
#include <chrono>

int main(int argc, char **argv)
{
    Fl_Window *window = new Fl_Window(340, 180);
    Fl_Box *box = new Fl_Box(20, 40, 300, 100, "Hello, World!");
    box->box(FL_UP_BOX);
    box->labelfont(FL_BOLD + FL_ITALIC);
    box->labelsize(36);
    box->labeltype(FL_SHADOW_LABEL);

    Fl::add_timeout(1.0, [](void *v) {
        Fl_Box *b = (Fl_Box *)v;
        for (size_t i = 0; i < 10; i++)
        {
            b->label(std::string(i, '|').c_str());
            Fl::check();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    },box);

    window->end();
    window->show(argc, argv);
    return Fl::run();
}