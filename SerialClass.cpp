/* 
 * File:   SerialClass.cpp
 * Author: Lameguy64
 * 
 * Created on April 25, 2018, 9:24 AM
 * 
 * Note: While there is some Linux support in place, it need some work
 * as last time I tested it on Linux it won't change the baud rate and
 * timeout settings need to be implemented.
 */

#include <math.h>
#include <stdio.h>
#include <sys/file.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#ifndef __WIN32__
#include <sys/ioctl.h>
#include <termios.h>
#include <bits/time.h>
#endif
#include "SerialClass.h"

SerialClass::SerialClass() {

#ifdef __WIN32__
	hComm = INVALID_HANDLE_VALUE;
#else
	hComm = -1;
#endif

	portOpened = 0;
	
}

SerialClass::~SerialClass() {
	
#ifdef __WIN32__
	if ( hComm != INVALID_HANDLE_VALUE ) {
		CloseHandle( hComm );
	}
#else
	if ( hComm >= 0 ) {
		close( hComm );
	}
#endif
	
}

SerialClass::ErrorType SerialClass::OpenPort(const char* name) {
	
#ifdef __WIN32__
	if ( hComm != INVALID_HANDLE_VALUE ) {
		CloseHandle( hComm );
	}
	
	hComm = CreateFile( name,  //port name
		GENERIC_READ | GENERIC_WRITE,	//Read/Write
		0,								// No Sharing
		NULL,							// No Security
		OPEN_EXISTING,					// Open existing port only
		0,								// Non Overlapped I/O
		NULL );							// Null for Comm Devices
	 
	if ( hComm == INVALID_HANDLE_VALUE ) {
		return ERROR_OPENING;
	}
	
	DCB dcbSerialParams;
	
	if ( !GetCommState( hComm, &dcbSerialParams ) ) {
		return ERROR_CONFIG;
	}
	
	dcbSerialParams.BaudRate = CBR_115200;// Setting BaudRate = 115200
	dcbSerialParams.ByteSize = 8;         // Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;// Setting StopBits = 1
	dcbSerialParams.Parity   = NOPARITY;  // Setting Parity = None
	
	if ( !SetCommState( hComm, &dcbSerialParams ) ) {
		return ERROR_CONFIG;
	}
	
	COMMTIMEOUTS timeouts;
	
	timeouts.ReadIntervalTimeout         = 100; // in milliseconds
	timeouts.ReadTotalTimeoutConstant    = 100; // in milliseconds
	timeouts.ReadTotalTimeoutMultiplier  = 1; // in milliseconds
	timeouts.WriteTotalTimeoutConstant   = 100; // in milliseconds
	timeouts.WriteTotalTimeoutMultiplier = 10; // in milliseconds
	
	if ( !SetCommTimeouts( hComm, &timeouts ) ) {
		return ERROR_CONFIG;
	}
			
#else
	
	hComm = open( name , O_RDWR|O_NOCTTY );
	
	if ( hComm == -1 ) {
		
		return ERROR_OPENING;
		
	}
	
	if ( SetRate( 115200 ) != OK ) {
		return ERROR_CONFIG;
	}
	
#endif
	
	portOpened = 1;
	
	return OK;
}

SerialClass::ErrorType SerialClass::SetRate(int rate) {

#ifdef __WIN32__	
	
	if ( hComm == INVALID_HANDLE_VALUE ) {
		return ERROR_NOT_OPEN;
	}
	
	DCB dcbSerialParams;
	
	if ( !GetCommState( hComm, &dcbSerialParams ) ) {
		return ERROR_CONFIG;
	}
	
	dcbSerialParams.BaudRate = rate;		// Setting BaudRate = 9600
	dcbSerialParams.ByteSize = 8;			// Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;	// Setting StopBits = 1
	dcbSerialParams.Parity   = NOPARITY;	// Setting Parity = None
	
	if ( !SetCommState( hComm, &dcbSerialParams ) ) {
		return ERROR_CONFIG;
	}
	
#else
	
	if ( hComm == -1 ) {
		return ERROR_NOT_OPEN;
	}
	
	struct termios tty;

	/*
    if ( tcgetattr( hComm, &tty ) < 0 ) {
        return ERROR_CONFIG;
    }
	
    tty.c_cflag |= (CLOCAL | CREAD);    // ignore modem controls 
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;					// 8-bit characters 
    tty.c_cflag &= ~PARENB;				// no parity bit 
    tty.c_cflag &= ~CSTOPB;				// only need 1 stop bit 
    tty.c_cflag &= ~CRTSCTS;			// no hardware flowcontrol 
	
    // setup for non-canonical mode
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;
	*/
	
	memset(&tty, 0x0, sizeof(termios));
	
	tty.c_cflag = CS8|CLOCAL|CREAD;
	tty.c_iflag = IGNPAR;
	tty.c_oflag = 0;
	tty.c_lflag = 0;
	
	cfsetospeed(&tty, (speed_t)rate);
    cfsetispeed(&tty, (speed_t)rate);
	
    // fetch bytes as they become available
	tty.c_cc[VMIN] = 0;
	tty.c_cc[VTIME] = 2;

    if ( tcsetattr( hComm, TCSANOW, &tty ) != 0 ) {
        return ERROR_CONFIG;
    }
	
#endif
	
	return OK;
}

int SerialClass::PendingBytes() {

#ifdef __WIN32__
	
	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError(hComm, &dwErrorFlags, &ComStat);

	return((int)ComStat.cbInQue);
	
#else
	
	int bytes;
	ioctl(hComm, FIONREAD, &bytes);
			
	return bytes;
	
#endif
	
	
	
}

int SerialClass::SendBytes(void* data, int length) {
	
#ifdef __WIN32__
	DWORD bytesWritten;
	
	if( !WriteFile( hComm, data, length, &bytesWritten, NULL ) ) {
		return -1;
	}
	
#else
	
	int bytesWritten;
	
	bytesWritten = write(hComm, data, length);
	
#endif
	
	return bytesWritten;
}

int SerialClass::ReceiveBytes(void* data, int bytes) {

#ifdef __WIN32__

	DWORD bytesReceived;
	
	if ( !ReadFile( hComm, data, bytes, &bytesReceived, NULL )  ) {
		return -1;
	}
	
#else
	int bytesReceived;
	struct timeval timeout;
	
	fd_set read_fds, write_fds, except_fds;
	
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_ZERO(&except_fds);
	FD_SET(hComm, &read_fds);
	
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	
	if ( select(hComm+1, &read_fds, &write_fds, &except_fds, &timeout) == 1 ) {
	
		bytesReceived = read(hComm, data, bytes);
	
	} else {
		
		return -1;
		
	}
	
#endif
	
	return bytesReceived;
	
}

void SerialClass::ClosePort() {
	
#ifdef __WIN32__
	if ( hComm != INVALID_HANDLE_VALUE ) {
		CloseHandle( hComm );
		hComm = INVALID_HANDLE_VALUE;
	}
#else
	if ( hComm >= 0 ) {
		close( hComm );
		hComm = -1;
	}
#endif
	
	portOpened = 0;
	
}