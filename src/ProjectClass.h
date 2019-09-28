/* 
 * File:   ProjectClass.h
 * Author: Lameguy64
 *
 * Created on September 16, 2018, 8:11 PM
 */

#ifndef PROJECTCLASS_H
#define PROJECTCLASS_H

#include <tinyxml2.h>
#include <string>
#include <vector>

#include "SymbolsClass.h"

class ProjectClass {
public:
	
	enum {
		WATCH_CHAR = 0,
		WATCH_SHORT,
		WATCH_INT
	} WATCH_TYPE;
	
	typedef struct BOOKMARK {
		unsigned int addr;
		std::string remark;
	} BOOKMARK;
	
	class WATCHPOINT {
	public:
		std::string sym_name;
		unsigned int addr;
		unsigned char type:6;
		unsigned char unsign:1;
		unsigned char hex:1;
		int value;
	};
	
	ProjectClass();
	virtual ~ProjectClass();
	
	void clear();
	int Save(const char* filename);
	int Load(const char* filename);
	
	void AddWatch(unsigned int addr, int type, int unsign, int hex);
	void AddWatch(const char *sym, int type, int unsign, int hex);
	
	std::string	 file;
	std::string	 exe_file;
	std::string	 sym_file;
	std::string	 last_src_file;
	
	std::vector<BOOKMARK> bookmarks;
	std::vector<WATCHPOINT*> watchpoints;
	
	SymbolsClass symbols;
	
private:
	
	tinyxml2::XMLElement *DumpWinPos( tinyxml2::XMLDocument* doc, 
		int x, int y, int w, int h );
	
};

#endif /* PROJECTCLASS_H */

