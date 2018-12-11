/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Fl_Status_Bar.h
 * Author: Lameguy64
 *
 * Created on October 11, 2018, 8:59 AM
 */

#ifndef FL_STATUS_BAR_H
#define FL_STATUS_BAR_H

#include <string>
#include <Fl/Fl_Box.H>

class Fl_Status_Bar : public Fl_Box {
public:
	Fl_Status_Bar(int x, int y, int w, int h, const char* l=0);
	virtual ~Fl_Status_Bar();

	void draw();
	
	void SetStatus(const char* text);
	void SetPort(const char* port) {
		port_text = port;
		redraw();
	}
	void SetConnected(int status) {
		connected = status;
		redraw();
	}
	void SetAddress(unsigned int addr) {
		address = addr;
		redraw();
	}
	
private:
		
	std::string status_text;
	std::string port_text;
	int connected;
	unsigned int address;

};

#endif /* FL_STATUS_BAR_H */

