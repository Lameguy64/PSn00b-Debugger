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
	
	/* Wait for 'start' response code from debug monitor */
	int dbWaitBegin();
	
	/* Ping if debug monitor is still responsive */
	int dbPing();
	
	/* Soft reboots target machine */
	int dbReboot();
	
	/* Pauses target machine */
	void dbPause();
	
	/* Instruction step */
	int dbStep();
	
	/* Run and break on specified address */
	int dbRunTo(unsigned int bpc);
	
	/* Unused commands for a failed multiple breakpoint implementation */
	int dbSetBreak(unsigned int addr);
	int dbGetBreaks(unsigned int *buff);
	
	/* Sets a single data breakpoint */
	int dbSetDataBreak(int flags, unsigned int addr, unsigned int mask);
	
	/* Resumes execution */
	int dbContinue();
	
	/* Read CPU registers */
	int dbRegisters(unsigned int* regs);
	
	/* Read a region of memory */
	int dbGetMemory(unsigned int pos, int len, void* buff);
	
	/* Read a word */
	int dbGetWord(unsigned int addr, int type, int &error);
	
	/* Write a word */
	int dbSetWord(unsigned int addr, int type, unsigned int value);
	
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

