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
#include "WatchesUI.h"

extern MainUI		*ui;
extern SourceUI		*src_ui;
extern SymbolsUI	*sym_ui;
extern MessageUI	*msg_ui;
extern MemoryUI		*mem_ui;
extern BookmarksUI	*bmlist_ui;
extern WatchesUI	*watch_ui;

extern Fl_RGB_Image	*mwin_icon;
extern Fl_RGB_Image *mem_icon;
extern Fl_RGB_Image *watch_icon;

extern ProgramClass program;


void ShowBookmarksWindow();


ProjectClass::ProjectClass() {
}

ProjectClass::~ProjectClass() {
	
	if( watchpoints.size() > 0 ) {
		
		for(int i=0; i<watchpoints.size(); i++) {
			
			delete watchpoints[i];
			
		}
		
	}
	
	watchpoints.clear();
	
}

void ProjectClass::clear()
{
	file.clear();
	
	symbols.ClearSymbols();
	bookmarks.clear();
	
	last_src_file.clear();
	exe_file.clear();
	sym_file.clear();
	
	for( int i=0; i<watchpoints.size(); i++ )
	{
		delete watchpoints[i];
	}
	watchpoints.clear();
	
}

int ProjectClass::Save(const char *filename) {
	
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement *base = doc.NewElement( "psdb_project" );
	
	tinyxml2::XMLElement *o,*oo;
	
	// Save files
	o = doc.NewElement( "files" );
	if( !exe_file.empty() )
	{
		o->SetAttribute( "exe_file", exe_file.c_str() );
	}
	if( !sym_file.empty() )
	{
		o->SetAttribute( "sym_file", sym_file.c_str() );
	}
	base->InsertEndChild( o );
	
	// Save bookmarks
	o = doc.NewElement( "bookmarks" );
	for( int i=0; i<bookmarks.size(); i++ )
	{
		char addrtemp[10];
		
		sprintf(addrtemp, "%08x", bookmarks[i].addr);
		
		oo = doc.NewElement("mark");
		oo->SetAttribute("address", addrtemp);
		oo->SetAttribute("comment", bookmarks[i].remark.c_str());
		o->InsertEndChild(oo);
	}
	base->InsertEndChild(o);
	
	// Save watchpoints
	o = doc.NewElement("watchpoints");
	for( int i=0; i<watchpoints.size(); i++ )
	{
		
		char addrtemp[10];
		
		oo = doc.NewElement("watch");
		
		if( watchpoints[i]->sym_name.empty() )
		{
			sprintf(addrtemp, "%08x", watchpoints[i]->addr);
			oo->SetAttribute("address", addrtemp);
		}
		else
		{
			oo->SetAttribute("symbol", watchpoints[i]->sym_name.c_str());
		}
		
		oo->SetAttribute("type", watchpoints[i]->type);
		oo->SetAttribute("unsigned", (bool)watchpoints[i]->unsign);
		oo->SetAttribute("hex", (bool)watchpoints[i]->hex);
		
		o->InsertEndChild(oo);
		
	}
	base->InsertEndChild(o);
	
	// Save window positions
	o = doc.NewElement( "debugger_win" );
	o->InsertEndChild( DumpWinPos( &doc, ui->x(), ui->y(), ui->w(), ui->h() ) );
	base->InsertEndChild( o );
	
	if( src_ui->shown() )
	{
		o = doc.NewElement( "source_win" );
		o->InsertEndChild( DumpWinPos( &doc, src_ui->x(), src_ui->y(), 
			src_ui->w(), src_ui->h() ) );
		base->InsertEndChild( o );
	}
	
	if( sym_ui->shown() )
	{
		o = doc.NewElement( "symbols_win" );
		o->InsertEndChild( DumpWinPos( &doc, sym_ui->x(), sym_ui->y(), 
			sym_ui->w(), sym_ui->h() ) );
		base->InsertEndChild( o );
	}

	if( msg_ui->shown() )
	{
		o = doc.NewElement( "message_win" );
		o->InsertEndChild( DumpWinPos( &doc, msg_ui->x(), msg_ui->y(), 
			msg_ui->w(), msg_ui->h() ) );
		base->InsertEndChild( o );
	}

	if( bmlist_ui->shown() )
	{
		o = doc.NewElement( "bookmarks_win" );
		o->InsertEndChild( DumpWinPos( &doc, bmlist_ui->x(), bmlist_ui->y(), 
			bmlist_ui->w(), bmlist_ui->h() ) );
		base->InsertEndChild( o );
	}
	
	if( mem_ui->shown() )
	{
		o = doc.NewElement( "memory_win" );
		o->InsertEndChild( DumpWinPos( &doc, mem_ui->x(), mem_ui->y(), 
			mem_ui->w(), mem_ui->h() ) );
		base->InsertEndChild( o );
	}

	if( watch_ui->shown() )
	{
		o = doc.NewElement( "watch_win" );
		o->InsertEndChild( DumpWinPos( &doc, watch_ui->x(), watch_ui->y(), 
			watch_ui->w(), watch_ui->h() ) );
		base->InsertEndChild( o );
	}
	
	doc.InsertEndChild( base );
	
	if( doc.SaveFile( filename ) != tinyxml2::XML_SUCCESS ) {
		return -1;
	}
	
	return 0;
	
}

