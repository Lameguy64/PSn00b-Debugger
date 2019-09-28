// Originally created on August 6, 2018, 2:26 PM

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
#include "WatchesUI.h"

#include "source.h"
#include "MemoryUI.h"
#include "RemoteMemClass.h"
#include "monitor_defs.h"
#include "pstypes.h"
#include "defaults.h"
#include "debugger.h"

#ifndef __WIN32
extern void Sleep(int msec);
#endif

	
// Windows
MainUI		*ui;
SourceUI	*src_ui;
SymbolsUI	*sym_ui;
MessageUI	*msg_ui;
MemoryUI	*mem_ui;
BookmarksUI	*bmlist_ui;
WatchesUI	*watch_ui;

// Window open status (might be redundant)
int	src_window_open = 0;
int sym_window_open = 0;
int msg_window_open = 0;
int mem_window_open = 0;
int bkm_window_open = 0;
int wtc_window_open = 0;

// Icon images for aforementioned windows
Fl_RGB_Image *ui_icon;
Fl_RGB_Image *src_icon;
Fl_RGB_Image *sym_icon;
Fl_RGB_Image *mem_icon;
Fl_RGB_Image *mwin_icon;
Fl_RGB_Image *bmark_icon;
Fl_RGB_Image *watch_icon;

// Communications/debug related
CommsClass		comms;
ProgramClass	program;
RemoteMemClass	code_mem;	// For disassembler view
RemoteMemClass	data_mem;	// For memory view


// Project
ProjectClass	project;
ConfigClass		config;


// Register buffer
unsigned int reg_buffer[40];
int run_to_cursor = false;

int browser_last_focus = 0;

int db_wait_response;
int db_wait_timeout;

void DebugControls(int enable);
void loop(void);

void initWindows(void);
void closeWindows(void);

void UpdateViews();

int searchFont(const char* searchname);
void ClearAll();

int RefreshRemoteCode(int force = false);
void RefreshRemoteData(int force = false);

void PrintMessage(const char* text);
unsigned int AddressInput(int* type);

void StepControls(int enable);
void cb_MainMinMax(int max);

extern int WatchUpdate();


int main(int argc, char** argv) {
	
	if( config.LoadConfig( "psdebug.cfg" ) == 0 ) {
		#ifdef DEBUG
		printf("Configuration loaded successfully.\n");
		#endif
	}
	
	initWindows();
	
	loop();
	
	closeWindows();
		
	comms.serial.ClosePort();
	
	return 0;
	
}



