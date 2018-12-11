/* 
 * File:   Fl_Main_Window.cpp
 * Author: Lameguy64
 * 
 * Created on October 9, 2018, 4:28 PM
 */

#include <iostream>
#include "Main_Window.h"

Main_Window::Main_Window(int x, int y, int w, int h, const char* l)
	: Fl_Double_Window( x, y, w, h, l ) {
	
	cb_func = 0;
	
}

Main_Window::~Main_Window() {
}

int Main_Window::handle(int event) {
	
	switch( event ) {
		case FL_HIDE:
			if( cb_func ) {
				cb_func( 0 );
			} 
			break;
		case FL_SHOW:
			if( cb_func ) {
				cb_func( 1 );
			}
			break;
	}
	
	int ret = Fl_Double_Window::handle( event );
	
	if( ret ) {
		return ret;
	}
	
}

void Main_Window::SetMinMaxCallback(MinMaxCallback *func) {

	cb_func = func;
	
}
