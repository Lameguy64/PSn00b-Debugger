/* 
 * File:   Fl_Disassembler.h
 * Author: Lameguy64
 *
 * Created on June 19, 2018, 10:54 AM
 */

#ifndef DISASSEMBLYCLASS_H
#define DISASSEMBLYCLASS_H

#include <Fl/Fl_Group.H>

class Fl_Disassembler : public Fl_Group {
public:
	
	Fl_Disassembler(int x, int y, int w, int h, const char* l = nullptr);
	virtual ~Fl_Disassembler();
	
	void SetPos(unsigned long pos, int scroll);
	void SetPC(unsigned long pc, int scroll);
	
	void ClearPC(void) {
		
		show_exec = false;
		
	}
	
	unsigned int GetPos() {
		
		return cursor_pos;
		
	}
	
	void ShowSymbols(int v) {
		
		show_symbols = v;
		
	}
	
	void textfont(Fl_Font font) {
	
		_font_index = font;
		
	}
	
	void textsize(Fl_Fontsize newSize) {
	
		_font_size = newSize;
		
	}
	
	Fl_Font textfont() {
	
		return _font_index;
		
	}
	
	Fl_Fontsize textsize() {
	
		return _font_size;
		
	}
	
	int handle(int event);
	void draw();
	
	void clear();
	unsigned int scroll_pos;
	
private:
	
	void scroll_fix();
	
	unsigned int cursor_pos;
	unsigned int exec_pos;
	int show_exec;
	int show_symbols;
	
	Fl_Font _font_index;
	Fl_Fontsize _font_size;
	
};

#endif /* DISASSEMBLYCLASS_H */

