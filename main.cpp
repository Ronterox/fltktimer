#include <FL/Fl.H> 
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Progress.H>

#include <iostream>
#include <thread>
#include <chrono>

#define LOG(x) std::cout << __FILE__ << ':' << __LINE__ << ": " << x << std::endl

// Button callback
void button_callback(Fl_Widget *button, void *data)
{
  // Deactivate the button
  button->deactivate();
  Fl::check();

  // Make the progress bar
  Fl_Window *w = (Fl_Window *)data;
  w->begin();
  Fl_Progress *progress = new Fl_Progress(10, 50, 200, 30);
  progress->minimum(0);
  progress->maximum(1);
  progress->color(0x88888800);
  progress->selection_color(0x4444ff00);
  progress->labelcolor(FL_WHITE);
  w->end();

  // Computation loop
  for (int t = 1; t <= 500; ++t)
  {
    progress->value(t / 500.0);
    char percent[10];
    sprintf(percent, "%d%%", int((t / 500.0) * 100.0));
    progress->label(percent);
    Fl::check();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Cleanup
  w->remove(progress);
  delete (progress);
  button->activate();
  w->redraw();
}

// Main
int main()
{
  Fl_Window win(220, 90);
  Fl_Button button(10, 10, 100, 25, "Press");
  button.callback(button_callback, &win);
  win.resizable(win);
  win.show();
  return Fl::run();
}
