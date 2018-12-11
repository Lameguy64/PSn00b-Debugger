/* 
 * File:   main.cpp
 * Author: Lameguy64
 *
 * Created on August 6, 2018, 2:26 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <iostream>
#include <Fl/Fl_Native_File_Chooser.H>
#include <Fl/fl_message.H>
#include "mips_disassembler.h"

#include "ProjectClass.h"
#include "ConfigClass.h"
#include "CommsClass.h"
#include "SymbolsClass.h"

#include "MainUI.h"
#include "SourceUI.h"
#include "AboutUI.h"
#include "SymbolsUI.h"
#include "MessageUI.h"
#include "BookmarksUI.h"

#include "source.h"
#include "MemoryUI.h"
#include "RemoteMemClass.h"

	
// Windows
MainUI		*ui;
SourceUI	*src_ui;
SymbolsUI	*sym_ui;
MessageUI	*msg_ui;
MemoryUI	*mem_ui;
BookmarksUI	*bmlist_ui;

// Icon images for aforementioned windows
Fl_RGB_Image *ui_icon;
Fl_RGB_Image *src_icon;
Fl_RGB_Image *sym_icon;
Fl_RGB_Image *mem_icon;
Fl_RGB_Image *mwin_icon;
Fl_RGB_Image *bmark_icon;

// Communications/debug related
CommsClass		comms;
ProgramClass	program;
RemoteMemClass	code_mem;	// For disassembler view
RemoteMemClass	data_mem;	// For memory view

// Project
std::string		project_file;
ProjectClass	project;
ConfigClass		config;

int	src_window_open = 0;
int sym_window_open = 0;
int msg_window_open = 0;
int mem_window_open = 0;
int bkm_window_open = 0;

unsigned int reg_buffer[38];
int run_to_cursor = false;

int browser_last_focus = 0;

void initWindows();
void updateViews();


int searchFont(const char* searchname);
void ClearAll();

int RefreshRemoteCode(int force = false);
void RefreshRemoteData(int force = false);

void PrintMessage(const char* text);
unsigned int AddressInput(int* type);

void setStep(int enable);
void cb_MainMinMax(int max);


#ifndef __WIN32

extern void Sleep(int msec);

#define DEFAULT_FONT		"mono"
#define DEFAULT_FONT_SIZE	11

#else

#define DEFAULT_FONT		"Courier New"
#define DEFAULT_FONT_SIZE	12

#endif



void activateDebug() {
	
	ui->execControls->activate();
	setStep( true );
	
	ui->regRefresh->activate();
	ui->rebootButton->activate();
	
	ui->connectButton->deactivate();
	ui->disconnectButton->activate();
	
	ui->remoteMode->activate();
	
	ui->statusBar->SetConnected( 1 );
	
	Fl::wait();
	
}

void deactivateDebug() {
	
	ui->execControls->deactivate();
	setStep( false );
	
	ui->codeRefresh->deactivate();
	ui->regRefresh->deactivate();
	ui->rebootButton->deactivate();
	
	ui->connectButton->activate();
	ui->disconnectButton->deactivate();
	
	ui->remoteMode->deactivate();
	ui->localMode->value( 1 );
	ui->remoteMode->value( 0 );
	
	ui->statusBar->SetConnected( 0 );
	mem_ui->memory->redraw();
	
	Fl::wait();
	
}

int main(int argc, char** argv) {
	
	if( config.LoadConfig( "psdebug.cfg" ) == 0 ) {
		#ifdef DEBUG
		std::cout << "Configuration loaded successfully." << std::endl;
		#endif
	}
	
	initWindows();
	
	if( !config.serial_dev.empty() ) {
		ui->statusBar->SetPort( config.serial_dev.c_str() );
	}
	
	if( config.ram_8mb ) { 
		code_mem.SetMemSize( 8 );
		data_mem.SetMemSize( 8 );
	} else {
		code_mem.SetMemSize( 2 );
		data_mem.SetMemSize( 2 );
	}
	
	ClearAll();
	
	
	ui->show();
	
	while( ui->shown() ) {
		
		Fl::wait( 0.05f );
		
		char response[34];
		memset( response, 0, 34 );
		
		if( comms.serial.IsOpen() ) {
			
			int pendingBytes = comms.serial.PendingBytes();

			if( pendingBytes > 32 ) {
				pendingBytes = 32;
			}
			
			if( pendingBytes > 0 ) {

				comms.serial.ReceiveBytes( response, pendingBytes );
				
				// Begin response
				if( strstr( response, "~BGN" ) ) {

					updateViews();
					activateDebug();
					setStep( true );
					
					ui->registers->SetLastState( "Program Begin" );
					
				} else if( strstr( response, "~SUS" ) ) {

					updateViews();
					setStep( true );
					RefreshRemoteCode( true );

					ui->registers->SetLastState( "Target Stopped" );
					ui->statusBar->SetStatus( nullptr );
					
				// BPC encountered (breakpoints in the future)
				} else if( strstr( response, "~BRK" ) ) {

					updateViews();
					setStep( true );
					
					ui->registers->SetLastState( "Breakpoint" );
					
				// Exception encountered
				} else if( strstr( response, "~EXC" ) ) {

					#ifdef DEBUG
					std::cout << "Exception!" << std::endl;
					#endif
					
					updateViews();

					const char* cause = "Unknown";

					switch( (reg_buffer[35]>>2)&0x1f ) {
						case 0x01:
							cause = "TLB Modification";
							break;
						case 0x2:
							cause = "TLB Load";
							break;
						case 0x3:
							cause = "TLB Store";
							break;
						case 0x4:
							cause = "Address Error on Load";
							break;
						case 0x5:
							cause = "Address Error on Store";
							break;
						case 0x6:
							cause = "Bus error on Instruction Fetch";
							break;
						case 0x7:
							cause = "Bus error on Data Load/Store";
							break;
						case 0x8:
							cause = "Syscall";
							break;
						case 0x9:
							cause = "Break instruction";
							break;
						case 0xa:
							cause = "Reserved Instruction";
							break;
						case 0xb:
							cause = "Coprocessor Unusable";
							break;
						case 0xc:
							cause = "Arithmetic Overflow";
							break;
					}

					ui->registers->SetLastState( cause );
					ui->statusBar->SetStatus( "Exception Error" );
					
					fl_message_title( "Unhandled Exception" );
					fl_message( cause );
					
				}
				
				if( strchr( response, '~' ) ) {

					char *pos = strchr( response, '~' );
					memmove( pos, pos+4, strlen( pos+4 )+1 );
					
				}
				
				PrintMessage( response );

			}
		}
	}
	
	ui->hide();
	src_ui->hide();
	sym_ui->hide();
	msg_ui->hide();
	mem_ui->hide();
	bmlist_ui->hide();
	
	Fl::wait();
	
	delete ui;
	delete src_ui;
	delete sym_ui;
	delete msg_ui;
	delete mem_ui;
	delete bmlist_ui;
	
	delete ui_icon;
	delete src_icon;
	delete sym_icon;
	delete mem_icon;
	delete mwin_icon;
	delete bmark_icon;
	
	comms.serial.ClosePort();
	
	return 0;
	
}

// Hacked in icons
#include "psdebug_icon.h"
#include "source_icon.h"
#include "symbols_icon.h"
#include "mem_icon.h"
#include "mwin_icon.h"
#include "bmark_icon.h"

void initWindows() {
	
	ui = new MainUI();
	src_ui = new SourceUI();
	sym_ui = new SymbolsUI();
	msg_ui = new MessageUI();
	mem_ui = new MemoryUI();
	bmlist_ui = new BookmarksUI();
	msg_ui->init_text();
	
	ui_icon		= new Fl_RGB_Image( idata_psdebug_icon.pixel_data, 16, 16, 4 );
	src_icon	= new Fl_RGB_Image( idata_source_icon.pixel_data, 16, 16, 4 );
	sym_icon	= new Fl_RGB_Image( idata_symbols_icon.pixel_data, 16, 16, 4 );
	mem_icon	= new Fl_RGB_Image( idata_mem_icon.pixel_data, 16, 16,4 );
	mwin_icon	= new Fl_RGB_Image( idata_mwin_icon.pixel_data, 16, 16, 4 );
	bmark_icon	= new Fl_RGB_Image( idata_bmark_icon.pixel_data, 16, 16, 4 );
	
	ui->icon( ui_icon );
	src_ui->icon( src_icon );
	sym_ui->icon( sym_icon );
	mem_ui->icon( mem_icon );
	msg_ui->icon( mwin_icon );
	bmlist_ui->icon( bmark_icon );
	
	ui->SetMinMaxCallback( cb_MainMinMax );
	ui->disasm->textfont( searchFont( DEFAULT_FONT ) );
	ui->disasm->textsize( DEFAULT_FONT_SIZE );
	ui->registers->textfont( searchFont( DEFAULT_FONT ) );
	ui->registers->textsize( DEFAULT_FONT_SIZE );
	mem_ui->memory->textfont( searchFont( DEFAULT_FONT ) );
	mem_ui->memory->textsize( DEFAULT_FONT_SIZE );
	
}

int searchFont(const char* searchname) {
	
	int i = 0;
	int k = Fl::set_fonts(i ? (i>1 ? "*" : 0) : "-*");
	int t;
	int index = -1;
	
	for( i=0; i<k; i++ ) {
		
		const char* name = Fl::get_font_name( (Fl_Font)i, &t );
		if( strcasecmp( searchname, name ) == 0 ) {
			index = i;
			break;
		}
		
	}
	
	return index;
	
}

int fileExists( const char* fileName ) {
	
	struct stat fd;
	
	if( stat( fileName, &fd ) == 0 ) {
		return -1;
	}
	
	return 0;
}

void updateViews() {
	
	comms.dbRegisters( reg_buffer );
	
	ui->registers->UpdateRegs( reg_buffer );
	ui->statusBar->SetAddress( reg_buffer[36] );
	ui->disasm->SetPC( reg_buffer[36], true );
	
	if( mem_ui->shown() ) {
		
		RefreshRemoteData( true );
		
	}
	
	RefreshRemoteCode();
	
	if( src_ui->shown() ) {

		int line;
		SymbolsClass::SourceClass *source = 
			project.symbols.AddrToLine( reg_buffer[36], &line );

		if( source ) {
			ViewSource( source, line );
		}

	}
	
}


void ClearRegs() {
	
	memset( reg_buffer, 0, 4*38 );
	reg_buffer[36] = 0x80000000;
	
	ui->registers->UpdateRegs( reg_buffer );
	ui->disasm->SetPC( 0x80010000, true );
}

void ClearAll() {
	
	src_ui->hide();
	src_ui->sourceBrowser->clear();
	src_ui->sourceFileChoice->clear();
	
	sym_ui->hide();
	sym_ui->symbolsTable->clear();
	
	msg_ui->hide();
	msg_ui->text->text( "" );
	
	mem_ui->hide();
	
	bmlist_ui->hide();
	bmlist_ui->bookmarkTable->clear();
	
	project.clear();
	program.clear();
	
	deactivateDebug();
	
	ui->redraw();
	
}

void setStep(int enable) {
	
	if( enable ) {
		
		ui->stepButton->activate();
		ui->stepOverButton->activate();
		ui->stepOutButton->activate();
		
	} else {
		
		ui->stepButton->deactivate();
		ui->stepOverButton->deactivate();
		ui->stepOutButton->deactivate();
		
	}
	
}

#define RAM_SIZE	1048576*2

int addr_valid(unsigned int addr) {
	
	int ram_ok = false;
	
	// Check if in RAM region
	switch( (addr>>24) ) {
		case 0x00:
		case 0x80:
		case 0xa0:
			ram_ok = true;
			break;
	}
	
	if( ram_ok ) {
		
		if( ( addr&0x00ffffff ) < RAM_SIZE ) {
			return true;
		}
		
		return false;
	}
	
	// Check if in BIOS ROM region
	if( (addr>>20) == 0xbfc ) {
		if( ( addr&0x000fffff ) < 524288 ) {
			return true;
		}
	}
	
	return false;
}

int RefreshRemoteCode(int force) {
	
	if ( !ui->remoteMode->value() ) {
		return 0;
	}
	
	fl_font( ui->disasm->textfont(), ui->disasm->textsize() );
	
	int test_size = (4*(ui->disasm->h()/fl_height()))+2;
	unsigned int test_addr = (ui->disasm->scroll_pos<<2);
	
	if( ( code_mem.TestUpdate( test_addr, test_size ) ) || ( force ) ) {
		
		ui->statusBar->SetStatus( "Updating disassembly..." );
		
	}
	
	switch( code_mem.Update( &comms, test_addr, test_size, force ) ) {
		case 1:
			ui->statusBar->SetStatus( "Done updating." );
			break;
		case 2:
			ui->statusBar->SetStatus( "Error updating." );
			break;
		default:
			break;
	}
	
}

void cb_MainMinMax(int max) {
	
	if( max == 0 ) {
		
		if( config.auto_minimize ) {
			
			if( src_ui->shown() ) {
				src_ui->iconize();
				src_window_open = true;
			}
			if( sym_ui->shown() ) {
				sym_ui->iconize();
				sym_window_open = true;
			}
			if( msg_ui->shown() ) {
				msg_ui->iconize();
				msg_window_open = true;
			}
			if( bmlist_ui->shown() ) {
				bmlist_ui->iconize();
				bkm_window_open = true;
			}
			if( mem_ui->shown() ) {
				mem_ui->iconize();
				mem_window_open = true;
			}
		}
		
	} else {
		
		if( src_window_open ) {
			src_ui->show();
		}
		if( sym_window_open ) {
			sym_ui->show();
		}
		if( msg_window_open ) {
			msg_ui->show();
		}
		if( bkm_window_open ) {
			bmlist_ui->show();
		}
		if( mem_window_open ) {
			mem_ui->show();
		}
		
		src_window_open = false;
		sym_window_open = false;
		msg_window_open = false;
		bkm_window_open = false;
		
	}
	
}

void cb_Exit(Fl_Menu_ *w, void *u) {
	
	ui->hide();
	
}

void cb_NewProject(Fl_Menu_* w, void* u) {
	
	ClearAll();
	project_file.clear();
	ClearRegs();
	
}

void cb_OpenProject(Fl_Menu_* w, void* u) {
	
	Fl_Native_File_Chooser file_chooser;
	
	file_chooser.title( "Open Project" );
	file_chooser.filter( "PSn00b Debug Project\t*.pdb\n" );
	
	file_chooser.type( Fl_Native_File_Chooser::BROWSE_FILE );
	
	if( file_chooser.show() == 0 ) {
		
		ClearAll();
		
		if( project.Load( file_chooser.filename() ) ) {
			return;
		}
		
		project_file = file_chooser.filename();
		
	}
	
}

void cb_SaveProject(Fl_Menu_* w, void* u) {
	
	if( project_file.empty() ) {
		
		cb_SaveProjectAs( w, u );
		
	} else {
		
		if( project.Save( project_file.c_str() ) ) {
			
			fl_message_title( "Error Saving Project" );
			fl_message( "Unable to save project file to specified location." );
			project_file.clear();
			
		}
		
	}
	
}

void cb_SaveProjectAs(Fl_Menu_* w, void* u) {
	
	Fl_Native_File_Chooser file_chooser;
	
	file_chooser.title( "Save Project" );
	file_chooser.filter( "PSn00b Debug Project\t*.pdb\n" );
	
	file_chooser.type( Fl_Native_File_Chooser::BROWSE_SAVE_FILE );
	file_chooser.options( Fl_Native_File_Chooser::SAVEAS_CONFIRM );
	
	if( file_chooser.show() == 0 ) {
		
		// Check file extension and add if there's none
		const char *name_pos;

		name_pos = strrchr( file_chooser.filename(), '\\' );
		if( !name_pos ) {

			name_pos = strrchr( file_chooser.filename(), '/' );

		}

		if( name_pos ) {

			if( !strchr( name_pos, '.' ) ) {

				project_file = file_chooser.filename();
				project_file.append( ".pdb" );

			} else {

				project_file = file_chooser.filename();

			}

		} else {

			project_file = file_chooser.filename();
			project_file.append( ".pdb" );

		}
		
		if( project.Save( project_file.c_str() ) ) {
			
			fl_message_title( "Error Saving Project" );
			fl_message( "Unable to save project file to specified location." );
			project_file.clear();
			
		}
		
	}
	
}

void cb_SelectExec(Fl_Menu_* w, void* u) {
	
	Fl_Native_File_Chooser file_chooser;
	
	file_chooser.title( "Select PS Executable" );
	file_chooser.filter( "All Supported Files\t*.exe;*.cpe;*.elf\n"
		"PS-EXE Executable\t*.exe\n"
		"CPE Executable\t*.cpe\n"
		"Executable and Linkable Format\t*.elf\n");
	
	if( file_chooser.show() == 0 ) {
		
		if( program.LoadProgram( file_chooser.filename() ) ) {
			return;
		}
		
		project.symbols.ClearSymbols();
		ui->uploadButton->activate();
		ui->disasm->SetPC( program.exe_params.pc0, true );

		project.exe_file = file_chooser.filename();
		
		// Check if there's an accompanying symbols file
		std::string sym = file_chooser.filename();
		
		if( sym.rfind( "." ) != std::string::npos ) {
			
			sym.replace( sym.find( "." ), 4, ".sym" );
			
			if( fileExists( sym.c_str() ) ) {
				
				fl_message_title( "Symbols File Found" );
				int loadSymbols = false;
				if( config.autoload_symbols ) {
					
					loadSymbols = true;
					
				} else {
					
					loadSymbols = fl_choice( "Load symbol file?", "No", "Yes", nullptr );
							
				}
				
				if( loadSymbols ) {
					
					fl_message_title( "Error loading symbols" );
					switch( project.symbols.LoadSymbols( sym.c_str() ) ) {
					case -1:
						fl_message( "Cannot open symbol file." );
						return;
					case -2:
						fl_message( "Unsupported symbols file format." );
						return;
					}
					
					UpdateSourceView();
					project.sym_file = sym;
					ui->disasm->redraw();
					
				}
				
			}
			
		}
		
	}
	
}

void cb_ReloadProject(Fl_Menu_* w, void* u) {
	
	if( project_file.empty() ) {
		return;
	}
	
	ClearAll();
	
	if( project.Load( project_file.c_str() ) ) {
		return;
	}
	
}

void cb_SourceCode(Fl_Menu_ *w, void *u) {
	
	if( project.symbols.sources.size() == 0 ) {
		
		fl_message_title( "No Source Debug Data" );
		fl_message( "Symbols file with source level debug data\n"
			"is required for source level debugging." );
		
		return;
		
	}
	
	ShowSourceWindow();
	
}

void cb_Symbols(Fl_Menu_ *w, void *u) {
	
	if( project.symbols.symbols.size() == 0 ) {
		
		fl_message_title( "No Symbols Data" );
		fl_message( "Symbols file is required for browsing\n"
			"symbol names." );
		
		return;
		
	}
	
	ShowSymbolsWindow();
	
}

void cb_GotoAddress(Fl_Menu_* w, void* u) {
	
	int type;
	unsigned int addr = AddressInput( &type );
	
	#ifdef DEBUG
	std::cout << std::hex << "A=" << addr << std::dec << " T=" << type << std::endl;
	#endif
	
	if( browser_last_focus == 0 ) {
	
		switch( type ) {
			case 1:	// Direct address
			case 2:
				ui->disasm->SetPos( addr, true );
				ui->statusBar->SetAddress( addr );
				break;
			case 3: // Register
				ui->disasm->SetPos( reg_buffer[addr], true );
				ui->statusBar->SetAddress( reg_buffer[addr] );
				break;
			case 4: // PC address
				ui->disasm->SetPos( reg_buffer[37], true );
				ui->statusBar->SetAddress( reg_buffer[37] );
				break;
		}
		
		RefreshRemoteCode();
	
	} else {
		
		switch( type ) {
			case 1:	// Direct address
			case 2:
				mem_ui->memory->SetPos( addr, true );
				break;
			case 3: // Register
				mem_ui->memory->SetPos( reg_buffer[addr], true );
				break;
			case 4: // PC address
				mem_ui->memory->SetPos( reg_buffer[37], true );
				break;
		}
		
		RefreshRemoteData();
		
	}
	
}

/////////////////////////////////////////////////////////////////////////////

void cb_RunToCursor(Fl_Widget *w, void *u) {
	
	unsigned int addr = ui->disasm->GetPos()<<2;
	comms.dbRunTo( addr );
	
}

void cb_GotoSourceLine(Fl_Widget *w, void *u) {
	
	int line;
	
	SymbolsClass::SourceClass *source = 
		project.symbols.AddrToLine( ui->disasm->GetPos()<<2, &line );
	
	if( source ) {
		
		ViewSource( source, line );
		
	}
	
}

void cb_GotoJumpDest(Fl_Widget *w, void *u) {
	
	Fl_Disassembler *ct = (Fl_Disassembler*)w;
	
	unsigned int addr = ct->GetPos()<<2;
	
	if( addr<program.exe_params.t_addr ) {
		return;
	}
	if( addr>(program.exe_params.t_addr+program.exe_params.t_size ) ) {
		return;
	}
	
	addr = mips_GetJumpAddr( addr, 
		program.exe_data[(addr-program.exe_params.t_addr)>>2] );
	
	if( addr ) {
		ct->SetPos( addr, true );
		ui->statusBar->SetAddress( addr );
	}
	
	RefreshRemoteCode();
	
}

void cb_SetBookmark(Fl_Widget *w, void *u) {

	//comms.dbSetBreak( ui->disassembler->GetPos()<<2 );
	
	int exists = false;
	
	for( int i=0; i<project.bookmarks.size(); i++ ) {
		
		if( ( ui->disasm->GetPos()<<2 ) == project.bookmarks[i].addr ) {
			
			exists = true;
			project.bookmarks.erase( project.bookmarks.begin()+i );
			
		}
		
	}
	
	if( !exists ) {
		
		ProjectClass::BOOKMARK bookmark;

		fl_message_title( "Set Bookmark" );
		const char* c = fl_input( "Input a comment for this bookmark.", NULL );
		if( c ) {
			bookmark.remark = c;
		}
		bookmark.addr = ui->disasm->GetPos()<<2;
		project.bookmarks.push_back( bookmark );
		
	}
	
	bmlist_ui->bookmarkTable->clear();
	for( int i=0; i<project.bookmarks.size(); i++ ) {
		bmlist_ui->bookmarkTable->additem( &project.bookmarks[i] );
	}
	bmlist_ui->bookmarkTable->autowidth( 20 );
	
}

void cb_Disassembly(Fl_Disassembler *w, void *u) {
	
	if( Fl::event_button3() ) {
		
		Fl_Menu_Item pulldown[] = {
			{ "Run to Cursor", 0, cb_RunToCursor, 0, 0, 0, FL_HELVETICA, 12 },
			{ "Set/Clear Bookmark", 0, cb_SetBookmark, 0, FL_MENU_DIVIDER, 0, FL_HELVETICA, 12 },
			{ "Goto Jump/Branch Target", 0, cb_GotoJumpDest, 0, 0, 0, FL_HELVETICA, 12 },
			{ "Goto Source Line", 0, cb_GotoSourceLine, 0, 0, 0, FL_HELVETICA, 12 },
			{ 0 }
		};
		
		if( project.symbols.sources.size() == 0 ) {
			pulldown[3].deactivate();
		}
		
		const Fl_Menu_Item* item = pulldown->popup( Fl::event_x(), Fl::event_y() );
		
		if( item ) {
			if( item->callback() ) {
				item->callback()( w, nullptr );
			}
		}
		
		return;
		
	}
	
	ui->statusBar->SetAddress( w->GetPos()<<2 );
	RefreshRemoteCode();
	
}

/////////////////////////////////////////////////////////////////////////////

void cb_Upload(Fl_Button* w, void* u) {
	
	ui->statusBar->SetStatus( "Uploading" );
	
	if( config.serial_dev.empty() ) {
		fl_message_title( "No Serial Device" );
		fl_message( "No serial device set." );
		ui->statusBar->SetStatus( "Upload error" );
		return;
	}
	
	if( comms.serial.OpenPort( config.serial_dev.c_str() ) != SerialClass::OK ) {
		fl_message_title( "Error Opening Port" );
		fl_message( "Cannot open serial port." );
		ui->statusBar->SetStatus( "Upload error" );
		return;
	}
		
	if( config.patch_enable ) {
		
		if( config.patch_file.empty() ) {
			fl_message_title( "No Patch File" );
			fl_message( "No patch file specified." );
			comms.serial.ClosePort();
			ui->statusBar->SetStatus( "Upload error" );
			return;
		}
		
		if( comms.UploadPatch( config.patch_file.c_str() ) ) {
			#ifdef DEBUG
			std::cout << "Error patching." << std::endl;
			#endif
			comms.serial.ClosePort();
			ui->statusBar->SetStatus( "Upload error" );
			return;
		}
		
	}
	
	Sleep( 20 );
	
	if( comms.UploadProgram( &program ) ) {
		#ifdef DEBUG
		std::cout << "Error uploading." << std::endl;
		#endif
		comms.serial.ClosePort();
		ui->statusBar->SetStatus( "Upload error" );
		return;
	}
	
	/*activateDebug();
	
	if( mem_ui->shown() ) {
		Sleep(200); // Delay otherwise remote data view will hang itself
		RefreshRemoteData();
	}*/
	
	ui->statusBar->SetStatus( "Upload ok" );
	
}

