/* 
 * File:   Fl_Main_Window.h
 * Author: Lameguy64
 *
 * Created on October 9, 2018, 4:28 PM
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Fl/Fl_Double_Window.H>

typedef void (MinMaxCallback)(int type);

class Main_Window : public Fl_Double_Window {
public:
	
	Main_Window(int x, int y, int w, int h, const char* l = 0);
	virtual ~Main_Window();
	
	int handle(int event);
	
	void SetMinMaxCallback(MinMaxCallback *func);

private:
	
	MinMaxCallback *cb_func;
	
};

#endif /* MAIN_WINDOW_H */

