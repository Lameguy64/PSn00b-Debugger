/* 
 * File:   Fl_Status_Bar.cpp
 * Author: Lameguy64
 * 
 * Created on October 11, 2018, 8:59 AM
 */

#include <Fl/fl_draw.H>
#include "Fl_Status_Bar.h"

Fl_Status_Bar::Fl_Status_Bar(int x, int y, int w, int h, const char* l) :
	Fl_Box(x, y, w, h, l) {
	
	address = 0;
	
}

Fl_Status_Bar::~Fl_Status_Bar() {
}

void Fl_Status_Bar::SetStatus(const char* text) {
	
	if( text == nullptr ) {
		status_text.clear();
	} else {
		status_text = text;
	}
	
	redraw();
	
}

void Fl_Status_Bar::draw() {
	
	char temp[16];
	int tw,th;
	const char *text;
	
	fl_draw_box(box(), x(), y(), w(), h(), color());
	
	fl_font(labelfont(), labelsize());
	fl_color(labelcolor());
	
	fl_draw(0, status_text.c_str(), x()+4, (y()+h())-4);
	
	int xx = (x()+w())-10;
	
	
	// First line (from right)
	sprintf( temp, "%08X", address );
	fl_measure(temp, tw, th, 0);
	xx -= tw;
	fl_draw(temp, xx, (y()+h())-4);
	xx -= 10;
	fl_draw_box(FL_THIN_DOWN_FRAME, xx, y()+3, 2, h()-5, color());
	
	
	// Second line
	fl_color(labelcolor());
	if( connected ) {
		
		text = "Connected";
		
	} else {
	
		text = "Disconnected";
		
	}
	
	fl_measure(text, tw, th, 0);
	xx -= tw+10;
	fl_draw(text, xx, (y()+h())-4);
	
	xx -= 10;
	fl_draw_box(FL_THIN_DOWN_FRAME, xx, y()+3, 2, h()-5, color());
	
	fl_measure(port_text.c_str(), tw, th, 0);
	xx -= tw+10;
	fl_color(labelcolor());
	fl_draw(port_text.c_str(), xx, (y()+h())-4);
	
	xx -= 10;
	fl_draw_box(FL_THIN_DOWN_FRAME, xx, y()+3, 2, h()-5, color());
	
}
