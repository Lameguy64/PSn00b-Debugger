// Created on August 27, 2019, 3:06 PM

#include <FL/fl_message.H>

#include "MainUI.h"
#include "ProjectClass.h"
#include "CommsClass.h"
#include "SymbolsClass.h"
#include "SourceUI.h"
#include "source.h"
#include "mips_disassembler.h"
#include "BookmarksUI.h"
#include "RemoteMemClass.h"


#define MAX_FOLLOW_UNDO	16


extern MainUI		*ui;
extern SourceUI		*src_ui;
extern BookmarksUI	*bmlist_ui;
extern RemoteMemClass code_mem;

extern ProjectClass	project;
extern ProgramClass program;
extern CommsClass	comms;

unsigned int follow_undo[MAX_FOLLOW_UNDO];
int follow_undo_step = 0;
int follow_undo_count = 0;


extern int RefreshRemoteCode(int force = false);
extern int ViewSource(SymbolsClass::SourceClass* source, int line);


void ClearFollowUndo() {
	
	follow_undo_step = 0;
	follow_undo_count = 0;
	
}

void cb_RunToCursor(Fl_Widget *w, void *u) {
	
	unsigned int addr = ui->disasm->GetPos()<<2;
	comms.dbRunTo(addr);
	
}

void cb_GotoSourceLine(Fl_Widget *w, void *u) {
	
	int line;
	
	SymbolsClass::SourceClass *source = 
		project.symbols.AddrToLine( ui->disasm->GetPos()<<2, &line );
	
	if( source ) {
		
		ViewSource(source, line);
		
	}
	
}

void cb_GotoJumpDest(Fl_Widget *w, void *u) {
	
	Fl_Disassembler *ct = (Fl_Disassembler*)w;
	
	unsigned int addr = ct->GetPos()<<2;
	
	if( ui->remoteMode->value() == 0 ) {
	
		if( addr<program.exe_params.t_addr ) {
			return;
		}
		if( addr>(program.exe_params.t_addr+program.exe_params.t_size ) ) {
			return;
		}

		addr = mips_GetJumpAddr( addr, 
			program.exe_data[(addr-program.exe_params.t_addr)>>2] );
		
	} else {
		
		if( ( addr >= code_mem.remote_addr ) && 
		( addr <= (code_mem.remote_addr+code_mem.remote_size ) ) ) {
			
			addr = mips_GetJumpAddr(addr, 
				code_mem.remote_data[(addr-code_mem.remote_addr)>>2]);
			
		} else {
			
			return;
			
		}
		
	}
	
	if( addr ) {

		follow_undo[follow_undo_step] = ct->GetPos()<<2;
		
		follow_undo_step++;

		if( follow_undo_step >= MAX_FOLLOW_UNDO )
			follow_undo_step = 0;

		if( follow_undo_count < MAX_FOLLOW_UNDO )
			follow_undo_count++;

		ct->SetPos( addr, true );
		ui->statusBar->SetAddress( addr );

	}

	RefreshRemoteCode();
	
}

void cb_GotoJumpUndo(Fl_Widget *w, void *u) {

	if( follow_undo_count == 0 )
		return;
	
	Fl_Disassembler *ct = (Fl_Disassembler*)w;
	
	follow_undo_step--;
	ct->SetPos(follow_undo[follow_undo_step], true);
	ui->statusBar->SetAddress(follow_undo[follow_undo_step]);
	
	if( follow_undo_step < 0 )
		follow_undo_step = MAX_FOLLOW_UNDO-1;
	
	follow_undo_count--;
	
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
	
	if( Fl::event_key() == FL_Left ) {
		
		cb_GotoJumpUndo(w, nullptr);
				
	} if( Fl::event_key() == FL_Right ) {
		
		cb_GotoJumpDest(w, nullptr);
		
	} else if( Fl::event_button3() ) {
		
		Fl_Menu_Item pulldown[] = {
			{ "Run to Cursor", 0, cb_RunToCursor, 0, 0, 0, FL_HELVETICA, 12 },
			{ "Set/Clear Bookmark", 0, cb_SetBookmark, 0, FL_MENU_DIVIDER, 0, FL_HELVETICA, 12 },
			{ "Follow Jump/Branch Instruction", FL_Right, cb_GotoJumpDest, 0, 0, 0, FL_HELVETICA, 12 },
			{ "Undo Follow", FL_Left, cb_GotoJumpUndo, 0, FL_MENU_DIVIDER, 0, FL_HELVETICA, 12 },
			{ "Goto Source Line", 0, cb_GotoSourceLine, 0, 0, 0, FL_HELVETICA, 12 },
			{ 0 }
		};
		
		if( follow_undo_count == 0 ) {
			pulldown[3].deactivate();
		}
		
		if( project.symbols.sources.size() == 0 ) {
			pulldown[4].deactivate();
		}
		
		const Fl_Menu_Item* item = pulldown->popup( Fl::event_x(), Fl::event_y() );
		
		if( item ) {
			if( item->callback() ) {
				item->callback()(w, nullptr);
			}
		}
		
		return;
		
	}
	
	ui->statusBar->SetAddress( w->GetPos()<<2 );
	RefreshRemoteCode();
	
}

void cb_RefreshCode(Fl_Button* w, void* u) {
	
	RefreshRemoteCode( true );
	
}