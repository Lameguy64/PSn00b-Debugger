// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef MessageUI_h
#define MessageUI_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
extern void cb_MessageClear(Fl_Menu_*, void*);
#include <FL/Fl_Text_Display.H>

class MessageUI : public Fl_Double_Window {
  void _MessageUI();
public:
  MessageUI(int X, int Y, int W, int H, const char *L = 0);
  MessageUI(int W, int H, const char *L = 0);
  MessageUI();
  static Fl_Menu_Item menu_[];
  Fl_Text_Display *logDisplay;
  Fl_Text_Buffer *text; 
  void init_text();
};
#endif
