/* 
 * File:   SerialClass.cpp
 * Author: Lameguy64
 * 
 * Created on August 22, 2018, 6:22 AM
 */

#include <Fl/Fl_Menu_.H>
#include <Fl/fl_message.H>
#include <math.h>
#include <iostream>

#include "MainUI.h"
#include "AboutUI.h"

#define VERSION "0.26b"

extern MainUI* ui;

void cb_About(Fl_Menu_* w, void* u) {
	
	AboutUI* about_ui = new AboutUI();
	
	about_ui->position(
		ui->x()+(ui->w()>>1)-(about_ui->w()>>1),
		ui->y()+(ui->h()>>1)-(about_ui->h()>>1) );
	
	about_ui->versionCaption->label( "Version " VERSION );
	
	about_ui->show();
	
	while( about_ui->shown() ) {
		Fl::wait();
	}
	
	delete about_ui;
	
}