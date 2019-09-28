/* 
 * File:   ProjectClass.cpp
 * Author: Lameguy64
 * 
 * Created on September 19, 2018, 11:58 AM
 */

#include <stdio.h>
#include <iostream>
#include <Fl/fl_message.H>
#include "MainUI.h"
#include "SourceUI.h"
#include "ProjectClass.h"

#include "source.h"
#include "Fl_Symbol_Table.h"
#include "SymbolsUI.h"
#include "CommsClass.h"


extern ProjectClass project;

extern CommsClass comms;

extern MainUI* ui;
extern SourceUI* src_ui;
extern SymbolsUI* sym_ui;

extern Fl_RGB_Image *src_icon;
extern Fl_RGB_Image *sym_icon;


void cb_SymbolsTable(Fl_Symbol_Table* w, void* u) {
	
	if( Fl::event() == FL_PUSH ) {
		
		if( Fl::event_clicks() ) {

			// Get selected item
			int col_a, col_b;
			int row_a, row_b;
			sym_ui->symbolsTable->get_selection( row_a, col_a, row_b, col_b );

			const SymbolsClass::SYM_NAME *item;
			item = sym_ui->symbolsTable->item( row_b );
			
			if( item != nullptr ) {
				
				ui->disasm->SetPos( item->addr, true );
				ui->statusBar->SetAddress( item->addr );
				
			}

		}
		
	}
	
}

void ShowSymbolsWindow() {
	
	sym_ui->symbolsTable->clear();
	
	for( int i=0; i<project.symbols.symbols.size(); i++ ) {
		
		sym_ui->symbolsTable->additem( &project.symbols.symbols[i] );
		
	}
	
	sym_ui->symbolsTable->autowidth( 12 );
	
	if( !sym_ui->shown() ) {
		sym_ui->icon( sym_icon );
		sym_ui->show();
	}
	
}

static void cb_sourceRunToLine(Fl_Widget *w, void *u) {
	
	if( !comms.serial.IsOpen() ) {
		return;
	}
	
	SymbolsClass::SourceClass* source = 
		(SymbolsClass::SourceClass*)src_ui->sourceBrowser->user_data();
	
	unsigned int addr = source->LineToAddr( src_ui->sourceBrowser->value() );
	
#ifdef DEBUG
	std::cout << "Run to = " << std::hex << addr << std::dec << std::endl;
#endif
	
	if( addr > 0 ) {
		
		comms.dbRunTo( addr );
		
	}
	
}

void cb_sourceBrowser(Fl_Source_Browser* w, void* u) {
	
	if( Fl::event_button() == FL_LEFT_MOUSE ) {
		
		SymbolsClass::SourceClass* source = (SymbolsClass::SourceClass*)u;
		unsigned int addr = source->LineToAddr( w->value() );

		if( addr > 0 ) {
			ui->disasm->SetPos( addr, 1 );
			ui->statusBar->SetAddress( addr );
		}
		
	} else if( Fl::event_button() == FL_RIGHT_MOUSE ) {
		
		Fl_Menu_Item pulldown[] = {
			{ "Run to Line", 0, cb_sourceRunToLine, 0, 0, 0, FL_HELVETICA, 12 },
			{ 0 }
		};
		
		const Fl_Menu_Item* item = pulldown->popup( Fl::event_x(), Fl::event_y() );

		if( item ) {
			if( item->callback() ) {
				item->callback()( w, nullptr );
			}
		}

	}
	
}

void cb_sourceChoice(Fl_Widget* w, void* u) {
	
	SymbolsClass::SourceClass* source = (SymbolsClass::SourceClass*)u;
	
	ViewSource( source );
	
}

void ShowSourceWindow() {
	
	if( project.symbols.sources.size() == 0 ) {
		return;
	}
	
	if( ViewSource( project.symbols.sources[0] ) ) {
		return;
	}
	
}

int ViewSource(SymbolsClass::SourceClass* source, int line) {
	
	if( source->src_file != project.last_src_file ) {
		
		FILE* fp = fopen( source->src_file.c_str(), "r" );

		if( !fp ) {
			fl_message_title( "File Not Found" );
			fl_message( "Could not open file:\n%s", source->src_file.c_str() );
			return 1;
		}

		src_ui->sourceBrowser->clear();

		char linebuff[256];

		while( !feof( fp ) ) {

			std::string untabbed_text;

			untabbed_text.clear();

			linebuff[0] = 0;
			fgets( linebuff, 256, fp );

			if( linebuff[0] == 0 ) {
				break;
			}

			for( int c=0; linebuff[c] != 0x0; c++ ) {

				if( linebuff[c] == '\t' ) {
					
					int toadd = 4-(untabbed_text.size()%4);
					
					for( int d=0; d<toadd; d++ ) {
						untabbed_text.append( " " );
					}
					
				} else {
					
					untabbed_text.push_back( linebuff[c] );
					
				}

			}

			src_ui->sourceBrowser->add( untabbed_text.c_str() );

		}

		fclose( fp );
		
	}
	
	if( line > 0 ) {
		
		src_ui->sourceBrowser->value( line );
		
		for( int i=0; i<project.symbols.sources.size(); i++ ) {

			if( project.symbols.sources[i] == source ) {
				src_ui->sourceFileChoice->value( i );
				break;
			}

		}
		
	}
	
	src_ui->sourceBrowser->user_data( source );
	
	if( !src_ui->shown() ) {
		src_ui->icon( src_icon );
		src_ui->show();
	}
	
	return 0;
	
}

void UpdateSourceView() {
	
	// Query for source files
	src_ui->sourceFileChoice->clear();
	
	for( int i=0; i<project.symbols.sources.size(); i++ ) {
		
		// Trim off the path name
		std::string name_nopath = project.symbols.sources[i]->src_file;
		
		if( name_nopath.rfind( "\\" ) != std::string::npos ) {
			
			name_nopath.erase( name_nopath.begin(), 
				name_nopath.begin()+name_nopath.rfind( "\\" )+1 );
			
		}
		
		// Add to menu
		src_ui->sourceFileChoice->add( name_nopath.c_str(), 0, 
			cb_sourceChoice, project.symbols.sources[i], 0 );
		
		#ifdef DEBUG
		std::cout << name_nopath << " = " 
			<< project.symbols.sources[i]->lines.size() << std::endl;
		#endif
		
		if( i == 0 ) {
			src_ui->sourceFileChoice->value( 0 );
			project.last_src_file.clear();
		}
		
	}
	
}