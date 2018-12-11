/* 
 * File:   Fl_MemoryView.h
 * Author: Lameguy64
 *
 * Created on November 02, 2018, 07:02 PM
 */

#include <Fl/fl_message.H>
#include <Fl/fl_draw.H>
#include <iostream>

#include "MemoryUI.h"
#include "MainUI.h"
#include "BookmarksUI.h"
#include "ProjectClass.h"
#include "CommsClass.h"
#include "RemoteMemClass.h"

extern MainUI		*ui;
extern MemoryUI		*mem_ui;
extern BookmarksUI	*bmlist_ui;
extern Fl_RGB_Image	*mem_icon;

extern ProjectClass project;
extern CommsClass	comms;

extern RemoteMemClass	data_mem;


void RefreshRemoteData(int force = false) {
	
	if( !comms.serial.IsOpen() ) {
		return;
	}
	
	fl_font( mem_ui->memory->textfont(), mem_ui->memory->textsize() );
	
	int test_size = (8*(mem_ui->memory->h()/fl_height()))+2;
	unsigned int test_addr = (mem_ui->memory->scroll_pos<<3);
	
	if( ( data_mem.TestUpdate( test_addr, test_size ) ) || ( force ) ) {
		
		ui->statusBar->SetStatus( "Updating memory..." );
		
	}
	
	switch( data_mem.Update( &comms, test_addr, test_size, force ) ) {
		case 1:
			ui->statusBar->SetStatus( "Done updating." );
			mem_ui->memory->redraw();
			break;
		case 2:
			ui->statusBar->SetStatus( "Error updating." );
			mem_ui->memory->redraw();
			break;
		default:
			break;
	}
	
}


void cb_MemoryView(Fl_Menu_ *w, void *u) {
	
	if( mem_ui->shown() ) {
		return;
	}
	
	RefreshRemoteData();
	
	mem_ui->icon( mem_icon );
	mem_ui->show();
	
}

void cb_MemoryRefresh(Fl_Button *w, void *u) {
	
	RefreshRemoteData( true );
	
}

void cb_MemoryMode(Fl_Light_Button *w, long mode) {
	
	mem_ui->memory->SetHexDisplay( mode );
	mem_ui->memory->redraw();
	
}

void cb_MemorySetBookmark(Fl_Widget *w, void *u) {
	
	int exists = false;
	
	for( int i=0; i<project.bookmarks.size(); i++ ) {
		
		if( ( mem_ui->memory->GetPos()<<3 ) == project.bookmarks[i].addr ) {
			
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
		
		bookmark.addr = mem_ui->memory->GetPos()<<3;
		project.bookmarks.push_back( bookmark );
		
	}
	
	bmlist_ui->bookmarkTable->clear();
	for( int i=0; i<project.bookmarks.size(); i++ ) {
		bmlist_ui->bookmarkTable->additem( &project.bookmarks[i] );
	}
	
	bmlist_ui->bookmarkTable->autowidth( 20 );
	
}

void cb_Memory(Fl_MemoryView *w, void *u) {
	
	if( Fl::event_button3() ) {
		
		Fl_Menu_Item pulldown[] = {
			{ "Set/Clear Bookmark", 0, cb_MemorySetBookmark, 0, 0, 0, FL_HELVETICA, 12 },
			{ 0 }
		};
		
		const Fl_Menu_Item* item = pulldown->popup( Fl::event_x(), Fl::event_y() );
		
		if( item ) {
			if( item->callback() ) {
				item->callback()( w, nullptr );
			}
		}
		
		return;
		
	}
	
	RefreshRemoteData();
	
}