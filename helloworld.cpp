#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Progress.H>

#include <string>
#include <thread>
#include <chrono>
#include <iostream>

int main(int argc, char** argv)
{
    Fl_Window* window = new Fl_Window(340, 180);
    Fl_Box* box = new Fl_Box(20, 40, 300, 100, "Hello, World!");
    Fl_Progress* progress = new Fl_Progress(20, 140, 300, 20);

    progress->minimum(0);
    progress->maximum(10);
    progress->color(0x88888800);
    progress->selection_color(0x4444ff00);
    progress->labelcolor(FL_WHITE);

    box->box(FL_UP_BOX);
    box->labelfont(FL_BOLD + FL_ITALIC);
    box->labelsize(36);
    box->labeltype(FL_SHADOW_LABEL);

    window->end();

    Fl::add_timeout(1.0, [](void* v) {
        Fl_Box* b = (Fl_Box*)v;
        for (size_t i = 1; i <= 10; i++)
        {
            b->label(std::string(i, '|').c_str());
            Fl::check();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }, box);

    Fl::add_timeout(.5, [](void* v) {
        Fl_Progress* p = (Fl_Progress*)v;
        for (uint32_t i = 1; i <= 10; i++)
        {
            char buf[4];
            sprintf(buf, "%d%%", i * 10);
            p->label(buf);
            p->value(i);
            Fl::check();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }, progress);

    window->show(argc, argv);

    return Fl::run();
}