void cb_commConnect(Fl_Button* w, void* u) {
	
	/*if( config.serial_dev.empty() ) {
		fl_message_title( "No Serial Device" );
		fl_message( "No serial device set." );
		return;
	}
	
	if( comms.serial.OpenPort( config.serial_dev.c_str() ) != SerialClass::OK ) {
		fl_message_title( "Error Opening Port" );
		fl_message( "Cannot open serial port." );
		return;
	}
	
	if( comms.dbTest() ) {
		
		comms.serial.ClosePort();
		
		fl_message_title( "Target Not Detected" );
		fl_message( "Target is not responding." );
		return;
		
	}*/
	
	activateDebug();
	
	if( mem_ui->shown() ) {
		RefreshRemoteData();
	}
	
}

void cb_commDisconnect(Fl_Button* w, void* u) {
	
	comms.serial.ClosePort();
	deactivateDebug();
	
}

void cb_Reboot(Fl_Button* w, void* u) {
	
	comms.dbReboot();
	ui->registers->SetLastState( "Reboot & Disconnect" );
	
	comms.serial.ClosePort();
	deactivateDebug();
	
}

void cb_Pause(Fl_Button* w, void* u) {
	
	comms.dbPause();
	
}

void cb_StepM(Fl_Menu_* w, void* u) { cb_Step((Fl_Button*)w, u); }
void cb_Step(Fl_Button* w, void* u) {
	
	if( !ui->stepButton->active() ) {
		return;
	}
	
	unsigned int pc = mips_GetNextPc( reg_buffer, false );
	#ifdef DEBUG
	std::cout << "Step to PC=" << std::hex << pc << std::dec << std::endl;
	#endif
	comms.dbRunTo( pc );
	
	run_to_cursor = false;
	
	Sleep( 100 );
	
}