const char *UpdateCause() {
	
	const char* cause = "Unknown";

	switch( (reg_buffer[PS_REG_cause]>>2)&0x1f ) {
		case 0x1:
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

	ui->registers->SetLastState(cause);
	
	return cause;
	
}

void HandleCommsCode(int code) {
	
	switch(code) {
		
		case M_EV_PAUSE:

			UpdateViews();
			StepControls(true);
			RefreshRemoteCode(true);

			ui->registers->SetLastState("Target Stopped");
			ui->statusBar->SetStatus(NULL);

			break;
			
		case M_EV_BREAK:
			
			UpdateViews();
			StepControls(true);
			ui->registers->SetLastState("Breakpoint");
			
			break;
		
		case M_EV_DBGFAIL:
			
			UpdateViews();
			UpdateCause();

			ui->statusBar->SetStatus("Debugger Failure");

			fl_message_title("Debugger Failure");
			fl_message("Exception occurred inside debug monitor.");
			
			break;
			
		case M_EV_EXCEPT:
			
			UpdateViews();
			StepControls(true);

			const char* cause = UpdateCause();

			ui->statusBar->SetStatus("Exception Error");

			fl_message_title("Unhandled Exception");
			fl_message(cause);
			
			break;
			
	}
	
}

void loop(void) {

	if( !config.serial_dev.empty() ) {
		ui->statusBar->SetPort( config.serial_dev.c_str() );
	}
	
	if( config.ram_8mb ) { 
		
		code_mem.SetMemSize(8);
		data_mem.SetMemSize(8);
		
	} else {
		
		code_mem.SetMemSize(2);
		data_mem.SetMemSize(2);
		
	}
	
	ClearAll();
	
	ui->show();
	
	while( ui->shown() ) {
		
		Fl::wait(0.05f);
		
		char response[34];
		memset(response, 0, 34);
		
		// Debugger comms and message handling
		if( comms.serial.IsOpen() ) {
			
			int pendingBytes = comms.serial.PendingBytes();

			if( pendingBytes > 32 ) {
				pendingBytes = 32;
			}
			
			if( pendingBytes > 0 ) {

				char *cpos;
				
				comms.serial.ReceiveBytes(response, pendingBytes);
				
				// Pause event
				if( strchr(response, M_EV_PAUSE) ) {
					
					HandleCommsCode(M_EV_PAUSE);
					
					cpos = strchr(response, M_EV_PAUSE);
					
					strcpy(cpos, cpos+1);
					
					db_wait_response = false;
				
				// Debug break event
				} else if( strchr(response, M_EV_BREAK) ) {
					
					HandleCommsCode(M_EV_BREAK);
					
					cpos = strchr(response, M_EV_BREAK);
					
					strcpy(cpos, cpos+1);
					
					db_wait_response = false;
				
				// Debugger fail event
				} else if( strchr(response, M_EV_DBGFAIL) ) {
					
					HandleCommsCode(M_EV_DBGFAIL);
					
					cpos = strchr(response, M_EV_DBGFAIL);
					
					strcpy(cpos, cpos+1);
					
					db_wait_response = false;
					
				} else if( strchr(response, M_EV_EXCEPT) ) {
					
					HandleCommsCode(M_EV_EXCEPT);
					
					cpos = strchr(response, M_EV_EXCEPT);
					
					strcpy(cpos, cpos+1);
					
					db_wait_response = false;
					
				}
				
				PrintMessage(response);
				
			}
		}
		
		if( db_wait_response ) {
			
			db_wait_timeout++;
			
			if( db_wait_timeout > 500 ) {
				
				CommErrorMsg(COMM_ERR_TIMEOUT);
				
				comms.serial.ClosePort();
				DebugControls(false);
				
				db_wait_response = false;
				
			}
			
		}
		
		
	}
	
}

// Hacked in icons
#include "psdebug_icon.h"
#include "source_icon.h"
#include "symbols_icon.h"
#include "mem_icon.h"
#include "mwin_icon.h"
#include "bmark_icon.h"
#include "watch_icon.h"

void initWindows(void) {
	
	// Create windows
	ui = new MainUI();
	src_ui = new SourceUI();
	sym_ui = new SymbolsUI();
	msg_ui = new MessageUI();
	mem_ui = new MemoryUI();
	bmlist_ui = new BookmarksUI();
	watch_ui = new WatchesUI();
	
	msg_ui->init_text();
	
	// Set icons
	ui_icon		= new Fl_RGB_Image(idata_psdebug_icon.pixel_data, 16, 16, 4);
	src_icon	= new Fl_RGB_Image(idata_source_icon.pixel_data, 16, 16, 4);
	sym_icon	= new Fl_RGB_Image(idata_symbols_icon.pixel_data, 16, 16, 4);
	mem_icon	= new Fl_RGB_Image(idata_mem_icon.pixel_data, 16, 16,4);
	mwin_icon	= new Fl_RGB_Image(idata_mwin_icon.pixel_data, 16, 16, 4);
	bmark_icon	= new Fl_RGB_Image(idata_bmark_icon.pixel_data, 16, 16, 4);
	watch_icon	= new Fl_RGB_Image(idata_watch_icon.pixel_data, 16, 16, 4);
			
	ui->icon( ui_icon );
	src_ui->icon(src_icon);
	sym_ui->icon(sym_icon);
	mem_ui->icon(mem_icon);
	msg_ui->icon(mwin_icon);
	watch_ui->icon(bmlist_ui);
	bmlist_ui->icon(bmark_icon);
	watch_ui->icon(watch_icon);
	
	// Configure windows
	ui->SetMinMaxCallback( cb_MainMinMax );
	ui->size_range(400, 300);
	
	// Set registers font
	if( config.regs_font < 0 ) {
		config.regs_font = searchFont(DEFAULT_FONT);
	}
	if( config.regs_font_size < 0 ) {
		config.regs_font_size = DEFAULT_FONT_SIZE;
	}
	
	ui->registers->textfont(config.regs_font);
	ui->registers->textsize(config.regs_font_size);

	
	// Set disassembly font
	if( config.asm_font < 0 ) {
		config.asm_font = searchFont(DEFAULT_FONT);
	}
	
	if( config.asm_font_size < 0 ) {
		config.asm_font_size = DEFAULT_FONT_SIZE;
	}
	
	ui->disasm->textfont(config.asm_font);
	ui->disasm->textsize(config.asm_font_size);
	
	// Set memory browser font
	mem_ui->memory->textfont(searchFont(DEFAULT_FONT));
	mem_ui->memory->textsize(DEFAULT_FONT_SIZE);
	
	if( config.show_sym_disassembler )
		ui->symbolsToggle->set();
	
	ui->disasm->ShowSymbols((bool)config.show_sym_disassembler);
	
}

void closeWindows(void) {

	ui->hide();
	src_ui->hide();
	sym_ui->hide();
	msg_ui->hide();
	mem_ui->hide();
	bmlist_ui->hide();
	watch_ui->hide();
	
	Fl::wait();
		
	delete ui;
	delete src_ui;
	delete sym_ui;
	delete msg_ui;
	delete mem_ui;
	delete bmlist_ui;
	delete watch_ui;
	
	delete ui_icon;
	delete src_icon;
	delete sym_icon;
	delete mem_icon;
	delete mwin_icon;
	delete bmark_icon;
	delete watch_icon;
	
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

void UpdateViews() {
	
	int ret = comms.dbRegisters(reg_buffer);
	
	if( ret < 0 )
	{
		CommErrorMsg(COMM_ERR_TIMEOUT);
		return;
	}
	else
	{
		if( (ret&0xF0) == 0xC0  )
		{
			HandleCommsCode(ret);
			return;
		}
	}
	
	// Update values
	ui->registers->UpdateRegs(reg_buffer);
	ui->statusBar->SetAddress(reg_buffer[PS_REG_epc]);
	ui->disasm->SetPC(reg_buffer[PS_REG_epc], true);
	
	// Update disassembly
	if( ui->remoteMode->value() )
	{
		RefreshRemoteCode();
	}

	// Update memory view
	if( mem_ui->shown() )
	{
		RefreshRemoteData(true);
	}
	
	// Update watchpoints
	if( watch_ui->shown() )
	{
		WatchUpdate();
	}
	
	// Update source code line
	if( src_ui->shown() )
	{
		int line;
		SymbolsClass::SourceClass *source = 
			project.symbols.AddrToLine(reg_buffer[36], &line);

		if( source )
		{
			ViewSource(source, line);
		}
	}
	
}


void ClearRegs() {
	
	memset( reg_buffer, 0, 4*41 );
	reg_buffer[PS_REG_epc] = 0;
	
	ui->registers->UpdateRegs(reg_buffer);
	ui->registers->UpdateRegs(reg_buffer);
	ui->disasm->SetPC(0, true);
	
}

void ClearAll() {
	
	
	src_ui->hide();
	src_ui->sourceBrowser->clear();
	src_ui->sourceFileChoice->clear();
	
	sym_ui->hide();
	sym_ui->symbolsTable->clear();
	
	msg_ui->hide();
	msg_ui->text->text("");
	
	mem_ui->hide();
	
	bmlist_ui->hide();
	bmlist_ui->bookmarkTable->clear();
	
	watch_ui->hide();
	watch_ui->watchTable->clear();
	
	project.clear();
	program.clear();
	
	ui->redraw();
	
	DebugControls(false);
	ClearRegs();
	
}



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
		
		ui->statusBar->SetStatus("Updating disassembly...");
		
	}
	
	switch( code_mem.Update( &comms, test_addr, test_size, force ) ) {
		case 1:
			ui->statusBar->SetStatus("Done updating disassembly");
			break;
		case 2:
			ui->statusBar->SetStatus("Error updating disassembly");
			break;
		default:
			break;
	}
	
	return 0;
	
}

