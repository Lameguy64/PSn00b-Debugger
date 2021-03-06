// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "BookmarksUI.h"
BookmarksUI::BookmarksUI(int X, int Y, int W, int H, const char *L)
  : Fl_Double_Window(X, Y, W, H, L) {
  _BookmarksUI();
}

BookmarksUI::BookmarksUI(int W, int H, const char *L)
  : Fl_Double_Window(0, 0, W, H, L) {
  clear_flag(16);
  _BookmarksUI();
}

BookmarksUI::BookmarksUI()
  : Fl_Double_Window(0, 0, 200, 300, "Bookmarks") {
  clear_flag(16);
  _BookmarksUI();
}

void BookmarksUI::_BookmarksUI() {
this->box(FL_FLAT_BOX);
this->color(FL_BACKGROUND_COLOR);
this->selection_color(FL_BACKGROUND_COLOR);
this->labeltype(FL_NO_LABEL);
this->labelfont(0);
this->labelsize(14);
this->labelcolor(FL_FOREGROUND_COLOR);
this->align(Fl_Align(FL_ALIGN_TOP));
this->when(FL_WHEN_RELEASE);
{ bookmarkTable = new Fl_Bookmark_Table(0, 0, 200, 300);
  bookmarkTable->box(FL_THIN_DOWN_FRAME);
  bookmarkTable->color(FL_BACKGROUND_COLOR);
  bookmarkTable->selection_color(FL_SELECTION_COLOR);
  bookmarkTable->labeltype(FL_NORMAL_LABEL);
  bookmarkTable->labelfont(0);
  bookmarkTable->labelsize(12);
  bookmarkTable->labelcolor(FL_FOREGROUND_COLOR);
  bookmarkTable->callback((Fl_Callback*)cb_BookmarksTable);
  bookmarkTable->align(Fl_Align(FL_ALIGN_TOP));
  bookmarkTable->when(FL_WHEN_RELEASE);
  bookmarkTable->end();
  Fl_Group::current()->resizable(bookmarkTable);
} // Fl_Bookmark_Table* bookmarkTable
end();
}
