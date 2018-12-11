/* 
 * File:   Fl_MemoryView.h
 * Author: Lameguy64
 *
 * Created on November 02, 2018, 07:02 PM
 */

#ifndef FL_MEMORYVIEW_H
#define FL_MEMORYVIEW_H

#include <Fl/Fl_Group.H>

class Fl_MemoryView : public Fl_Group {
public:
	
	Fl_MemoryView(int x, int y, int w, int h, const char* l = nullptr);
	virtual ~Fl_MemoryView();
	
	void SetPos(unsigned long pos, int scroll);
	
	void textfont(Fl_Font font) {
		_font_index = font;
	}
	void textsize(Fl_Fontsize newSize) {
		_font_size = newSize;
	}
	int textfont() {
		return _font_index;
	}
	int textsize() {
		return _font_size;
	}
	
	int handle(int event);
	void draw();
	
	void SetHexDisplay(int mode) {
		hex_mode = mode;
	}
	
	unsigned int GetPos() {
		return cursor_pos;
	}
	
	unsigned int scroll_pos;
	
	
private:
	
	void scroll_fix();
	
	unsigned int cursor_pos;
	int hex_mode;
	
	Fl_Font _font_index;
	Fl_Fontsize _font_size;
	
};

#endif /* FL_MEMORYVIEW_H */

