#include <iostream>
#include <Fl/Fl_Menu_Item.H>
#include <Fl/Fl_Timer.H>
#include <Fl/fl_message.H>
#include "ProjectClass.h"
#include "WatchesUI.h"
#include "CommsClass.h"
#include "ConfigClass.h"


extern WatchesUI *watch_ui;
extern ProjectClass project;
extern CommsClass comms;
extern ConfigClass config;

static DataBreakUI *break_ui;
static SetWatchUI *setwatch_ui;

static int is_edit;

void cb_WatchView(Fl_Menu_ *w, void *u) {
	
	watch_ui->show();
	
}

void cb_AddWatch(Fl_Widget *w, void *u) {
	
	setwatch_ui = new SetWatchUI;
	
	is_edit = false;
	
	setwatch_ui->show();
	
	while(setwatch_ui->shown())
		Fl::wait();
	
	delete setwatch_ui;
	
}

void cb_EditWatch(Fl_Widget *w, void *u) {
	
	char old_value[32];
	Fl_WatchTable *ww = watch_ui->watchTable;
	ProjectClass::WATCHPOINT *item;
	
	setwatch_ui = new SetWatchUI;
	
	is_edit = true;
	
	item = ww->get_item(ww->callback_row());

	if( item->sym_name.size() ) {
		
		setwatch_ui->addrInput->value(item->sym_name.c_str());
		
	} else {
		
		sprintf(old_value, "0x%X", item->addr);
		setwatch_ui->addrInput->value(old_value);
		
	}
	
	setwatch_ui->typeChoice->value(item->type);
	setwatch_ui->unsignedCheck->value(item->unsign);
	setwatch_ui->hexCheck->value(item->hex);
	
	setwatch_ui->show();
	
	while(setwatch_ui->shown())
		Fl::wait();
	
	delete setwatch_ui;
	
}

void cb_WatchOkay(Fl_Return_Button *w, void *u) {
	
	ProjectClass::WATCHPOINT *watch;

	if( !is_edit ) {
		watch = new ProjectClass::WATCHPOINT;
	} else {
		watch = watch_ui->watchTable->get_item(watch_ui->watchTable->callback_row());
	}
	
	if( strncasecmp(setwatch_ui->addrInput->value(), "0x", 2) == 0 ) {
		
		sscanf(setwatch_ui->addrInput->value(), "%x", &watch->addr);
		
	} else {
		
		int is_symbol;
		watch->addr = project.symbols.ResolveSymbol(
			setwatch_ui->addrInput->value(), is_symbol);
		
		if( !is_symbol ) {
			
			fl_message_title("Bad Value");
			fl_message("Bad address syntax or symbol not found.");
			delete watch;
			return;
			
		}
		
		watch->sym_name = project.symbols.CorrectSymbol(
			setwatch_ui->addrInput->value());
		
	}
	
	watch->type		= setwatch_ui->typeChoice->value();
	watch->unsign	= setwatch_ui->unsignedCheck->value();
	watch->hex		= setwatch_ui->hexCheck->value();
	
	if( ( watch->type == ProjectClass::WATCH_SHORT ) 
		&& ( (watch->addr%2) != 0 ) ) {
		
		fl_message_title("Bad Address Alignment");
		fl_message("Address must be in multiples of two for specified tyoe.");
		delete watch;
		return;
		
	} else if( ( watch->type == ProjectClass::WATCH_INT ) 
		&& ( (watch->addr%4) != 0 ) ) {
		
		fl_message_title("Bad Address Alignment");
		fl_message("Address must be in multiples of four for specified.");
		delete watch;
		return;
		
	}
	
	if( !is_edit ) {
		
		project.watchpoints.push_back(watch);
		watch_ui->watchTable->add_item(watch);
		watch_ui->watchTable->redraw();

	} else {
		
		watch_ui->watchTable->update_types();
		
	}
	
	setwatch_ui->hide();
	
}

void cb_WatchCancel(Fl_Button *w, void *u) {
	
	setwatch_ui->hide();
	
}

void cb_DeleteWatch(Fl_Widget *w, void *u) {
	
	int row = watch_ui->watchTable->callback_row();
	ProjectClass::WATCHPOINT *item = watch_ui->watchTable->get_item(row);
	
	for(int i=0; i<project.watchpoints.size(); i++) {
		
		if( project.watchpoints[i] == item ) {
			
			delete project.watchpoints[i];
			project.watchpoints.erase(project.watchpoints.begin()+i);
			break;
			
		}
		
	}
	
	watch_ui->watchTable->delete_item(row);
	
}

void cb_WatchSetValue(Fl_Widget *w, void *u) {
	
	const char *type[] = {
		"char",
		"short",
		"int"
	};
	char last_value[32];
	
	Fl_WatchTable *ww = watch_ui->watchTable;
		
	if( ww->callback_context() == Fl_Table_Row::CONTEXT_CELL ) {
		
		ProjectClass::WATCHPOINT *watch = ww->get_item(ww->callback_row());
		
		if( watch->hex ) {
			sprintf(last_value, "0x%X", watch->value);
		} else {
			sprintf(last_value, "%d", watch->value);
		}
		
		unsigned int val;
		const char *newval = fl_input("Enter New Value (%s)", 
			last_value, type[watch->type]);
		
		if( newval == nullptr )
			return;
		
		if( strncasecmp(newval, "0x", 2) == 0 ) {
		
			sscanf(newval, "%x", &val);

		} else {
			
			val = atoi(newval);
			
		}
		
		// Put value write comms here
		if( comms.dbSetWord(watch->addr, watch->type, val) ) {
			
			fl_message_title("Error Setting");
			fl_message("Debugger stopped responding.");
			return;
			
		}
		
		watch->value = val;
		ww->update_values();
		
	}
	
}

