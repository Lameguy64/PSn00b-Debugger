// Created on August 29, 2019, 2:28 PM

#include <iostream>
#include <Fl/fl_draw.H>
#include "Fl_WatchTable.h"

#define HEADER_FONTFACE FL_HELVETICA_BOLD
#define HEADER_FONTSIZE 12
#define ROW_FONTFACE    FL_COURIER
#define ROW_FONTSIZE    12

#define TABLE_ROW_HEIGHT	16

static const char *G_header[] = { "Name/Address", "Type", "Value", 0 };

extern ProjectClass project;

Fl_WatchTable::Fl_WatchTable(int X, int Y, int W, int H, const char *L)
	: Fl_Table_Row(X, Y, W, H, L) {
	
	type(SELECT_SINGLE);
	col_header(1);
	col_resize(1);
	cols(3);
	rows(0);
	
	col_header_height(TABLE_ROW_HEIGHT);
	autowidth(10);
	
	_selected = -1;
	_show_values = false;
	
	std::cout << "Widget initialized" << std::endl;
	
}

Fl_WatchTable::~Fl_WatchTable() {
}

void Fl_WatchTable::add_item(ProjectClass::WATCHPOINT* item) {
	
	const char *s = nullptr; 
	char strbuff[50];
	
	Row r;
	std::string c;
	
	// Address/name column
	if( item->sym_name.size() ) {
		
		// Use symbol name if specified
		c = item->sym_name;
		
	} else {
		
		// Plain address
		sprintf(strbuff, "0x%08X", item->addr);
		c = strbuff;
		
	}
	r.cols.push_back(c);
	
	// Type column
	switch(item->type) {
		case ProjectClass::WATCH_CHAR:
			c = "char";
			break;
		case ProjectClass::WATCH_SHORT:
			c = "short";
			break;
		case ProjectClass::WATCH_INT:
			c = "int";
			break;
		default:
			c = "unknown type";
	}
	if( item->unsign ) {
		c += " (unsigned)";
	}
	r.cols.push_back(c);
	
	// Watch value
	c = "??";
	r.cols.push_back(c);
	
	r.watch = item;
	
	// Add item to list and update table
	_row_items.push_back(r);
	rows(_row_items.size());
	row_height(_row_items.size()-1, fl_height()+4);
	autowidth(10);
	redraw();
	
}

void Fl_WatchTable::delete_item(int row) {
	
	_row_items.erase(_row_items.begin()+row);
	rows(_row_items.size());
	redraw();
	
}

void Fl_WatchTable::update_types() {
	
	char temp[64];
	
	for(int i=0; i<_row_items.size(); i++) {
		
		// Name/address
		if( _row_items[i].watch->sym_name.size() ) {
		
			// Use symbol name if specified
			_row_items[i].cols[0] = _row_items[i].watch->sym_name;

		} else {

			// Plain address
			char strbuff[12];
			sprintf(strbuff, "0x%08X", _row_items[i].watch->addr);
			_row_items[i].cols[0] = strbuff;

		}

		// Type column
		switch(_row_items[i].watch->type) {
			case ProjectClass::WATCH_CHAR:
				_row_items[i].cols[1] = "char";
				break;
			case ProjectClass::WATCH_SHORT:
				_row_items[i].cols[1] = "short";
				break;
			case ProjectClass::WATCH_INT:
				_row_items[i].cols[1] = "int";
				break;
			default:
				_row_items[i].cols[1] = "unknown type";
		}
		
		if( _row_items[i].watch->unsign ) {
			_row_items[i].cols[1] += " (unsigned)";
		}
		
	}
	
	autowidth(10);
	redraw();
	
}

