/* 
 * File:   Fl_RegView.h
 * Author: Lameguy64
 *
 * Created on May 19, 2018, 1:39 PM
 */

#ifndef REGVIEWCLASS_H
#define REGVIEWCLASS_H

#include <Fl/Fl_Group.H>

class Fl_RegView : public Fl_Group {
public:
	
	Fl_RegView(int x, int y, int w, int h, const char* l = nullptr);
	
	void UpdateRegs(unsigned int* regs);
	void SetLastState(const char* text);
	void draw();
	
	void textfont(Fl_Font font) {
		_font_index = font;
	}
	void textsize(Fl_Fontsize newSize) {
		_font_size = newSize;
	}
	
	Fl_Font textfont(void) {
		return _font_index;
	}
	
	Fl_Fontsize textsize(void) {
		return _font_size;
	}
	
	enum {
		rzero=0, rat,rv0,rv1,
		ra0,ra1,ra2,ra3,
		rt0,rt1,rt2,rt3,
		rt4,rt5,rt6,rt7,
		rs0,rs1,rs2,rs3,
		rs4,rs5,rs6,rs7,
		rt8,rt9,rk0,rk1,
		rgp,rsp,rfp,rra,
		rhi,rlo,
		rstatus,
		rcause,
		repc,
		inst,
	};
	
private:
	
	unsigned int _reg_last_values[41];
	unsigned int _reg_values[41];
	
	Fl_Font _font_index;
	Fl_Fontsize _font_size;
	
	const char* _last_state;
	
};

#endif /* REGVIEWCLASS_H */

