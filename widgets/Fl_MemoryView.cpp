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

#include "Fl_MemoryView.h"
#include "ProgramClass.h"
#include "MainUI.h"
#include "ProjectClass.h"
#include "CommsClass.h"
#include "RemoteMemClass.h"
#include "ConfigClass.h"

extern MainUI *ui;

extern int browser_last_focus;

extern ProgramClass program;
extern ProjectClass project;
extern ConfigClass config;

extern CommsClass comms;
extern RemoteMemClass data_mem;

Fl_MemoryView::Fl_MemoryView(int x, int y, int w, int h, const char* l) 
	: Fl_Group(x, y, w, h, l) {
	
	end();
	
	scroll_pos = 0x80000000>>3;
	cursor_pos = 0x80000000>>3;
	
	hex_mode = 0;
	
}

Fl_MemoryView::~Fl_MemoryView() {
}

void Fl_MemoryView::scroll_fix() {
	
	fl_font( _font_index, _font_size );
	int line_height = fl_height();
	
	if ( scroll_pos+4 > cursor_pos ) {

		scroll_pos = cursor_pos-4;

	} else if( scroll_pos+(((h()-4)/line_height)-4) < cursor_pos ) {

		scroll_pos = cursor_pos-(((h()-4)/line_height)-4);

	}
	
}

int Fl_MemoryView::handle(int event) {
	
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
			browser_last_focus = 1;
			redraw();
			return 1;
			
		case FL_UNFOCUS:
			redraw();
			return 1;
	}
	
	if( event == FL_SHORTCUT ) {
		
		int ret = ui->handle( event );
		if( ret ) {
			return ret;
		}
		
	}
	
	return 0;
	
}

void Fl_MemoryView::draw() {
		
	Fl_Group::draw();
	
	fl_color(FL_BLACK);
	fl_font( _font_index, _font_size );
	int line_height = fl_height();
	
	fl_push_clip( x()+2, y()+2, w()-4, h()-4 );
	
	for (int i=0; i<(h()/line_height)+1; i++) {
	
		int ty=line_height*i;
		char text[128];
		char temp[32];
		
		sprintf( text, "%08X: ", (scroll_pos+i)<<3 );
		
		if( comms.serial.IsOpen() ) {
			
			int draw_size = data_mem.remote_size;
			unsigned int draw_addr = data_mem.remote_addr;
			unsigned int* draw_data = data_mem.remote_data;

			if( ( draw_data ) && ( ( (scroll_pos+i) >= ( draw_addr>>3 ) ) && 
				( (scroll_pos+i) < ( ( draw_addr+draw_size )>>3 ) ) ) ) {

				#ifdef DEBUG
				if( i == 0 ) {
					std::cout << std::dec << (scroll_pos-(draw_addr>>3)) << std::endl;
				}
				#endif

				unsigned int word_a = draw_data[((scroll_pos-(draw_addr>>3))+i)<<1];
				unsigned int word_b = draw_data[(((scroll_pos-(draw_addr>>3))+i)<<1)+1];
				
				switch( hex_mode ) {
					case 0:
						sprintf( temp, "%08X %08X ", 
							word_a, 
							word_b );
						
						strcat( text, temp );
						
						break;
					case 1:
						sprintf( temp, "%04X %04X %04X %04X ", 
							word_a&0xffff, word_a>>16, 
							word_b&0xffff, word_b>>16 );
						
						strcat( text, temp );
						
						break;
					case 2:
						sprintf( temp, "%02X %02X %02X %02X %02X %02X %02X %02X ", 
							word_a&0xff, (word_a>>8)&0xff, 
							(word_a>>16)&0xff, (word_a>>24)&0xff,
							word_b&0xff, (word_b>>8)&0xff, 
							(word_b>>16)&0xff, (word_b>>24)&0xff );
						
						strcat( text, temp );
						
						break;
				}
				
				memcpy( temp, &draw_data[((scroll_pos-(draw_addr>>3))+i)<<1], 8 );
				
				for( int i=0; i<8; i++ ) {
					if( temp[i] == 0x0 ) {
						temp[i] = ' ';
					}
				}
				
				strncat( text, temp, 8 );
				
				/*sprintf( text, "%08X:%08X", 4*(scroll_pos+i), 
					draw_data[scroll_pos-(draw_addr>>2)+i] );

				mips_Decode( draw_data[scroll_pos-(draw_addr>>2)+i], 
					4*(scroll_pos+i), instrtext, true );

				unsigned int jaddr = mips_GetJumpAddr( 4*(scroll_pos+i),
					draw_data[scroll_pos-(draw_addr>>2)+i] );

				if( ( jaddr ) && ( ui->symbolsCheck->value() ) ) {

					const char* sym_name = project.symbols.FindSymbol( jaddr );

					if( sym_name ) {
						strcat( instrtext, "  ; " );
						strcat( instrtext, sym_name );
					}

				}*/

			} else {

				switch( hex_mode ) {
				case 0:
					strcat( text, "???????? ???????? ????????" );
					break;
				case 1:
					strcat( text, "???? ???? ???? ???? ????????" );
					break;
				case 2:
					strcat( text, "?? ?? ?? ?? ?? ?? ?? ?? ????????" );
					break;
				}

			}
	
		} else {

			switch( hex_mode ) {
				case 0:
					strcat( text, "???????? ???????? ????????" );
					break;
				case 1:
					strcat( text, "???? ???? ???? ???? ????????" );
					break;
				case 2:
					strcat( text, "?? ?? ?? ?? ?? ?? ?? ?? ????????" );
					break;
			}
			
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
			
			if( ( project.bookmarks[j].addr>>3 ) == ( scroll_pos+i ) ) {
				
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
		
		if( config.show_sym_memorybrowser ) {
			
			const char* sym_name = project.symbols.FindSymbol( (scroll_pos+i)<<3 );
			
			if( !sym_name ) {
				sym_name = project.symbols.FindSymbol( ((scroll_pos+i)<<3)+4 );
			}

			if( sym_name ) {
				fl_measure( sym_name, tw, th, 0 );
				fl_draw( sym_name, (x()+w())-tw-4, (ty+(y()+line_height))-2 );
			}
			
		}
		
	}
	
	fl_pop_clip();
	
}

void Fl_MemoryView::SetPos(unsigned long pos, int scroll) {
	
	cursor_pos = pos>>3;
	
	if ( scroll ) {
		scroll_fix();
	}
	
	redraw();
	
}