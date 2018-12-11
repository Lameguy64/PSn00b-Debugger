/* 
 * File:   settings.cpp
 * Author: Lameguy64
 *
 * Created on August 22, 2018, 8:05 PM
 */

#include <stdio.h>
#include <Fl/Fl_Native_File_Chooser.H>
#include <Fl/fl_message.H>

#include "ConfigClass.h"
#include "SerialClass.h"
#include "SettingsUI.h"
#include "MainUI.h"
#include "SerialClass.h"
#include "MemoryUI.h"
#include "RemoteMemClass.h"

extern MainUI *ui;
extern MemoryUI *mem_ui;
extern ConfigClass config;
extern RemoteMemClass code_mem;
extern RemoteMemClass data_mem;

static SettingsUI* setup_ui;

void cb_DebugSettings(Fl_Menu_ *w, void *u) {
	
	setup_ui = new SettingsUI();
	
	setup_ui->position( 
		ui->x()+(ui->w()>>1)-(setup_ui->w()>>1),
		ui->y()+(ui->h()>>1)-(setup_ui->h()>>1) );
	
#ifdef __WIN32

	// Scan for available serial ports
	for(size_t i=1; i<=16; i++) {
		
		SerialClass serial;
		TCHAR strPort[32] = {0};
		sprintf(strPort, "\\\\.\\COM%d", i);   
	
		if ( serial.OpenPort( strPort ) == SerialClass::OK ) {
			
			sprintf(strPort, "COM%d", i);
			setup_ui->serialChoice->add( strPort );
			serial.ClosePort();
			
		}
		
	}
	
#endif
	
	if( config.patch_enable ) {
		setup_ui->patchFileToggle->value( 1 );
		setup_ui->patchFileInput->value( config.patch_file.c_str() );
		setup_ui->patchGroup->activate();
	}
	
	if( !config.serial_dev.empty() ) {
		setup_ui->serialChoice->value( config.serial_dev.c_str() );
	}
	
	setup_ui->bpcBreakToggle->value( config.set_bpc );
	setup_ui->loadSymbolsCheck->value( config.autoload_symbols );
	setup_ui->autoMinimizeCheck->value( config.auto_minimize );
	
	setup_ui->symDissCheck->value( config.show_sym_disassembler );
	setup_ui->symMemCheck->value( config.show_sym_memorybrowser );
	
	setup_ui->ramExpEnable->value( config.exp3_enable );
	setup_ui->ramExpSize->value( config.exp3_size_kb );
	if( config.exp3_enable ) {
		setup_ui->ramExpSize->activate();
	}
	
	setup_ui->ram8mbEnable->value( config.ram_8mb );
	
	setup_ui->show();
	
	while( setup_ui->shown() ) {
		Fl::wait();
	}
	
	ui->statusBar->SetPort( config.serial_dev.c_str() );
	ui->disasm->redraw();
	if( mem_ui->shown() ) {
		mem_ui->memory->redraw();
	}
	
	delete setup_ui;
	
}

void cb_SettingsBrowsePatch(Fl_Button *w, void *u) {
	
	Fl_Native_File_Chooser chooser;
	
	chooser.title( "Browse Patch Binary" );
	chooser.filter( "Binary file\t*.bin\n" );
	
	if( chooser.show() == 0 ) {
		
		setup_ui->patchFileInput->value( chooser.filename() );
		
	}
	
}

void cb_SettingsExpToggle(Fl_Check_Button *w, void *u) {
	
	if( w->value() ) {
		setup_ui->ramExpSize->activate();
	} else {
		setup_ui->ramExpSize->deactivate();
	}
	
}

void cb_SettingsPatchToggle(Fl_Light_Button *w, void *u) {
	
	if( w->value() ) {
		setup_ui->patchGroup->activate();
	} else {
		setup_ui->patchGroup->deactivate();
	}
	
}

void cb_SettingsOkay(Fl_Button *w, void *u) {
	
	config.serial_dev	= setup_ui->serialChoice->value();
	config.set_bpc		= setup_ui->bpcBreakToggle->value();
	
	config.patch_enable	= setup_ui->patchFileToggle->value();
	config.patch_file	= setup_ui->patchFileInput->value();
	
	config.autoload_symbols = setup_ui->loadSymbolsCheck->value();
	config.auto_minimize	= setup_ui->autoMinimizeCheck->value();
	
	config.show_sym_disassembler = setup_ui->symDissCheck->value();
	config.show_sym_memorybrowser = setup_ui->symMemCheck->value();
	
	config.exp3_enable = setup_ui->ramExpEnable->value();
	config.exp3_size_kb = setup_ui->ramExpSize->value();
	
	config.ram_8mb = setup_ui->ram8mbEnable->value();
	
	if( config.ram_8mb ) { 
		code_mem.SetMemSize( 8 );
		data_mem.SetMemSize( 8 );
	} else {
		code_mem.SetMemSize( 2 );
		data_mem.SetMemSize( 2 );
	}
	
	if( config.SaveConfig( "psdebug.cfg" ) ) {
		
		fl_message_title( "Error Saving Settings" );
		fl_message( "Unable to save configuration settings." );
		
		return;
		
	}
	
	setup_ui->hide();
	
}