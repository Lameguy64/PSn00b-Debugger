/* 
 * File:   ProgramClass.cpp
 * Author: Lameguy64
 * 
 * Created on October 27, 2018, 8:34 AM
 */

#include <iostream>
#include <Fl/fl_message.H>
#include "MainUI.h"
#include "BookmarksUI.h"
#include "ProjectClass.h"
#include "CommsClass.h"

extern MainUI		*ui;
extern BookmarksUI	*bmlist_ui;
extern ProjectClass	project;

extern Fl_RGB_Image	*bmark_icon;
extern CommsClass comms;

void ShowBookmarksWindow() {
	
	if( bmlist_ui->shown() ) {
		return;
	}
	
	bmlist_ui->bookmarkTable->clear();
	for( int i=0; i<project.bookmarks.size(); i++ ) {
		
		bmlist_ui->bookmarkTable->additem( &project.bookmarks[i] );
		
	}
	bmlist_ui->bookmarkTable->autowidth( 20 );
	
	if( !bmlist_ui->shown() ) {
		bmlist_ui->icon( bmark_icon );
		bmlist_ui->show();
	}
	
}

void cb_Bookmarks(Fl_Menu_ *w, void *u) {
	
	ShowBookmarksWindow();
	
}

static void cb_RunToBookmark(Fl_Widget *w, void *u) {
	
	if( !comms.serial.IsOpen() ) {
		return;
	}
	
	Fl_Bookmark_Table *widget = (Fl_Bookmark_Table*)w;
	
	int col_a, col_b;
	int row_a, row_b;
	widget->get_selection( row_a, col_a, row_b, col_b );
	
#ifdef DEBUG
	std::cout << "Run to = " << std::hex << project.bookmarks[row_b].addr 
		<< std::dec << std::endl;
#endif
	
	comms.dbRunTo( project.bookmarks[row_b].addr );
	
}

static void cb_EditBookmark(Fl_Widget *w, void *u) {
	
	Fl_Bookmark_Table *widget = (Fl_Bookmark_Table*)w;
	
	int col_a, col_b;
	int row_a, row_b;
	widget->get_selection( row_a, col_a, row_b, col_b );
	
	fl_message_title( "Edit Bookmark" );
	
	const char* input = fl_input( "Input a comment for this bookmark.",
		project.bookmarks[row_b].remark.c_str() );
	
	if( input ) {
		project.bookmarks[row_b].remark = input;
	}
	
	bmlist_ui->bookmarkTable->clear();
	for( int i=0; i<project.bookmarks.size(); i++ ) {
		bmlist_ui->bookmarkTable->additem( &project.bookmarks[i] );
	}
	bmlist_ui->bookmarkTable->autowidth( 20 );
	
}

static void cb_DeleteBookmark(Fl_Widget *w, void *u) {
	
	Fl_Bookmark_Table *widget = (Fl_Bookmark_Table*)w;
	
	int col_a, col_b;
	int row_a, row_b;
	widget->get_selection( row_a, col_a, row_b, col_b );
	
	project.bookmarks.erase( project.bookmarks.begin()+row_b );
	
	bmlist_ui->bookmarkTable->clear();
	for( int i=0; i<project.bookmarks.size(); i++ ) {
		bmlist_ui->bookmarkTable->additem( &project.bookmarks[i] );
	}
	bmlist_ui->bookmarkTable->autowidth( 20 );
	
	ui->disasm->redraw();
}

void cb_BookmarksTable(Fl_Bookmark_Table *w, void *u) {
	
	if( Fl::event() == FL_PUSH ) {
		
		int col_a, col_b;
		int row_a, row_b;
		w->get_selection( row_a, col_a, row_b, col_b );
			
		if( Fl::event_button() == FL_LEFT_MOUSE ) {
				
			if( Fl::event_clicks() ) {

				if( ( row_b >= 0 ) && ( row_b < w->rows() ) ) {
				
					unsigned int bm_addr;
					bm_addr = w->item( row_b );

					if( bm_addr ) {

						ui->disasm->SetPos( bm_addr, true );
						ui->statusBar->SetAddress( bm_addr );

					}
				
				}

			}
			
		} else if( Fl::event_button() == FL_RIGHT_MOUSE ) {
			
			if( ( row_b >= 0 ) && ( row_b < w->rows() ) ) {
			
				Fl_Menu_Item pulldown[] = {
					{ "Run to Bookmark", 0, cb_RunToBookmark, 0, 0, 0, FL_HELVETICA, 12 },
					{ "Edit Comment", 0, cb_EditBookmark, 0, 0, 0, FL_HELVETICA, 12 },
					{ "Delete", 0, cb_DeleteBookmark, 0, 0, 0, FL_HELVETICA, 12 },
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
		
	}
	
}