void Fl_WatchTable::update_values() {
	
	char temp[64];
	
	for(int i=0; i<_row_items.size(); i++) {
		
		// Value
		if( _show_values ) {
			
			if( _row_items[i].watch->hex ) {

				sprintf(temp, "0x%X", _row_items[i].watch->value);

			} else {

				char	val_char	= *((char*)&_row_items[i].watch->value);
				short	val_short	= *((short*)&_row_items[i].watch->value);
				int		val_int		= *((int*)&_row_items[i].watch->value);

				if( _row_items[i].watch->hex ) {

					sprintf(temp, "0x%X", ((unsigned int)val_int));

				} else {

					if( _row_items[i].watch->unsign ) {

						switch(_row_items[i].watch->type) {
							case ProjectClass::WATCH_CHAR:
								sprintf(temp, "%d", ((unsigned int)val_char));
								break;
							case ProjectClass::WATCH_SHORT:
								sprintf(temp, "%d", ((unsigned int)val_short));
								break;
							case ProjectClass::WATCH_INT:
								sprintf(temp, "%d", ((unsigned int)val_int));
								break;
						}

					} else {

						switch(_row_items[i].watch->type) {
							case ProjectClass::WATCH_CHAR:
								sprintf(temp, "%d", val_char);
								break;
							case ProjectClass::WATCH_SHORT:
								sprintf(temp, "%d", val_short);
								break;
							case ProjectClass::WATCH_INT:
								sprintf(temp, "%d", val_int);
								break;
						}

					}

				}

			}

			_row_items[i].cols[2] = temp;
			
		} else {
			
			_row_items[i].cols[2] = "??";
			
		}
		
	}
	
	autowidth(10);
	redraw();
	
}

void Fl_WatchTable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) {
    
	const char *s = nullptr; 
	
	if( R < (int)_row_items.size() && C < (int)_row_items[R].cols.size() ) {
		s = _row_items[R].cols[C].c_str(); 
	}
	
    switch( context ) {
        case CONTEXT_COL_HEADER:
            fl_push_clip( X, Y, W, H );
			
			fl_draw_box( FL_THIN_UP_BOX, X, Y, W, H, FL_BACKGROUND_COLOR );
			
            if( C < 9 ) {
				
				fl_font( HEADER_FONTFACE, HEADER_FONTSIZE );
				fl_color( FL_BLACK );
				fl_draw( G_header[C], X+2, Y, W, H, FL_ALIGN_LEFT, 0, 0 );         // +2=pad left 
				
				// Draw sort arrow
				/*if( C == _sort_lastcol ) {
					draw_sort_arrow( X, Y, W, H );
				}*/
				
			}
			
            fl_pop_clip();
			
            return; 
			
        case CONTEXT_CELL:
            fl_push_clip(X,Y,W,H);
			
			// Bg color
			Fl_Color bgcolor = row_selected( R ) ? selection_color() : FL_WHITE;
			fl_color( bgcolor );
			fl_rectf( X, Y, W, H );
			fl_font( ROW_FONTFACE, ROW_FONTSIZE );
			fl_color( FL_BLACK );
			
			fl_color( fl_contrast( FL_WHITE, bgcolor ) );
			
			if( s ) {
				fl_draw( s, X+2,Y,W,H, FL_ALIGN_LEFT );     // +2=pad left 
			}
			//fl_draw("Sample0x00", X+2,Y,W,H, FL_ALIGN_LEFT);
			
            // Border
            fl_color( FL_LIGHT2 );
			fl_rect( X, Y, W, H );
            
            fl_pop_clip();
            return;
        
    }
	
}

int Fl_WatchTable::handle(int event) {
	
			
	int ret = Fl_Table_Row::handle( event );
	
	if( ret ) {
		return ret;
	}
		
	return event;
	
}

void Fl_WatchTable::autowidth(int pad) {
    
	int w, h;
	char strbuff[20];
	
    // Initialize all column widths to header width
    fl_font( HEADER_FONTFACE, HEADER_FONTSIZE );
	
    for( int c=0; G_header[c]; c++ )
	{
		w=0;
		fl_measure( G_header[c], w, h, 0 );                   // pixel width of header text
		if( ( w+pad ) > col_width(c) )
		{
			col_width(c, w+pad);
		}
    }
	
    fl_font(ROW_FONTFACE, ROW_FONTSIZE);
	
    for( int r=0; r<_row_items.size(); r++ )
	{	
        for( int c=0; c<_row_items[r].cols.size(); c++ )
		{
			w = 0;
			fl_measure( _row_items[r].cols[c].c_str(), w, h, 0);       // pixel width of row text
			
			if( ( w + pad ) > col_width( c ) )
			{
				col_width(c, w+pad);
			}
			
        }
    }
	
    table_resized();
    redraw();
	
}

void Fl_WatchTable::clear() {
	
	_row_items.clear();
	rows(0);
	redraw();
	
}