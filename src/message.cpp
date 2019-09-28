/* 
 * File:   Fl_Disassembler.cpp
 * Author: Lameguy64
 * 
 * Created on October 12, 2018, 7:26 PM
 */

#include "MessageUI.h"

extern MessageUI *msg_ui;
extern Fl_RGB_Image *mwin_icon;

void cb_MessageWindow(Fl_Menu_ *w, void *u) {
	
	msg_ui->icon( mwin_icon );
	msg_ui->show();
	
}

void cb_MessageClear(Fl_Menu_ *w, void *u) {
	
	msg_ui->text->text( "" );
	
}

void PrintMessage(const char* text) {
	
	msg_ui->text->append( text );
	
	int num_lines = msg_ui->text->count_lines( 0, msg_ui->text->length() );
	
	msg_ui->logDisplay->scroll( num_lines-1, 0 );
	msg_ui->logDisplay->redraw();
	
}