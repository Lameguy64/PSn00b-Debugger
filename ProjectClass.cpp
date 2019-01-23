/* 
 * File:   ProjectClass.cpp
 * Author: Lameguy64
 * 
 * Created on September 16, 2018, 8:11 PM
 */

#include <iostream>
#include <Fl/fl_message.H>
#include "ProjectClass.h"
#include "MainUI.h"
#include "SourceUI.h"
#include "ProgramClass.h"
#include "SymbolsUI.h"
#include "BookmarksUI.h"
#include "MessageUI.h"
#include "source.h"
#include "MemoryUI.h"

extern MainUI		*ui;
extern SourceUI		*src_ui;
extern SymbolsUI	*sym_ui;
extern MessageUI	*msg_ui;
extern MemoryUI		*mem_ui;
extern BookmarksUI	*bmlist_ui;

extern Fl_RGB_Image	*mwin_icon;
extern Fl_RGB_Image *mem_icon;

extern ProgramClass program;


void ShowBookmarksWindow();


ProjectClass::ProjectClass() {
}

ProjectClass::~ProjectClass() {
}

void ProjectClass::clear() {
	
	symbols.ClearSymbols();
	bookmarks.clear();
	
	last_src_file.clear();
	exe_file.clear();
	sym_file.clear();
	
}

int ProjectClass::Save(const char *filename) {
	
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement *base = doc.NewElement( "psdb_project" );
	
	tinyxml2::XMLElement *o,*oo;
	
	o = doc.NewElement( "files" );
	if( !exe_file.empty() ) {
		o->SetAttribute( "exe_file", exe_file.c_str() );
	}
	if( !sym_file.empty() ) {
		o->SetAttribute( "sym_file", sym_file.c_str() );
	}
	base->InsertEndChild( o );
	
	o = doc.NewElement( "bookmarks" );
	for( int i=0; i<bookmarks.size(); i++ ) {
		char addrtemp[10];
		sprintf( addrtemp, "%08x", bookmarks[i].addr );
		oo = doc.NewElement( "mark" );
		oo->SetAttribute( "address", addrtemp );
		oo->SetAttribute( "comment", bookmarks[i].remark.c_str() );
		o->InsertEndChild( oo );
	}
	base->InsertEndChild( o );
	
	o = doc.NewElement( "debugger_win" );
	o->InsertEndChild( DumpWinPos( &doc, ui->x(), ui->y(), ui->w(), ui->h() ) );
	base->InsertEndChild( o );
	
	o = doc.NewElement( "source_win" );
	o->SetAttribute( "active", src_ui->visible() );
	o->InsertEndChild( DumpWinPos( &doc, src_ui->x(), src_ui->y(), 
		src_ui->w(), src_ui->h() ) );
	base->InsertEndChild( o );
	
	o = doc.NewElement( "symbols_win" );
	o->SetAttribute( "active", sym_ui->visible() );
	o->InsertEndChild( DumpWinPos( &doc, sym_ui->x(), sym_ui->y(), 
		sym_ui->w(), sym_ui->h() ) );
	base->InsertEndChild( o );
	
	o = doc.NewElement( "message_win" );
	o->SetAttribute( "active", msg_ui->visible() );
	o->InsertEndChild( DumpWinPos( &doc, msg_ui->x(), msg_ui->y(), 
		msg_ui->w(), msg_ui->h() ) );
	base->InsertEndChild( o );
	
	o = doc.NewElement( "bookmarks_win" );
	o->SetAttribute( "active", bmlist_ui->visible() );
	o->InsertEndChild( DumpWinPos( &doc, bmlist_ui->x(), bmlist_ui->y(), 
		bmlist_ui->w(), bmlist_ui->h() ) );
	base->InsertEndChild( o );
	
	o = doc.NewElement( "memory_win" );
	o->SetAttribute( "active", mem_ui->visible() );
	o->InsertEndChild( DumpWinPos( &doc, mem_ui->x(), mem_ui->y(), 
		mem_ui->w(), mem_ui->h() ) );
	base->InsertEndChild( o );
	
	doc.InsertEndChild( base );
	
	if( doc.SaveFile( filename ) != tinyxml2::XML_SUCCESS ) {
		return -1;
	}
	
	return 0;
	
}

