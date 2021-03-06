/* 
 * File:   ConfigClass.cpp
 * Author: Lameguy64
 * 
 * Created on August 22, 2018, 7:26 PM
 */

#include <tinyxml2.h>
#include "ConfigClass.h"

ConfigClass::ConfigClass() {
	
	patch_enable = false;
	set_bpc = false;
	autoload_symbols = false;
	auto_minimize = true;
	
	show_sym_disassembler = true;
	show_sym_memorybrowser = true;
	
	ram_8mb = false;
	
	update_interval = 0.5f;
	
	exp3_enable = false;
	exp3_size_kb = 0;
	
	regs_font		= -1;
	regs_font_size	= -1;
	
	asm_font		= -1;
	asm_font_size	= -1;
	
}

ConfigClass::~ConfigClass() {
}

int ConfigClass::LoadConfig(const char* file) {
	
	tinyxml2::XMLDocument doc;
	
	if( doc.LoadFile( file ) != tinyxml2::XML_SUCCESS ) {
		return 1;
	}
	
	tinyxml2::XMLElement* base = doc.FirstChildElement( "psdebug_config" );
	if( !base ) {
		return 1;
	}
	
	tinyxml2::XMLElement* elem = base->FirstChildElement( "patcher" );
	if( elem ) {
		patch_enable = elem->IntAttribute( "enabled" );
		if( elem->Attribute( "file" ) ) {
			patch_file = elem->Attribute( "file" );
		}
	}
	
	elem = base->FirstChildElement( "comm_settings" );
	if( elem ) {
		if( elem->Attribute( "port" ) ) {
			serial_dev = elem->Attribute( "port" );
		}
	}
	
	elem = base->FirstChildElement( "exec_options" );
	if( elem ) {
	
		//set_bpc = elem->IntAttribute( "set_bpc", set_bpc );
		
		set_bpc = elem->IntAttribute("set_bpc");
		
	}
	
	elem = base->FirstChildElement( "options" );
	if( elem ) {
		
		/*
		autoload_symbols		= elem->IntAttribute( "load_symbols", autoload_symbols );
		auto_minimize			= elem->IntAttribute( "auto_minimize", auto_minimize );
		
		show_sym_disassembler	= elem->IntAttribute( "disassembler_symbols", show_sym_disassembler );
		show_sym_memorybrowser	= elem->IntAttribute( "memorybrowser_symbols", show_sym_memorybrowser );
		
		ram_8mb					= elem->IntAttribute( "ram_8mb", ram_8mb );
		*/
		
		autoload_symbols		= elem->IntAttribute("load_symbols");
		auto_minimize			= elem->IntAttribute("auto_minimize");
		
		show_sym_disassembler	= elem->IntAttribute("disassembler_symbols");
		show_sym_memorybrowser	= elem->IntAttribute("memorybrowser_symbols");
		
		ram_8mb					= elem->IntAttribute("ram_8mb");
		
		update_interval			= elem->FloatAttribute("update_interval");
		
	}
	
	elem = base->FirstChildElement( "exp_settings" );
	if( elem ) {
		
		exp3_enable		= elem->IntAttribute( "enabled" );
		exp3_size_kb	= elem->IntAttribute( "size" );
		
	}
	
	
	elem = base->FirstChildElement("font_registers");
	if( elem ) {
		
		if( elem->Attribute("font") ) {
			regs_font = elem->IntAttribute("font");
		}
		if( elem->Attribute("size") ) {
			regs_font_size = elem->IntAttribute("size");
		}

	}
	
	elem = base->FirstChildElement("font_disassembly");
	if( elem ) {
		
		if( elem->Attribute("font") ) {
			asm_font = elem->IntAttribute("font");
		}
		if( elem->Attribute("size") ) {
			asm_font_size = elem->IntAttribute("size");
		}

	}
	
	
	return 0;
	
}

int ConfigClass::SaveConfig(const char* file) {
	
	tinyxml2::XMLDocument doc;
	
	tinyxml2::XMLElement* base = doc.NewElement( "psdebug_config" );
	
	tinyxml2::XMLElement* node = doc.NewElement( "patcher" );
		node->SetAttribute( "enabled", patch_enable );
		node->SetAttribute( "file", patch_file.c_str() );
		base->InsertEndChild( node );
	
	node = doc.NewElement( "comm_settings" );
		node->SetAttribute( "port", serial_dev.c_str() );
		base->InsertEndChild( node );
		
	node = doc.NewElement( "exec_options" );
		node->SetAttribute( "set_bpc", set_bpc );
		base->InsertEndChild( node );
		
	node = doc.NewElement( "options" );
		node->SetAttribute( "load_symbols", autoload_symbols );
		node->SetAttribute( "auto_minimize", auto_minimize );
		node->SetAttribute( "disassembler_symbols", show_sym_disassembler );
		node->SetAttribute( "memorybrowser_symbols", show_sym_memorybrowser );
		node->SetAttribute( "ram_8mb", ram_8mb );
		node->SetAttribute("update_interval", (float)update_interval);
		base->InsertEndChild( node );
		
	node = doc.NewElement( "exp_settings" );
		node->SetAttribute( "enabled", exp3_enable );
		node->SetAttribute( "size", exp3_size_kb );
		base->InsertEndChild( node );
	
	node = doc.NewElement("font_registers");
		node->SetAttribute("font", regs_font);
		node->SetAttribute("size", regs_font_size);
		base->InsertEndChild(node);
		
	node = doc.NewElement("font_disassembly");
		node->SetAttribute("font", asm_font);
		node->SetAttribute("size", asm_font_size);
		base->InsertEndChild(node);
	
	doc.InsertFirstChild( base );
	
	if( doc.SaveFile( file ) != tinyxml2::XML_SUCCESS ) {
		return 1;
	}
	
	return 0;
}
