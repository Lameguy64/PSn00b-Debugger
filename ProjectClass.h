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
	
	typedef struct {
		unsigned int addr;
		std::string remark;
	} BOOKMARK;
	
	ProjectClass();
	virtual ~ProjectClass();
	
	void clear();
	int Save(const char* filename);
	int Load(const char* filename);
	
	std::string		exe_file;
	std::string		sym_file;
	std::string		last_src_file;
	
	std::vector<BOOKMARK> bookmarks;
	
	SymbolsClass	symbols;
	
private:
	tinyxml2::XMLElement *DumpWinPos( tinyxml2::XMLDocument* doc, 
		int x, int y, int w, int h );
};

#endif /* PROJECTCLASS_H */

