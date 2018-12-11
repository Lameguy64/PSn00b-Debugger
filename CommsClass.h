/* 
 * File:   CommsClass.h
 * Author: Lameguy64
 *
 * Created on June 18, 2018, 1:36 PM
 */

#ifndef COMMSCLASS_H
#define COMMSCLASS_H

#include <stdio.h>
#include <Fl/Fl_Widget.H>
#include "pstypes.h"
#include "SerialClass.h"
#include "ProgramClass.h"

class CommsClass {
public:
	
	CommsClass();
	virtual ~CommsClass();
	
	int UploadExecutable(const char* exefile);
	int UploadPatch(const char* binfile);
	int UploadProgram(ProgramClass* program);
	
	int DownloadMemory(unsigned int addr, unsigned int size, FILE* fp);
	
	int dbTest();
	void dbReboot();
	void dbPause();
	void dbRunTo(unsigned int bpc);
	int dbSetBreak(unsigned int addr);
	int dbGetBreaks(unsigned int *buff);
	void dbContinue();
	void dbRegisters(unsigned int* regs);
	int dbGetMemory(unsigned int pos, int len, void* buff);
	
	SerialClass serial;
	
	typedef struct {
		int size;
		unsigned int addr;
		unsigned int crc32;
	} BINPARAM;
	
	enum {
		UploadSetBPC = 0x01,
	};

private:

	void initTable32(unsigned int* table);
	unsigned int crc32(void* buff, int bytes, unsigned int crc);
	
	void* LoadCPE(FILE* fp, EXEC* param);
	static void cb_UploadCancel(Fl_Widget* w, void* u);
	
	int cancel;
};

#endif /* COMMSCLASS_H */