int ProjectClass::Load(const char* filename) {
	
	tinyxml2::XMLDocument doc;
	
	if( doc.LoadFile( filename ) != tinyxml2::XML_SUCCESS ) {
		return -1;
	}
	
	tinyxml2::XMLElement *base = doc.FirstChildElement( "psdb_project" );
	
	if( !base ) {
		return -1;
	}
	
	
	clear();
	
	tinyxml2::XMLElement *o = base->FirstChildElement( "files" );
	tinyxml2::XMLElement *oo;
	
	if( o->Attribute( "exe_file" ) ) {
		
		exe_file = o->Attribute( "exe_file" );
		
		if( program.LoadProgram( exe_file.c_str() ) ) {
			fl_message_title( "Error opening project" );
			fl_message( "Unable to load executable file:\n%s", 
				exe_file.c_str() );
			return -1;
		}
		
		ui->uploadButton->activate();
		ui->disasm->SetPC( program.exe_params.pc0, true );
		
	}
	if( o->Attribute( "sym_file" ) ) {
		
		sym_file = o->Attribute( "sym_file" );
		
		if( symbols.LoadSymbols( sym_file.c_str() ) ) {
			fl_message_title( "Error opening project" );
			fl_message( "Unable to load symbols file:\n%s", 
				sym_file.c_str() );
			return -1;
		}
		
		UpdateSourceView();
		
	}
	
	o = base->FirstChildElement( "bookmarks" );
	if( o ) {
		
		oo = o->FirstChildElement( "mark" );
		while( oo ) {
			BOOKMARK mark;
			
			const char *text = oo->Attribute( "address" );
			if( text ) {
				sscanf( text, "%08x", &mark.addr );
			}
			
			text = oo->Attribute( "comment" );
			if( text ) {
				mark.remark = text;
			}
			
			bookmarks.push_back( mark );
			
			oo = oo->NextSiblingElement( "mark" );
		}
		
	}
	
	o = base->FirstChildElement( "debugger_win" );
	if( o ) {
		oo = o->FirstChildElement( "window_pos" );
		ui->resize( 
			oo->IntAttribute( "x", 32 ),
			oo->IntAttribute( "y", 32 ),
			oo->IntAttribute( "w", 558 ), 
			oo->IntAttribute( "h", 600 ) );
	}
	
	o = base->FirstChildElement( "source_win" );
	if( o ) {
			
		if( ( symbols.sources.size() ) && ( o->IntAttribute( "active", 0 ) ) ) {

			oo = o->FirstChildElement( "window_pos" );

			if( oo ) {

				src_ui->resize( 
					oo->IntAttribute( "x", src_ui->x() ),
					oo->IntAttribute( "y", src_ui->y() ),
					oo->IntAttribute( "w", src_ui->w() ), 
					oo->IntAttribute( "h", src_ui->h() ) );
				
			}
			
			ShowSourceWindow();
				
		}
	}
	
	o = base->FirstChildElement( "symbols_win" );
	if( o ) {
		
		if( ( symbols.symbols.size() ) && ( o->IntAttribute( "active", 0 ) ) ) {
			
			oo = o->FirstChildElement( "window_pos" );
			
			if( oo ) {
				
				sym_ui->resize( 
					oo->IntAttribute( "x", sym_ui->x() ),
					oo->IntAttribute( "y", sym_ui->y() ),
					oo->IntAttribute( "w", sym_ui->w() ),
					oo->IntAttribute( "h", sym_ui->h() ) );
				
			}
			
			ShowSymbolsWindow();
			
		}
		
	}
	
	o = base->FirstChildElement( "message_win" );
	if( o ) {
		
		if( o->IntAttribute( "active", 0 ) ) {
			
			oo = o->FirstChildElement( "window_pos" );
			if( oo ) {

				msg_ui->resize( 
					oo->IntAttribute( "x", msg_ui->x() ),
					oo->IntAttribute( "y", msg_ui->y() ),
					oo->IntAttribute( "w", msg_ui->w() ), 
					oo->IntAttribute( "h", msg_ui->h() ) );

			}
			
			msg_ui->icon( mwin_icon );
			msg_ui->show();
			
		}
		
	}
	
	o = base->FirstChildElement( "bookmarks_win" );
	if( o ) {
		
		if( o->IntAttribute( "active", 0 ) ) {
			
			oo = o->FirstChildElement( "window_pos" );
			if( oo ) {

				bmlist_ui->resize( 
					oo->IntAttribute( "x", msg_ui->x() ),
					oo->IntAttribute( "y", msg_ui->y() ),
					oo->IntAttribute( "w", msg_ui->w() ), 
					oo->IntAttribute( "h", msg_ui->h() ) );

			}
			
			ShowBookmarksWindow();
			
		}
		
	}
	
	o = base->FirstChildElement( "memory_win" );
	if( o ) {
		
		if( o->IntAttribute( "active", 0 ) ) {
			
			oo = o->FirstChildElement( "window_pos" );
			if( oo ) {

				mem_ui->resize( 
					oo->IntAttribute( "x", msg_ui->x() ),
					oo->IntAttribute( "y", msg_ui->y() ),
					oo->IntAttribute( "w", msg_ui->w() ), 
					oo->IntAttribute( "h", msg_ui->h() ) );

			}
			
			mem_ui->icon( mem_icon );
			mem_ui->show();
			
		}
		
	}
	
	return 0;
	
}

tinyxml2::XMLElement *ProjectClass::DumpWinPos(tinyxml2::XMLDocument* doc, 
	int x, int y, int w, int h) {
	
	tinyxml2::XMLElement *o = doc->NewElement( "window_pos" );
	
	o->SetAttribute( "x", x );
	o->SetAttribute( "y", y );
	o->SetAttribute( "w", w );
	o->SetAttribute( "h", h );
	
	return o;
	
}