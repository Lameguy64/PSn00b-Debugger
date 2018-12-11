/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Fl_Symbol_Table.cpp
 * Author: Lameguy64
 * 
 * Created on September 23, 2018, 7:24 PM
 */

#include <algorithm>
#include <Fl/fl_draw.H>
#include <iostream>
#include "Fl_Symbol_Table.h"
#include "MainUI.h"

#define HEADER_FONTFACE FL_HELVETICA_BOLD
#define HEADER_FONTSIZE 12
#define ROW_FONTFACE    FL_COURIER
#define ROW_FONTSIZE    12

static const char *G_header[] = { "Name", "Address", "Type", "Source File", 0 };

extern MainUI* ui;


Fl_Symbol_Table::Fl_Symbol_Table(int x, int y, int w, int h, const char *l) 
	: Fl_Table_Row(x,y,w,h,l) {
	
	_sort_reverse = 0;
    _sort_lastcol = -1;
	
	Fl_Table_Row::type( SELECT_SINGLE );
	
	col_header( 1 );
	col_resize( 1 );
	cols( 3 );
	
}

Fl_Symbol_Table::~Fl_Symbol_Table() {
}

void Fl_Symbol_Table::sort_column(int col, int reverse) {
	
    std::sort( _row_items.begin(), _row_items.end(), SortColumn( col, reverse ) );
    redraw();
	
}

void Fl_Symbol_Table::additem(SymbolsClass::SYM_NAME *sym_item) {
	
	const char *s = nullptr; 
	char strbuff[20];
	
	Row r;
	std::string c;
	
	c = sym_item->name;
	r.cols.push_back( c );
	
	sprintf( strbuff, "$%08X", sym_item->addr );
	c = strbuff;
	r.cols.push_back( c );
	
	sprintf( strbuff, "$%02X", sym_item->id );
	c = strbuff;
	r.cols.push_back( c );
	r.sym = sym_item;
	
	_row_items.push_back( r );
	rows( _row_items.size() );
	
	fl_font( ROW_FONTFACE, ROW_FONTSIZE );
	row_height( _row_items.size()-1, fl_height()+4 );
	
}

void Fl_Symbol_Table::clear() {
	
	_row_items.clear();
	rows( 0 );
	redraw();
	
}

void Fl_Symbol_Table::draw_sort_arrow(int X,int Y,int W,int H) {
	
    int xlft = X+(W-6)-8;
    int xctr = X+(W-6)-4;
    int xrit = X+(W-6)-0;
    int ytop = Y+(H/2)-4;
    int ybot = Y+(H/2)+4;
	
    if( _sort_reverse ) {
        
		// Engraved down arrow
        fl_color( FL_WHITE );
        fl_line( xrit, ytop, xctr, ybot );
        fl_color( 41 );                   // dark gray
        fl_line( xlft, ytop, xrit, ytop );
        fl_line( xlft, ytop, xctr, ybot );
		
    } else {
		
        // Engraved up arrow
        fl_color( FL_WHITE );
        fl_line( xrit, ybot, xctr, ytop );
        fl_line( xrit, ybot, xlft, ybot );
        fl_color( 41 );                   // dark gray
        fl_line( xlft, ybot, xctr, ytop );
		
    }
}

void Fl_Symbol_Table::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) {
    
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
				if( C == _sort_lastcol ) {
					draw_sort_arrow( X, Y, W, H );
				}
				
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
			
			fl_draw( s, X+2,Y,W,H, FL_ALIGN_LEFT );     // +2=pad left 
			
            // Border
            fl_color( FL_LIGHT2 );
			fl_rect( X, Y, W, H );
            
            fl_pop_clip();
            return;
        
    }
}

const SymbolsClass::SYM_NAME *Fl_Symbol_Table::item(int row) {
	
	if( ( row < 0 ) || ( row >= rows() ) ) {
		return nullptr;
	}
	
	return _row_items[row].sym;
	
}

void Fl_Symbol_Table::autowidth(int pad) {
    
	int w, h;
	char strbuff[20];
	
    // Initialize all column widths to header width
    fl_font( HEADER_FONTFACE, HEADER_FONTSIZE );
	
    for( int c=0; G_header[c]; c++ ) {
		w=0;
		fl_measure( G_header[c], w, h, 0 );                   // pixel width of header text
		col_width( c, w+pad );
    }
	
    fl_font( ROW_FONTFACE, ROW_FONTSIZE );
	
    for( int r=0; r<_row_items.size(); r++ ) {
		
        for( int c=0; c<_row_items[r].cols.size(); c++ ) {
            
			w = 0;
			fl_measure( _row_items[r].cols[c].c_str(), w, h, 0);       // pixel width of row text
			
			if( ( w + pad ) > col_width( c ) ) {
				
				col_width(c, w+pad);
				
			}
			
        }
		
    }
	
    table_resized();
    redraw();
}

int Fl_Symbol_Table::handle(int event) {
	
	int ret = Fl_Table_Row::handle( event );
	
	int COL = callback_col();
	switch( callback_context() ) {
        case CONTEXT_COL_HEADER:				// someone clicked on column header
            if( Fl::event() == FL_RELEASE && Fl::event_button() == 1 ) {
                if ( _sort_lastcol == COL ) {	// Click same column? Toggle sort
                    _sort_reverse ^= 1;
                } else {						// Click diff column? Up sort 
                    _sort_reverse = 0;
                }
                sort_column(COL, _sort_reverse);
                _sort_lastcol = COL;
            }
            break;
    }
	
	if( ret ) {
		return ret;
	}
	
	/*ret = ui->handle(event);
	if( ret ) {
		return ret;
	}*/
	
	return 0;
	
}