/* 
 * File:   Fl_Source_Browser.h
 * Author: Lameguy64
 *
 * Created on September 9, 2018, 10:58 PM
 */

#ifndef FL_SOURCE_BROWSER_H
#define FL_SOURCE_BROWSER_H

#include <Fl/Fl_Browser.H>

class Fl_Source_Browser : public Fl_Browser {
public:
	
	Fl_Source_Browser(int x, int y, int w, int h, const char* l = nullptr);
	
	void item_draw(void* item, int X, int Y, int W, int H) const;
	int handle(int event);
	
private:
	
	int _highlight_line;
	
};

#endif /* FL_SOURCE_BROWSER_H */