int ProjectClass::Load(const char* filename) {
	
	tinyxml2::XMLDocument doc;
	
	if( doc.LoadFile(filename) != tinyxml2::XML_SUCCESS )
	{
		return -1;
	}
	
	tinyxml2::XMLElement *base = doc.FirstChildElement("psdb_project");
	
	if( !base )
	{
		return -1;
	}
	
	
	clear();
	
	tinyxml2::XMLElement *o = base->FirstChildElement("files");
	tinyxml2::XMLElement *oo;
	
	if( o->Attribute("exe_file") )
	{
		exe_file = o->Attribute("exe_file");
		
		if( program.LoadProgram(exe_file.c_str()) )
		{
			fl_message_title("Error opening project");
			fl_message("Unable to load executable file:\n%s", exe_file.c_str());
			return -1;
		}
		
		ui->uploadButton->activate();
		ui->disasm->SetPC(program.exe_params.pc0, true);
		
	}
	
	if( o->Attribute("sym_file") ) {
		
		sym_file = o->Attribute("sym_file" );
		
		if( symbols.LoadSymbols(sym_file.c_str()) )
		{
			fl_message_title("Error opening project");
			fl_message("Unable to load symbols file:\n%s\n"
				"Debugging information will be unavailable.", 
				sym_file.c_str() );
		}
		else
		{
			UpdateSourceView();
		}
		
	}
	
	// Load bookmark points
	o = base->FirstChildElement("bookmarks");
	if( o )
	{
		oo = o->FirstChildElement("mark");
		while( oo )
		{
			BOOKMARK mark;
			
			const char *text = oo->Attribute("address");
			if( text ) {
				sscanf(text, "%08x", &mark.addr);
			}
			
			text = oo->Attribute("comment");
			if( text ) {
				mark.remark = text;
			}
			
			bookmarks.push_back(mark);
			
			oo = oo->NextSiblingElement("mark");
		}
	}
	
	// Load watchpoints
	o = base->FirstChildElement("watchpoints");
	if( o )
	{
		oo = o->FirstChildElement("watch");
		while( oo )
		{
			if( oo->Attribute("symbol") )
			{
				AddWatch(
					oo->Attribute("symbol"),
					oo->IntAttribute("type"),
					(int)oo->BoolAttribute("unsigned"),
					(int)oo->BoolAttribute("hex"));	
			}
			else
			{
				unsigned int addr;
				const char *text = oo->Attribute("address");
				if( text ) {
					sscanf(text, "%08x", &addr);
				}
				AddWatch(
					addr, oo->IntAttribute("type"),
					(int)oo->BoolAttribute("unsigned"),
					(int)oo->BoolAttribute("hex"));
			}
			
			oo = oo->NextSiblingElement("watch");
		}
	}
	
	// Load window positions
	o = base->FirstChildElement("debugger_win");
	if( o )
	{
		oo = o->FirstChildElement("window_pos");
		
		ui->resize(
			oo->IntAttribute("x"), 
			oo->IntAttribute("y"),
			oo->IntAttribute("w"),
			oo->IntAttribute("h"));
	}
	
	o = base->FirstChildElement("source_win");
	if( o )
	{
		if( symbols.sources.size() )
		{
			oo = o->FirstChildElement("window_pos");

			if( oo )
			{
				src_ui->resize( 
					oo->IntAttribute("x"),
					oo->IntAttribute("y"),
					oo->IntAttribute("w"), 
					oo->IntAttribute("h"));
			}
			
			ShowSourceWindow();
			
		}
	}
	
	o = base->FirstChildElement( "symbols_win" );
	if( o )
	{
		
		if( symbols.symbols.size() )
		{
			oo = o->FirstChildElement( "window_pos" );
			
			if( oo ) {
				
				sym_ui->resize( 
					oo->IntAttribute("x"),
					oo->IntAttribute("y"),
					oo->IntAttribute("w"),
					oo->IntAttribute("h"));
				
			}
			
			ShowSymbolsWindow();
		}
	}
	
	o = base->FirstChildElement( "message_win" );
	if( o ) {
			
		oo = o->FirstChildElement( "window_pos" );
		if( oo ) {

			msg_ui->resize( 
				oo->IntAttribute("x"),
				oo->IntAttribute("y"),
				oo->IntAttribute("w"), 
				oo->IntAttribute("h"));

		}

		msg_ui->icon( mwin_icon );
		msg_ui->show();
		
	}
	
	o = base->FirstChildElement( "bookmarks_win" );
	if( o ) {

		oo = o->FirstChildElement( "window_pos" );
		if( oo ) {

			bmlist_ui->resize( 
				oo->IntAttribute("x"),
				oo->IntAttribute("y"),
				oo->IntAttribute("w"), 
				oo->IntAttribute("h") );

		}

		ShowBookmarksWindow();
		
	}
	
	o = base->FirstChildElement( "memory_win" );
	if( o ) {
			
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
	
	o = base->FirstChildElement( "watch_win" );
	if( o ) {
			
		oo = o->FirstChildElement( "window_pos" );
		if( oo ) {

			watch_ui->resize( 
				oo->IntAttribute( "x", msg_ui->x() ),
				oo->IntAttribute( "y", msg_ui->y() ),
				oo->IntAttribute( "w", msg_ui->w() ), 
				oo->IntAttribute( "h", msg_ui->h() ) );

		}

		watch_ui->icon( watch_icon );
		watch_ui->show();
		
	}
	
	// Resolve watchpoints that use symbols
	for(int i=0; i<watchpoints.size(); i++)
	{
		if( watchpoints[i]->sym_name.size() ) {

			int found;
			unsigned int addr = symbols.ResolveSymbol(
				watchpoints[i]->sym_name.c_str(), found);
			
			if( !found )
			{
				fl_message_title("Symbol not found");
				fl_message("Watchpoint of symbol %s was not found.",
					watchpoints[i]->sym_name.c_str());
				
				delete watchpoints[i];
				
				watchpoints.erase(watchpoints.begin()+i);
				i--;
				continue;
			}

			watchpoints[i]->addr = addr;
			
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

void ProjectClass::AddWatch(unsigned int addr, int type, int unsign, int hex)
{
	WATCHPOINT *watch = new WATCHPOINT();
	
	watch->addr		= addr;
	watch->type		= type;
	watch->unsign	= unsign;
	watch->hex		= hex;
	watch->value	= 0;
	
	watchpoints.push_back(watch);
}

void ProjectClass::AddWatch(const char *sym, int type, int unsign, int hex)
{
	WATCHPOINT *watch = new WATCHPOINT();
	
	watch->sym_name = sym;
	watch->type		= type;
	watch->unsign	= unsign;
	watch->hex		= hex;
	watch->value	= 0;
	
	watchpoints.push_back(watch);
}