void cb_StepOverM(Fl_Menu_* w, void* u) { cb_StepOver((Fl_Button*)w, u); }
void cb_StepOver(Fl_Button* w, void* u) {
	
	if( !ui->stepOverButton->active() ) {
		return;
	}
	
	unsigned int pc = mips_GetNextPc( reg_buffer, true );
	#ifdef DEBUG
	std::cout << "Step to PC=" << std::hex << pc << std::dec << std::endl;
	#endif
	comms.dbRunTo( pc );
	
	run_to_cursor = false;
	
	Sleep( 100 );
	
}

void cb_StepOutM(Fl_Menu_* w, void* u) { cb_StepOut((Fl_Button*)w, u); }
void cb_StepOut(Fl_Button* w, void* u) {
	
	if( !ui->stepOutButton->active() ) {
		return;
	}
	
	#ifdef DEBUG
	std::cout << "Step out to ra=" << std::hex << reg_buffer[31] << std::dec << std::endl;
	#endif
	comms.dbRunTo( reg_buffer[31] );
	
	Sleep( 100 );
	
}

void cb_ContinueM(Fl_Menu_* w, void* u) { cb_Continue((Fl_Button*)w, u); }
void cb_Continue(Fl_Button* w, void* u) {
	
	if( !ui->runButton->active() ) {
		return;
	}
	
	comms.dbContinue();
	ui->registers->SetLastState( "Running" );
	
	setStep( false );
	
	ui->statusBar->SetStatus( nullptr );
	
	Sleep( 100 );
	
}

void cb_RefreshRegs(Fl_Button* w, void* u) {
	
	comms.dbRegisters( reg_buffer );
	
	ui->registers->UpdateRegs( reg_buffer );
	
	ui->disasm->SetPC(reg_buffer[36], true);
	
	Sleep( 100 );
	
}

void cb_DataView(Fl_Light_Button* w, long userdata) {
	
	#ifdef DEBUG
	std::cout << "Mode = " << userdata << std::endl;
	#endif
	
	if ( userdata == 1 ) {
		
		if ( !RefreshRemoteCode() ) {
			return;
		}
		
		ui->disasm->redraw();
		ui->codeRefresh->activate();
		
	} else {
		
		ui->codeRefresh->deactivate();
		
	}
	
	ui->disasm->redraw();
	
}

void cb_RefreshCode(Fl_Button* w, void* u) {
	
	RefreshRemoteCode( true );
	
}