//// callback stuff ////

void cb_MainMinMax(int max) {
	
	if( max == 0 )
	{	
		if( config.auto_minimize )
		{
			if( src_ui->shown() )
			{
				src_ui->iconize();
				src_window_open = true;
			}
			if( sym_ui->shown() )
			{
				sym_ui->iconize();
				sym_window_open = true;
			}
			if( msg_ui->shown() )
			{
				msg_ui->iconize();
				msg_window_open = true;
			}
			if( bmlist_ui->shown() )
			{
				bmlist_ui->iconize();
				bkm_window_open = true;
			}
			if( mem_ui->shown() )
			{
				mem_ui->iconize();
				mem_window_open = true;
			}
			if( watch_ui->shown() )
			{
				watch_ui->iconize();
				wtc_window_open = true;
			}
		}
		
	}
	else
	{
		
		if( src_window_open )
		{
			src_ui->show();
		}
		if( sym_window_open )
		{
			sym_ui->show();
		}
		if( msg_window_open )
		{
			msg_ui->show();
		}
		if( bkm_window_open )
		{
			bmlist_ui->show();
		}
		if( mem_window_open )
		{
			mem_ui->show();
		}
		if( wtc_window_open )
		{
			watch_ui->show();
		}
		
		src_window_open = false;
		sym_window_open = false;
		msg_window_open = false;
		bkm_window_open = false;
		wtc_window_open = false;
		
	}
	
}

