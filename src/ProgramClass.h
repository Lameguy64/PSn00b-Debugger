/* 
 * File:   ProgramClass.h
 * Author: Lameguy64
 *
 * Created on June 19, 2018, 10:20 AM
 */

#ifndef CONTEXTCLASS_H
#define CONTEXTCLASS_H

#include "pstypes.h"

class ProgramClass {
public:
	
	ProgramClass();
	virtual ~ProgramClass();
	
	void clear();
	int LoadProgram(const char* filename);
	
	EXEC exe_params;
	unsigned int *exe_data;		// Executable data
	
private:
	
	void* LoadCPE(FILE *fp, EXEC *param);
	void* LoadELF(FILE *fp, EXEC *param);
};

#endif /* CONTEXTCLASS_H */

