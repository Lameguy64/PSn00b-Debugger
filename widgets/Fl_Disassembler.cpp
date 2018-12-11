/* 
 * File:   Fl_Disassembler.cpp
 * Author: Lameguy64
 * 
 * Created on June 19, 2018, 10:54 AM
 */


#include <strings.h>
#include <iostream>
#include <Fl/Fl.H>
#include <Fl/fl_draw.H>

#include "Fl_Disassembler.h"
#include "mips_disassembler.h"
#include "ProgramClass.h"
#include "MainUI.h"
#include "ProjectClass.h"
#include "RemoteMemClass.h"
#include "ConfigClass.h"

extern RemoteMemClass code_mem;
extern ProgramClass program;
extern ProjectClass project;
extern ConfigClass config;

extern MainUI* ui;
extern int browser_last_focus;
extern unsigned int reg_buffer[];


Fl_Disassembler::Fl_Disassembler(int x, int y, int w, int h, const char* l) 
	: Fl_Group(x, y, w, h, l) {
	
	end();
	
	exec_pos = 0x80000000>>2;
	scroll_pos = 0x80000000>>2;
	cursor_pos = 0x80000000>>2;
	
}

Fl_Disassembler::~Fl_Disassembler() {
}

void Fl_Disassembler::scroll_fix() {
	
	fl_font( _font_index, _font_size );
	int line_height = fl_height();
	
	if ( scroll_pos+4 > cursor_pos ) {

		scroll_pos = cursor_pos-4;

	} else if( scroll_pos+(((h()-4)/line_height)-4) < cursor_pos ) {

		scroll_pos = cursor_pos-(((h()-4)/line_height)-4);

	}
	
}

int Fl_Disassembler::handle(int event) {
	
	fl_font( _font_index, _font_size );
	
	int steps;
	int line_height = fl_height();
	
	switch(event) {
		case FL_KEYBOARD:
			
			if ( Fl::event_key() == FL_Page_Up ) {
				
				cursor_pos -= (h()/line_height)+1;
				scroll_fix();
				redraw();
				
				if( callback() ) {
					callback()( this, user_data() );
				}
				
				return 1;
				
			} else if ( Fl::event_key() == FL_Page_Down ) {
				
				cursor_pos += (h()/line_height)+1;
				scroll_fix();
				redraw();
				
				if( callback() ) {
					callback()( this, user_data() );
				}
				
				return 1;
				
			} else if ( Fl::event_key() == FL_Up ) {
				
				if ( cursor_pos > 0 ) {
					cursor_pos--;
					scroll_fix();
					redraw();
				
					if( callback() ) {
						callback()( this, user_data() );
					}
					
				}
				return 1;
				
			} else if ( Fl::event_key() == FL_Down ) {
				
				cursor_pos++;
				scroll_fix();
				redraw();
				
				if( callback() ) {
					callback()( this, user_data() );
				}
				
				return 1;
				
			} else if( Fl::event_key() == FL_Enter ) {
				
				if( callback() ) {
					callback()( this, user_data() );
				}
				
				return 1;
				
			}
			break;
			
		case FL_PUSH:
			
			if ( Fl::event_button1() || Fl::event_button3() ) {
				
				take_focus();
				
				unsigned int ypos = Fl::event_y()-(y()+2);
				cursor_pos = (ypos/line_height)+scroll_pos;
				
				if( callback() ) {
					callback()( this, user_data() );
				}
				
				redraw();
				
			}
			
			break;
			
		case FL_MOUSEWHEEL:
			
			scroll_pos += Fl::event_dy();
			
			if( callback() ) {
				callback()( this, user_data() );
			}
			
			redraw();

			return 1;
		
		case FL_FOCUS:
			browser_last_focus = 0;
			redraw();
			return 1;
			
		case FL_UNFOCUS:
			
			redraw();
			return 1;
			
	}
	
	return 0;
	
}