void cb_Exit(Fl_Menu_ *w, void *u) {
	
	ui->hide();
	
}

void cb_NewProject(Fl_Menu_* w, void* u) {
	
	if( comms.serial.IsOpen() ) {
		CommErrorMsg(COMM_ERR_RUNNING);
		return;
	}
	
	ClearAll();
	project.file.clear();
	ClearRegs();
	
}

void cb_OpenProject(Fl_Menu_* w, void* u) {
	
	if( comms.serial.IsOpen() )
	{
		CommErrorMsg(COMM_ERR_RUNNING);
		return;
	}
	
	Fl_Native_File_Chooser file_chooser;
	
	file_chooser.title( "Open Project" );
	file_chooser.filter( "PSn00b Debug Project\t*.pdb\n" );
	
	file_chooser.type( Fl_Native_File_Chooser::BROWSE_FILE );
	
	if( file_chooser.show() == 0 )
	{
		
		ClearAll();
		
		// Load the project
		if( project.Load( file_chooser.filename() ) )
		{
			return;
		}
		
		project.file = file_chooser.filename();
		
		for(int i=0; i<project.watchpoints.size(); i++)
		{
			watch_ui->watchTable->add_item(project.watchpoints[i]);
		}
		
	}
	
}

void cb_SaveProject(Fl_Menu_* w, void* u) {
	
	if( project.file.empty() ) {
		
		cb_SaveProjectAs(w, u);
		
	} else {
		
		if( project.Save(project.file.c_str()) ) {
			
			fl_message_title( "Error Saving Project" );
			fl_message( "Unable to save project file to specified location." );
			project.file.clear();
			
		}
		
	}
	
}

void cb_SaveProjectAs(Fl_Menu_* w, void* u) {
	
	Fl_Native_File_Chooser file_chooser;
	
	file_chooser.title("Save Project");
	file_chooser.filter("PSn00b Debug Project\t*.pdb\n");
	
	file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	file_chooser.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
	
	if( file_chooser.show() == 0 ) {
		
		// Check file extension and add if there's none
		const char *name_pos;

		name_pos = strrchr(file_chooser.filename(), '\\');
		if( !name_pos ) {

			name_pos = strrchr(file_chooser.filename(), '/');

		}

		if( name_pos ) {

			if( !strchr(name_pos, '.') ) {

				project.file = file_chooser.filename();
				project.file.append(".pdb");

			} else {

				project.file = file_chooser.filename();

			}

		} else {

			project.file = file_chooser.filename();
			project.file.append(".pdb");

		}
		
		if( project.Save(project.file.c_str()) ) {
			
			fl_message_title( "Error Saving Project" );
			fl_message( "Unable to save project file to specified location." );
			project.file.clear();
			
		}
		
	}
	
}