void cb_DataBreakDialog(Fl_Widget *w, void *u) {
	
	break_ui = new DataBreakUI;
	
	{
		
		Fl_WatchTable *ww = watch_ui->watchTable;
		
		if( ww->callback_context() == Fl_Table_Row::CONTEXT_CELL ) {
			
			ProjectClass::WATCHPOINT *watch = ww->get_item(ww->callback_row());
			char buffer[64];
			
			std::cout << "Set breakpoint = " << ww->callback_row() << " "
				<< std::hex << watch->addr << std::dec << std::endl;
			
			sprintf(buffer, "0x%X", watch->addr);
			break_ui->breakAddrInput->value(buffer);
			
		}
		
	}
	
	break_ui->breakMaskInput->value("0xFFFFFFFF");
	
	break_ui->show();
	
	while(break_ui->shown())
		Fl::wait();
	
	break_ui->hide();
	delete break_ui;
	
}

void cb_BreakSet(Fl_Return_Button *w, void *u) {
	
	unsigned int addr;
	unsigned int mask;
	int flags;
	
	sscanf(break_ui->breakAddrInput->value(), "%x", &addr);
	sscanf(break_ui->breakMaskInput->value(), "%x", &mask);
	flags = (break_ui->readToggle->value()&0x1)
		|((break_ui->writeToggle->value()&0x1)<<1);
	
	if( comms.dbSetDataBreak(flags, addr, mask) ) {
		fl_message_title("Error Setting");
		fl_message("Debugger stopped responding.");
	}
	
	break_ui->hide();
	
}

void cb_BreakClear(Fl_Button *w, void *u) {
	
	if( comms.dbSetDataBreak(0, 0, 0) ) {
		fl_message_title("Error Setting");
		fl_message("Debugger stopped responding.");
	}
	
	break_ui->hide();
	
}

void cb_BreakCancel(Fl_Button *w, void *u) {
	
	break_ui->hide();
	
}

void cb_WatchTable(Fl_WatchTable *w, void *u) {
	
	if( Fl::event_button3() ) {
		
		Fl_Menu_Item pulldown[] = {
			{ "Add Watch...", 0, cb_AddWatch, 0, 0, 0, FL_HELVETICA, 12 },
			{ "Edit Watch...", 0, cb_EditWatch, 0, 0, 0, FL_HELVETICA, 12 },
			{ "Delete Watch", 0, cb_DeleteWatch, 0, FL_MENU_DIVIDER, 0, FL_HELVETICA, 12 },
			{ "Set/Clear Data Breakpoint...", 0, cb_DataBreakDialog, w, 0, 0, FL_HELVETICA, 12 },
			{ "Modify Value...", 0, cb_WatchSetValue, 0, 0, 0, FL_HELVETICA, 12 },
			{ 0 }
		};
		
		if( w->callback_context() == Fl_Table_Row::CONTEXT_CELL ) {
			
			w->select_all_rows(0);
			w->select_row(w->callback_row(), 1);
			
		} else {
			
			pulldown[1].deactivate();
			pulldown[2].deactivate();
			pulldown[4].deactivate();
			
		}
		
		if( !comms.serial.IsOpen() ) {
			
			pulldown[3].deactivate();
			pulldown[4].deactivate();
			
		}
				
		const Fl_Menu_Item* item = pulldown->popup(Fl::event_x(), Fl::event_y());
		
		if( item ) {
			if( item->callback() ) {
				item->callback()(w, item->user_data());
			}
		}
		
		return;
		
	}
	
}

int WatchUpdate() {

	if( !comms.serial.IsOpen() ) {
		
		fl_message_title("Not Connected");
		fl_message("Not yet connected to a compatible debug monitor.");
		return 1;
		
	}
	
	for(int i=0; i<project.watchpoints.size(); i++) {
		
		int error;
		
		project.watchpoints[i]->value = 
			comms.dbGetWord(
				project.watchpoints[i]->addr,
				project.watchpoints[i]->type,
				error);
		
		if( error ) {
			
			fl_message_title("Error Updating");
			fl_message("Debugger stopped responding.");
			return 1;
			
		}
		
	}
	
	watch_ui->watchTable->update_values();
	
	return 0;
	
}

void cb_WatchUpdate(Fl_Button *w, void *u) {
	
	WatchUpdate();
	
}

void cb_WatchAutoHandler(void *u) {
	
	if( WatchUpdate() ) {
		return;
	}
	
}

void cb_WatchAutoToggle(Fl_Light_Button *w, void *u) {
	
	if( w->value() ) {
		
		Fl::add_timeout(config.update_interval, 
			cb_WatchAutoHandler, nullptr);
		
	} else {
		
		Fl::remove_timeout(cb_WatchAutoHandler, nullptr);
		
	}
	
}