void Fl_Disassembler::draw() {
		
	Fl_Group::draw();
	
	fl_color(FL_BLACK);
	fl_font( _font_index, _font_size );
	int line_height = fl_height();
	
	fl_push_clip( x()+2, y()+2, w()-4, h()-4 );
	
	int draw_size = program.exe_params.t_size;
	unsigned int draw_addr = program.exe_params.t_addr;
	unsigned int* draw_data = program.exe_data;
	
	if( ui->remoteMode->value() ) {
		
		draw_data = code_mem.remote_data;
		draw_addr = code_mem.remote_addr;
		draw_size = code_mem.remote_size;
		
	}
	
	for (int i=0; i<(h()/line_height)+1; i++) {
	
		int ty=line_height*i;
		char text[128];
		char instrtext[128];
		
		instrtext[0] = 0;
		
		if( ( draw_data ) && ( ( ( scroll_pos+i ) >= ( draw_addr>>2 ) ) && 
			( ( scroll_pos+i ) < ( ( draw_addr+draw_size )>>2 ) ) ) ) {
					
			sprintf( text, "%08X:%08X", 4*(scroll_pos+i), 
				draw_data[scroll_pos-(draw_addr>>2)+i] );
			
			mips_Decode( draw_data[scroll_pos-(draw_addr>>2)+i], 
				4*(scroll_pos+i), instrtext, true );
			
			unsigned int jaddr = mips_GetJumpAddr( 4*(scroll_pos+i),
				draw_data[scroll_pos-(draw_addr>>2)+i] );
			
			if( ( jaddr ) && ( config.show_sym_disassembler ) ) {
				
				const char* sym_name = project.symbols.FindSymbol( jaddr );
				
				if( sym_name ) {
					strcat( instrtext, "  (" );
					strcat( instrtext, sym_name );
					strcat( instrtext, ")" );
				}
				
			}
					
		} else {
			
			sprintf(text, "%08X:????????", 4*(scroll_pos+i));
			instrtext[0] = 0x0;
			
		}
		
		if ( (scroll_pos+i) == exec_pos ) {
			
			fl_draw_box( FL_FLAT_BOX, x()+2, ty+y()+2, w()-4, line_height, FL_YELLOW );
			
		}

		if ( (scroll_pos+i) == cursor_pos ) {
			
			fl_draw_box( FL_FLAT_BOX, x()+2, ty+y()+2, w()-4, line_height, 15 );
			fl_color( fl_contrast( FL_BLACK, 15 ) );

			if ( Fl::focus() == this ) {
				fl_line_style( FL_DOT );
				fl_rect( x()+2, ty+y()+2, w()-4, line_height );
				fl_line_style( FL_SOLID );
			}
			
		} else {
			
			fl_color( FL_BLACK );
			
		}
		
		for( int j=0; j<project.bookmarks.size(); j++ ) {
			
			if( ( project.bookmarks[j].addr>>2 ) == ( scroll_pos+i ) ) {
				
				fl_polygon(
					x()+5, y()+ty+2+(fl_height()>>1),
					x()+8, y()+ty+2+(fl_height()>>1)-3,
					x()+11, y()+ty+2+(fl_height()>>1),
					x()+8, y()+ty+2+(fl_height()>>1)+3 );
				
			}
			
		}
		
		int tw,th;
		
		fl_draw( text, (x()+14), (ty+(y()+line_height))-1 );
		fl_measure( text, tw, th, 0 );
		fl_draw( instrtext, (x()+14)+tw+20, (ty+(y()+line_height))-1 );

		// Draw symbol
		if( config.show_sym_disassembler ) {
			
			const char* sym_name = project.symbols.FindSymbol( 4*(scroll_pos+i) );

			if( sym_name ) {
				fl_measure( sym_name, tw, th, 0 );
				fl_draw( sym_name, (x()+w())-tw-4, (ty+(y()+line_height))-2 );
			}
			
		}
		
	}
	
	unsigned int dest_addr = mips_GetNextPc( reg_buffer, false );
	
	if( ( dest_addr > reg_buffer[36]+4 ) || ( dest_addr < reg_buffer[36] ) ) {
		
		int src_y = (line_height*((reg_buffer[36]>>2)-scroll_pos))+(line_height>>1)+2;
		int dest_y = (line_height*((dest_addr>>2)-scroll_pos))+(line_height>>1)+2;

		fl_color( FL_BLACK );
		
		fl_polygon( 
			x()+8, dest_y+y()-4, 
			x()+12, dest_y+y(), 
			x()+8, dest_y+y()+4, 
			x()+8, dest_y+y()-4 );
		
		fl_yxline( x()+4, y()+src_y, y()+dest_y, x()+8 );
		fl_yxline( x()+5, y()+dest_y, y()+src_y, x()+8 );
		
	}
	
	fl_pop_clip();
	
}

void Fl_Disassembler::SetPos(unsigned long pos, int scroll) {
	
	cursor_pos = pos>>2;
	
	if ( scroll ) {
		scroll_fix();
	}
	
	redraw();
	
}

void Fl_Disassembler::SetPC(unsigned long pc, int scroll) {
	
	exec_pos = pc>>2;
	cursor_pos = exec_pos;
	
	if ( scroll ) {
		scroll_fix();
	}
	
	redraw();
	
}