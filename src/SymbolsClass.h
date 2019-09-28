/* 
 * File:   SymbolsClass.h
 * Author: Lameguy64
 *
 * Created on August 14, 2018, 1:54 PM
 */

#ifndef SYMBOLSCLASS_H
#define SYMBOLSCLASS_H

#include <string>
#include <vector>

class SymbolsClass {
public:
	
	class SourceClass {
	public:
		
		typedef struct {
			unsigned int addr;
			unsigned short lines;
			unsigned short absolute;
		} LINE;
		
		SourceClass();
		virtual ~SourceClass();
		
		unsigned int LineToAddr(int line);
		int IsLineMapped(int line);
		
		std::string src_file;		// Source file name
		unsigned int src_saddr;		// Starting address
		unsigned int src_eaddr;		// End address
		
		int src_start;				// Starting line of debug line data
		std::vector<LINE> lines;	// Line data
		
	};

	SymbolsClass();
	virtual ~SymbolsClass();
	
	SourceClass *AddrToLine(unsigned int addr, int *line);
	
	void ClearSymbols();
	int LoadSymbols(const char* filename);
	const char* FindSymbol(unsigned int addr);
	const char* CorrectSymbol(const char *name);
	unsigned int ResolveSymbol(const char *name, int &found);
	
	typedef struct {
		unsigned int id;
		int whatever;
	} SYM_HEAD;
	
	typedef struct {
		unsigned int	addr;
		unsigned char	id;
	} SYM_CHUNK;
	
	typedef struct {
		unsigned int addr;
		unsigned int id;
		std::string name;
	} SYM_NAME;
		
	std::vector<SourceClass*> sources;
	std::vector<SYM_NAME> symbols;
	
};

#endif /* SYMBOLSCLASS_H */

