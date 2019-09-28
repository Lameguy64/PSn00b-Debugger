/* 
 * File:   settings.cpp
 * Author: Lameguy64
 *
 * Created on December 10, 2018, 3:22 PM
 */

#include <Fl/Fl_Menu_.H>
#include <Fl/Fl_Native_File_Chooser.H>
#include <Fl/fl_message.H>
#include <string.h>
#include <iostream>
#include "MemDumpUI.h"
#include "RemoteMemClass.h"
#include "CommsClass.h"
#include "Fl_MemoryView.h"

extern RemoteMemClass code_mem;
extern CommsClass comms;

static MemDumpUI *memdump_ui;


void cb_DumpMemory(Fl_Menu_* w, void* u) {
	
	memdump_ui = new MemDumpUI();
	
	memdump_ui->show();
	
	while( memdump_ui->shown() ) {
		
		Fl::wait();
		
	}
	
	delete memdump_ui;
	
}

void cb_MemDumpClose(Fl_Button* w, void* u) {
	
	memdump_ui->hide();
	
}

void cb_MemDumpGo(Fl_Return_Button* w, void* u) {
	
	unsigned int dump_addr;
	unsigned int dump_len;
	const char *tval;
	FILE *fp;
	
	fl_message_title( "Invalid Parameters" );
	
	if( strlen( memdump_ui->dumpAddress->value() ) == 0 ) {
		fl_message( "No address specified." );
		return;
	}
	
	if( strlen( memdump_ui->dumpLength->value() ) == 0 ) {
		fl_message( "No length specified." );
		return;
	}
	
	// Get the values
	tval = memdump_ui->dumpAddress->value();
	if( strchr( tval, '$' ) ) {
		tval++;
		sscanf( tval, "%x", &dump_addr );
	} else if( strstr( tval, "0x" ) ) {
		sscanf( tval, "%x", &dump_addr );
	} else {
		sscanf( tval, "%d", &dump_addr );
	}
	
	tval = memdump_ui->dumpLength->value();
	if( strchr( tval, '$' ) ) {
		tval++;
		sscanf( tval, "%x", &dump_len );
	} else if( strstr( tval, "0x" ) ) {
		sscanf( tval, "%x", &dump_len );
	} else {
		sscanf( tval, "%d", &dump_len );
	}
	
#ifdef DEBUG
	std::cout << "Dump " << std::hex << dump_addr 
		<< ":" << dump_len << std::dec << std::endl;
#endif
	
	// Check if range is valid
	if( !memdump_ui->noAddrCheck->value() ) {
		
		if( !code_mem.AddressValid( dump_addr ) ) {
			fl_message( "Base address invalid." );
			return;
		}

		if( !code_mem.AddressValid( dump_addr+(dump_len-1) ) ) {
			fl_message( "Length exceeds segment range." );
			return;
		}
		
	}
	
	
	Fl_Native_File_Chooser file_chooser;
	
	file_chooser.title( "Save Memory Dump As" );
	file_chooser.filter( "All Files\t*.*\n" );
	
	file_chooser.type( Fl_Native_File_Chooser::BROWSE_SAVE_FILE );
	file_chooser.options( Fl_Native_File_Chooser::SAVEAS_CONFIRM );
	
	if( file_chooser.show() != 0 ) {
		return;
	}
	
	fp = fopen( file_chooser.filename(), "wb" );
	
	fl_message_title( "Download error" );
	
	if( comms.DownloadMemory( dump_addr, dump_len, fp ) <= 0 ) {
		fl_message( "No response from console." );
	}
	
	fclose( fp );
	
}