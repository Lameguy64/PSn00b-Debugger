// Originally Created on April 25, 2018, 9:24 AM

#ifndef SERIALCLASS_H
#define SERIALCLASS_H

#include <string>
#ifdef __WIN32__
#include <windows.h>
#endif

class SerialClass {
public:
	SerialClass();
	virtual ~SerialClass();
	
	enum ErrorType {
		ERROR_NONE = 0,
		ERROR_OPENING,
		ERROR_CONFIG,
		ERROR_NOT_OPEN,
		ERROR_WRITE_FAIL,
	};
	
	ErrorType OpenPort(const char* name);
	ErrorType SetRate(int rate);
	void ClosePort();
	
	int SendByte(int val);
	int SendBytes(void* data, int bytes);
	int ReceiveBytes(void* data, int bytes);
	int PendingBytes();
	
	int IsOpen() {
		return portOpened;
	}
	
#ifdef __WIN32__
	HANDLE hComm;
#else
	int hComm;
#endif
	
private:
	
	int portOpened;
	
};

#endif /* SERIALCLASS_H */

