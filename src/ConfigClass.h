// Originally created on August 22, 2018, 7:26 PM

#ifndef CONFIGCLASS_H
#define CONFIGCLASS_H

#include <string>

class ConfigClass {
public:
	ConfigClass();
	virtual ~ConfigClass();
	
	int LoadConfig(const char* file);
	int SaveConfig(const char* file);
	
	std::string serial_dev;
	std::string patch_file;
	int patch_enable;
	
	int regs_font;
	int regs_font_size;
	
	int asm_font;
	int asm_font_size;
	
	int set_bpc;
	int autoload_symbols;
	int auto_minimize;
	
	int show_sym_disassembler;
	int show_sym_memorybrowser;
	
	int exp3_enable;
	int exp3_size_kb;
	
	int ram_8mb;
	
	float update_interval;
	
};

#endif /* CONFIGCLASS_H */

