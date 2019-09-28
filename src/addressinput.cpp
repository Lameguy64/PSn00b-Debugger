#include <string.h>
#include <iostream>
#include <strings.h>
#include "MainUI.h"
#include "AboutUI.h"
#include "ProjectClass.h"

extern MainUI *ui;
extern ProjectClass project;

static AddressUI *addr_ui;
static int input_ok;

static const char* regs[] = {
	"at","v0","v1",
	"a0","a1","a2","a3",
	"t0","t1","t2","t3",
	"t4","t5","t6","t7",
	"s0","s1","s2","s3",
	"s4","s5","s6","s7",
	"t8","t9","k0","k1",
	"gp","sp","fp","ra",
};


unsigned int AddressInput(int *type) {
	
	unsigned int val = 0;
	
	*type = 0;
	addr_ui = new AddressUI;

	input_ok = false;
	
	addr_ui->show();
	
	while( addr_ui->shown() ) {
		Fl::wait();
	}
	
	if( input_ok ) {
		
		if( strlen( addr_ui->addressInput->value() ) > 0 ) {
			
			const char* sstr = addr_ui->addressInput->value();
			
			// Check if value is a register
			if( ( *sstr != '$' ) || 
				( ( sstr[0] == '0' ) && ( tolower( sstr[1] ) == 'x' ) ) ) {
				
				if( strcasecmp(sstr, "r0") == 0 ) {
					val = 0;
					*type = 1;
				}
				
				if( strcasecmp(sstr, "zero") == 0 ) {
					val = 0;
					*type = 1;
				}
				
				for( int i=0; i<31; i++ ) {

					if( strcasecmp( sstr, regs[i] ) == 0 ) {
						val = i;
						*type = 3;
						break;
					}

				}
				
				if( *type == 0 )  {
					
					if( strcasecmp( sstr, "pc" ) == 0 ) {
						*type = 4;
					}

					// Search symbols
					for( int i=0; i<project.symbols.symbols.size(); i++ ) {

						if( strcasecmp( sstr, project.symbols.symbols[i].name.c_str() ) == 0 ) {
							val = project.symbols.symbols[i].addr;
							*type = 2;
						}

					}
					
				}
				
			}
			
			if( *type == 0 ) {
				
				// Parse it as an address in hex
				if( *sstr == '$' ) {
					sstr++;
				}

				sscanf( sstr, "%x", &val );
				*type = 1;
			
			}
			
		}
		
	}
	
	
	delete addr_ui;
	
	return val;
	
}

void cb_AddressOk(Fl_Return_Button *w, void *u) {
	
	input_ok = true;
	addr_ui->hide();
	
}

void cb_AddressCancel(Fl_Button *w, void *u) {
	
	addr_ui->hide();
	
}