/* 
 * File:   Fl_RegView.cpp
 * Author: Lameguy64
 * 
 * Created on May 19, 2018, 1:39 PM
 */

#include <strings.h>
#include <iostream>
#include <Fl/Fl.H>
#include <Fl/fl_draw.H>
#include "Fl_RegView.h"
#include "mips_disassembler.h"
#include "pstypes.h"

Fl_RegView::Fl_RegView(int x, int y, int w, int h, const char* l)
	: Fl_Group(x, y, w, h, l) {
	
	_font_index = FL_HELVETICA;
	_font_size = 14;
	
	memset( _reg_last_values, 0x0, sizeof(unsigned int)*38 );
	memset( _reg_values, 0x0, sizeof(unsigned int)*38 );
	
	_last_state = nullptr;
	
}

void Fl_RegView::UpdateRegs(unsigned int* regs) {
	
	memcpy( _reg_last_values, _reg_values, sizeof(unsigned int)*40 );
	
	for( int i=0; i<40; i++ ) {
		_reg_values[i] = regs[i];
	}
	
	redraw();
	
}

void Fl_RegView::SetLastState(const char* text) {
	
	_last_state = text;
	
	redraw();
	
}

void Fl_RegView::draw() {
	
	int tx,ty;
	
	char temp[256];
	char inst[40];
	
	const char* reg_names[] = {
		"r0", "at", "v0", "v1",
		"a0", "a1", "a2", "a3",
		"t0", "t1", "t2", "t3",
		"t4", "t5", "t6", "t7",
		"s0", "s1", "s2", "s3",
		"s4", "s5", "s6", "s7",
		"t8", "t9", "k0", "k1",
		"gp", "sp", "fp", "ra",
		"hi", "lo"
	};
	
	const char* cop0_names[] = {
		"CAUSE ",
		"STATUS",
		"BADDR ",
		"TAR   ",
		"DCIC  "
	};
	
	Fl_Group::draw();
	
	fl_push_clip( x()+2, y()+2, w()-4, h()-4 );
	
	fl_color( FL_BLACK );
	fl_font( _font_index, _font_size );
	
	int line_height = fl_height();
	
	for( int iy=0; iy<6; iy++ ) {
		
		int line_x = x()+4;
		
		for( int ix=0; ix<6; ix++ ) {
			
			if( ((6*ix)+iy) > 33 ) {
				break;
			}
			
			if(( ix + iy ) == 0 ) {
				
				sprintf( inst, "%s=00000000 ", reg_names[0]);
				fl_color(FL_BLACK);
				
			} else {
				
				sprintf( inst, "%s=%08X ", reg_names[(6*ix)+iy], 
					_reg_values[((6*ix)+iy)-1] );
					
				if( _reg_values[((6*ix)+iy)-1] != _reg_last_values[((6*ix)+iy-1)] ) {
					
					fl_color(FL_RED);
					
				} else {
					
					fl_color(FL_BLACK);
					
				}
					
			}
			
			
			fl_draw( inst, line_x, y()+(line_height*(iy+1))-1 );
			
			fl_measure( inst, tx, ty );
			line_x += tx;
			
		}
		
	}
	
	if( _reg_values[PS_REG_epc] != _reg_last_values[PS_REG_epc] ) {
		fl_color(FL_RED);
	} else {
		fl_color(FL_BLACK);
	}
	sprintf(inst, "pc=%08X ", _reg_values[PS_REG_epc]);
	fl_draw(inst, x()+4, y()+(line_height*7)+4);
	fl_measure(inst, tx, ty);
	
	mips_Decode(_reg_values[PS_REG_opcode], _reg_values[PS_REG_epc], inst);
	fl_draw(inst, x()+4+tx, y()+(line_height*7)+4);
	
	
	fl_color( FL_BLACK );
	if(_last_state ) {
		fl_draw(_last_state, x()+4, y()+(line_height*9)+4 );
	}
	
	
	// Cop0 registers (for debugging)
	ty = y()+(line_height*11)+4;
	for( int i=34; i<=38; i++ ) {
		
		sprintf(inst, "%s=%08X ", cop0_names[i-34], _reg_values[i]);
					
		if( _reg_values[i] != _reg_last_values[i] ) {
			fl_color(FL_RED);
		} else {
			fl_color(FL_BLACK);
		}
		
		fl_draw(inst, x()+4, ty);
		ty += line_height;
		
	}
	
	fl_pop_clip();
	
}