void cb_SelectExec(Fl_Menu_* w, void* u) {
	
	if( comms.serial.IsOpen() ) {
		CommErrorMsg(COMM_ERR_RUNNING);
		return;
	}
	
	Fl_Native_File_Chooser file_chooser;
	
	file_chooser.title("Select PS Executable");
	file_chooser.filter("All Supported Files\t*.exe;*.cpe;*.elf\n"
		"PS-EXE Executable\t*.exe\n"
		"CPE Executable\t*.cpe\n"
		"Executable and Linkable Format\t*.elf\n");
	
	if( file_chooser.show() == 0 ) {
		
		if( program.LoadProgram(file_chooser.filename()) ) {
			return;
		}
		
		project.symbols.ClearSymbols();
		ui->uploadButton->activate();
		ui->disasm->SetPC(program.exe_params.pc0, true);

		project.exe_file = file_chooser.filename();
		
		// Check if there's an accompanying symbols file
		std::string sym = file_chooser.filename();
		
		if( sym.rfind(".") != std::string::npos ) {
			
			sym.replace(sym.find("."), 4, ".sym");
			
			if( fileExists(sym.c_str()) ) {
				
				fl_message_title( "Symbols File Found" );
				int loadSymbols = false;
				if( config.autoload_symbols ) {
					
					loadSymbols = true;
					
				} else {
					
					loadSymbols = fl_choice("Load symbol file?", "No", "Yes", nullptr);
							
				}
				
				if( loadSymbols ) {
					
					fl_message_title("Error loading symbols");
					switch( project.symbols.LoadSymbols( sym.c_str() ) ) {
					case -1:
						fl_message("Cannot open symbol file.");
						return;
					case -2:
						fl_message("Unsupported symbols file format.");
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
	
	if( comms.serial.IsOpen() ) {
		
		fl_message_title("Program Still Running");
		fl_message("Close connection or restart console first.");
		return;
		
	}
	
	if( project.file.empty() ) {
		return;
	}
	
	ClearAll();
	
	if( project.Load(project.file.c_str()) ) {
		return;
	}
	
}

void cb_SymbolsToggle(Fl_Menu_ *w, void *u) {
	
	std::cout << "Symbols toggled." << std::endl;
	
	ui->disasm->ShowSymbols(ui->symbolsToggle->value());
	ui->disasm->redraw();
	
	config.show_sym_disassembler = ui->symbolsToggle->value();
	
}

void cb_SourceCode(Fl_Menu_ *w, void *u) {
	
	if( project.symbols.sources.size() == 0 ) {
		
		fl_message_title("No Source Debug Data");
		fl_message("Symbols file with source level debug data\n"
			"is required for source level debugging.");
		
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
	unsigned int addr = AddressInput(&type);
	
	#ifdef DEBUG
	std::cout << std::hex << "A=" << addr << std::dec << " T=" << type << std::endl;
	#endif
	
	if( browser_last_focus == 0 ) {
	
		switch(type) {
			case 1:	// Direct address
			case 2:
				ui->disasm->SetPos(addr, true);
				ui->statusBar->SetAddress(addr);
				break;
			case 3: // Register
				ui->disasm->SetPos(reg_buffer[addr], true);
				ui->statusBar->SetAddress(reg_buffer[addr]);
				break;
			case 4: // PC address
				ui->disasm->SetPos(reg_buffer[PS_REG_epc], true);
				ui->statusBar->SetAddress(reg_buffer[PS_REG_epc]);
				break;
		}
		
		RefreshRemoteCode();
	
	} else {
		
		switch(type) {
			case 1:	// Direct address
			case 2:
				mem_ui->memory->SetPos(addr, true);
				break;
			case 3: // Register
				mem_ui->memory->SetPos(reg_buffer[addr], true);
				break;
			case 4: // PC address
				mem_ui->memory->SetPos(reg_buffer[PS_REG_epc], true);
				break;
		}
		
		RefreshRemoteData();
		
	}
	
}

void cb_Upload(Fl_Button* w, void* u) {

	if( comms.serial.IsOpen() )
	{
		CommErrorMsg(COMM_ERR_RUNNING);
		return;
	}
	
	if( config.serial_dev.empty() )
	{
		fl_message_title("No Serial Device");
		fl_message("No serial device configured.");
		ui->statusBar->SetStatus("Upload error");
		
		return;
	}
	
	ui->statusBar->SetStatus("Uploading");
	
	if( comms.serial.OpenPort(config.serial_dev.c_str() )
		!= SerialClass::ERROR_NONE )
	{	
		fl_message_title("Error Opening Port");
		fl_message("Cannot open serial port.");
		ui->statusBar->SetStatus("Upload error");
		
		return;
	}
	
	// Upload debug monitor patch
	if( config.patch_enable )
	{
		if( config.patch_file.empty() )
		{
			comms.serial.ClosePort();
			ui->statusBar->SetStatus("Upload error");
			
			fl_message_title("No Patch File");
			fl_message("No patch file specified.");
			
			return;
		}
		
		if( comms.UploadPatch(config.patch_file.c_str()) )
		{
			#ifdef DEBUG
			std::cout << "Error patching." << std::endl;
			#endif

			comms.serial.ClosePort();
			ui->statusBar->SetStatus("Upload error");
			return;
		}
		
		// Delay before sending program
		Sleep(20);
	}
	
	// Upload program code
	if( comms.UploadProgram(&program) )
	{
		#ifdef DEBUG
		std::cout << "Error uploading." << std::endl;
		#endif

		comms.serial.ClosePort();
		ui->statusBar->SetStatus("Upload error");
		
		return;
	}
	
	// Wait for a start response from the debugger
	if( comms.dbWaitBegin() )
	{
		CommErrorMsg(COMM_ERR_NOREPLY);
		return;
	}
	
	// Activate debugger controls
	DebugControls(true);
	StepControls(false);
	
	ui->statusBar->SetStatus("Program Loaded");
	watch_ui->watchTable->values_active(true);
	watch_ui->updateButton->activate();
	
	// Update remote memory view
	if( mem_ui->shown() )
	{
		// Delay otherwise remote data view will hang itself
		Sleep(200); 
		RefreshRemoteData();
	}
	
	db_wait_response = false;
	db_wait_timeout = 0;
	
}