/* 
 * File:   Fl_Source_Browser.cpp
 * Author: Lameguy64
 * 
 * Created on September 9, 2018, 10:58 PM
 */

#include <Fl/fl_draw.H>
#include "Fl_Source_Browser.h"
#include "SymbolsClass.h"
#include "MainUI.h"

#define SELECTED 1
#define NOTDISPLAYED 2

extern MainUI *ui;

struct FL_BLINE {	// data is in a linked list of these
  FL_BLINE* prev;
  FL_BLINE* next;
  void* data;
  Fl_Image* icon;
  short length;		// sizeof(txt)-1, may be longer than string
  char flags;		// selected, displayed
  char txt[1];		// start of allocated array
};

Fl_Source_Browser::Fl_Source_Browser(int x, int y, int w, int h, const char* l) 
	: Fl_Browser( x, y, w, h, l ) {
	
	end();
	
	_highlight_line = 0;
	
}

void Fl_Source_Browser::item_draw(void* item, int X, int Y, int W, int H) const {
  FL_BLINE* l = (FL_BLINE*)item;
  char* str = l->txt;
  const int* i = column_widths();

  bool first = true;	// for icon
  while (W > 6) {	// do each tab-separated field
    int w1 = W;	// width for this field
    char* e = 0; // pointer to end of field or null if none
    if (*i) { // find end of field and temporarily replace with 0
      e = strchr(str, column_char());
      if (e) {*e = 0; w1 = *i++;}
    }
    // Icon drawing code
    if (first) {
      first = false;
      if (l->icon) {
	l->icon->draw(X+2,Y+1);	// leave 2px left, 1px above
	int iconw = l->icon->w()+2;
	X += iconw; W -= iconw; w1 -= iconw;
      }
    }
    int tsize = textsize();
    Fl_Font font = textfont();
    Fl_Color lcol = textcolor();
    Fl_Align talign = FL_ALIGN_LEFT;
    // check for all the @-lines recognized by XForms:
    //#if defined(__GNUC__)
    //#warning FIXME This maybe needs to be more UTF8 aware now...?
    //#endif /*__GNUC__*/
    while (*str == format_char() && *++str && *str != format_char()) {
      switch (*str++) {
      case 'l': case 'L': tsize = 24; break;
      case 'm': case 'M': tsize = 18; break;
      case 's': tsize = 11; break;
      case 'b': font = (Fl_Font)(font|FL_BOLD); break;
      case 'i': font = (Fl_Font)(font|FL_ITALIC); break;
      case 'f': case 't': font = FL_COURIER; break;
      case 'c': talign = FL_ALIGN_CENTER; break;
      case 'r': talign = FL_ALIGN_RIGHT; break;
      case 'B': 
	if (!(l->flags & SELECTED)) {
	  fl_color((Fl_Color)strtol(str, &str, 10));
	  fl_rectf(X, Y, w1, H);
	} else while (isdigit(*str & 255)) str++; // skip digits
        break;
      case 'C':
	lcol = (Fl_Color)strtol(str, &str, 10);
	break;
      case 'F':
	font = (Fl_Font)strtol(str, &str, 10);
	break;
      case 'N':
	lcol = FL_INACTIVE_COLOR;
	break;
      case 'S':
	tsize = strtol(str, &str, 10);
	break;
      case '-':
	fl_color(FL_DARK3);
	fl_line(X+3, Y+H/2, X+w1-3, Y+H/2);
	fl_color(FL_LIGHT3);
	fl_line(X+3, Y+H/2+1, X+w1-3, Y+H/2+1);
	break;
      case 'u':
      case '_':
	fl_color(lcol);
	fl_line(X+3, Y+H-1, X+w1-3, Y+H-1);
	break;
      case '.':
	goto BREAK;
      case '@':
	str--; goto BREAK;
      }
    }
  BREAK:
    fl_font(font, tsize);
    if (l->flags & SELECTED)
      lcol = fl_contrast(lcol, selection_color());
    if (!active_r()) lcol = fl_inactive(lcol);
  
    int lnum = lineno( item );
  
    if( lnum == _highlight_line ) {
      fl_color(FL_YELLOW);
      fl_rectf(X, Y, w1, H);
    }
  
    fl_color(lcol);
	//fl_line( X+3, Y+(fl_height()>>1), X+7, Y+(fl_height()>>1) );
	
	SymbolsClass::SourceClass* source = (SymbolsClass::SourceClass*)user_data();
	
	if( source ) {
		if( source->IsLineMapped( lnum ) ) {

			fl_polygon(
				X+2, Y+(fl_height()>>1),
				X+5, Y+(fl_height()>>1)-3,
				X+8, Y+(fl_height()>>1),
				X+5, Y+(fl_height()>>1)+3 );

		}
	}
	
    fl_draw(str, X+3+8, Y, w1-6, H, e ? Fl_Align(talign|FL_ALIGN_CLIP) : talign, 0, 0);
	
	
    if (!e) break; // no more fields...
    *e = column_char(); // put the separator back
    X += w1;
    W -= w1;
    str = e+1;
  }
}

int Fl_Source_Browser::handle(int event) {
	
	int ret = Fl_Browser::handle( event );
	if( ret ) {
		return ret;
	}
	
	ret = ui->handle(event);
	if( ret ) {
		return ret;
	}
	
}