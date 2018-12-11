/* 
 * File:   SymbolsClass.cpp
 * Author: Lameguy64
 * 
 * Created on August 14, 2018, 1:54 PM
 */

#include <stdio.h>
#include <string.h>
#include <iostream>

#include "SymbolsClass.h"

SymbolsClass::SourceClass::SourceClass() {
	
}

SymbolsClass::SourceClass::~SourceClass() {
	
}

unsigned int SymbolsClass::SourceClass::LineToAddr(int line) {
	
	int current_line = src_start;
	unsigned int current_addr = src_saddr;
	
	for( int i=0; i<lines.size(); i++ ) {
		
		if( line <= current_line ) {
			return current_addr;
		}
		
		if( !lines[i].absolute ) {
			
			current_addr = lines[i].addr;
			current_line += lines[i].lines;
			
		} else {
			
			current_addr = lines[i].addr;
			current_line = lines[i].lines;
			
		}
		
	}
	
	return 0;
	
}

int SymbolsClass::SourceClass::IsLineMapped(int line) {
	
	/*if( line < src_start ) {
		return 0;
	}*/
	
	int current_line = src_start;
	
	for( int i=0; i<lines.size(); i++ ) {
		
		if( line == current_line ) {
			return 1;
		}
		
		if( !lines[i].absolute ) {
			
			current_line += lines[i].lines;
			
		} else {
			
			current_line = lines[i].lines;
			
		}
		
	}
	
	return 0;
	
}


SymbolsClass::SymbolsClass() {
}

SymbolsClass::~SymbolsClass() {
	
	if( sources.size() > 0 ) {
		
		for( int i=0; i<sources.size(); i++ ) {
			delete sources[i];
		}
		
		sources.clear();
		
	}
	
}

SymbolsClass::SourceClass *SymbolsClass::AddrToLine(unsigned int addr, int *line) {
	
	for( int s=0; s<sources.size(); s++ ) {
		
		SourceClass *src = sources[s];
		
		int current_line = src->src_start;
		unsigned int current_addr = src->src_saddr;
		unsigned int last_addr = current_addr;
		int last_line;

		for( int i=0; i<src->lines.size(); i++ ) {

			if( ( addr > last_addr ) && ( addr < current_addr ) ) {
				*line = last_line;
				return src;
			}
			if( addr <= current_addr ) {
				*line = current_line;
				return src;
			}

			last_addr = current_addr;
			last_line = current_line;
					
			if( !src->lines[i].absolute ) {

				current_addr = src->lines[i].addr;
				current_line += src->lines[i].lines;

			} else {

				current_addr = src->lines[i].addr;
				current_line = src->lines[i].lines;

			}

		}
	
	}
	
	return nullptr;
	
}

const char* SymbolsClass::FindSymbol(unsigned int addr) {
	
	for( int i=symbols.size()-1; i>=0; i-- ) {
		
		if( symbols[i].addr == addr ) {
			return symbols[i].name.c_str();
		}
		
	}
	
	return nullptr;
	
}

void SymbolsClass::ClearSymbols() {
	
	for( int i=0; i<sources.size(); i++ ) {
		delete sources[i];
	}
	sources.clear();
	
	symbols.clear();
	
}

int SymbolsClass::LoadSymbols(const char* filename) {
	
	FILE* fp = fopen( filename, "rb" );
	
	if( !fp ) {
		return -1;
	}
	
	{
		
		SYM_HEAD header;
		
		fread( &header, 1, sizeof(SYM_HEAD), fp );
		
		if( header.id != 0x01444e4d ) {
			fclose( fp );
			return -2;
		}
		
	}
	
	ClearSymbols();
	
	SYM_CHUNK chunk;
	char charbuff[256];
	int val;
	int end = 0;
	
	SourceClass* source = nullptr;
	SourceClass::LINE source_line;
	
	if( sources.size() > 0 ) {
		
		for( int i=0; i<sources.size(); i++ ) {
			delete sources[i];
		}
		
		sources.clear();
		
	}
	
	while( ( !end ) && ( !feof(fp) ) ) {
		
		fread( &chunk, 1, 5, fp );

		/*std::cout << std::hex << "POS=$" << ftell( fp ) << " ADDR=$" << chunk.addr;
		std::cout << "  ID=$" << (unsigned int)chunk.id << std::dec;*/
		
		SYM_NAME symbol;
		
		switch( chunk.id ) {
			case 0x1: // Function/variable
			case 0x2: 
			case 0x5:
			case 0x6:
				
				//std::cout << "  <var>" << std::endl;
				
				val = 0;
				fread( &val, 1, 1, fp );
				memset( charbuff, 0, 256 );
				fread( charbuff, 1, val, fp );
				//std::cout << "  FUNC/VAR=" << val << charbuff << std::endl;
				
				if( val > 0 ) {
					symbol.id = chunk.id;
					symbol.addr = chunk.addr;
					symbol.name = charbuff;
					
					symbols.push_back( symbol );
				}
				
				break;
				
			case 0x80:	// Source line address
				
				source_line.addr = chunk.addr;
				source_line.lines = 1;
				source_line.absolute = false;
				
				if( source ) {
					source->lines.push_back( source_line );
				}
				
				break;
				
			case 0x82:	// Source code address for N lines
				
				val = 0;
				fread( &val, 1, 1, fp );
				
				source_line.addr = chunk.addr;
				source_line.lines = val;
				source_line.absolute = false;
					
				if( source ) {
					source->lines.push_back( source_line );
				}
				
				break;
				
			case 0x86:	// Source code address for absolute line
				
				val = 0;
				fread( &val, 1, 4, fp );
				
				source_line.addr = chunk.addr;
				source_line.lines = val;
				source_line.absolute = true;
					
				if( source ) {
					source->lines.push_back( source_line );
				}
				break;
				
			case 0x88:	// Start of source with filename
				
				if( source != nullptr ) {
					sources.push_back( source );
				}
				
				source = new SourceClass();
				
				source->src_saddr = chunk.addr;
				
				fread( &source->src_start, 1, 4, fp );
				val = 0;
				fread( &val, 1, 1, fp );
				
				memset( charbuff, 0, 256 );
				fread( charbuff, 1, val, fp );
				source->src_file = charbuff;
				
				#ifdef DEBUG
				std::cout << std::hex << "POS=$" << ftell( fp ) 
					<< " Src Start = " << source->src_file << std::endl;
				#endif

				break;
				
			case 0x8a:	// Source code end address
				
				if( source != nullptr ) {
					
					#ifdef DEBUG
					std::cout << std::hex << "POS=$" << ftell( fp ) 
						<< " Src End = " << source->src_file << std::endl;
					#endif
					
					if( source->lines.size() ) {
						
						source->src_eaddr = chunk.addr;
						sources.push_back( source );
						
					} else {
						
						delete source;
						
					}
					
					source = nullptr;
				}
				
				break;
			
			case 0x8c:
				
				//std::cout << "  <func start>" << std::endl;
				fread( &val, 1, 4, fp );
				//std::cout << "  F.WHAT0=" << std::hex << val << std::endl;
				fread( &val, 1, 4, fp );
				//std::cout << "  F.WHAT1=" << val << std::endl;
				fread( &val, 1, 4, fp );
				//std::cout << "  F.WHAT2=" << val << std::endl;
				fread( &val, 1, 4, fp );
				//std::cout << "  F.WHAT3=" << val << std::endl;
				fread( &val, 1, 4, fp );
				//std::cout << "  F.WHAT4=" << val << std::endl;
				
				val = 0;
				fread( &val, 1, 1, fp );
				//std::cout << "  F.LEN=" << val << std::endl;
				
				memset( charbuff, 0, 256 );
				fread( charbuff, 1, val, fp );
				
				//std::cout << "  F.FILE=" << charbuff << std::endl;
				
				val = 0;
				fread( &val, 1, 1, fp );
				//std::cout << "  F.LEN=" << val << std::endl;
				
				memset( charbuff, 0, 256 );
				fread( charbuff, 1, val, fp );
				
				//std::cout << "  F.NAME=" << charbuff << std::endl;
				
				break;
			
			case 0x8e:
				
				//std::cout << "  <func end>" << std::endl;
				fread( &val, 1, 4, fp );
				//std::cout << "  F.LINE=" << val << std::endl;
				
				break;
				
			case 0x90:
				
				//std::cout << "  <func first inst>" << std::endl;
				fread( &val, 1, 4, fp );
				//std::cout << "  F.INST=" << std::hex << val << std::endl;
				
				break;
				
			case 0x92:
				
				//std::cout << "  <func last inst>" << std::endl;
				fread( &val, 1, 4, fp );
				//std::cout << "  F.INST=" << std::hex << val << std::endl;
				
				break;
				
			case 0x94:	// Symbol
				
				//std::cout << "  <symbol>" << std::endl;
				
				val = 0;
				fread( &val, 1, 2, fp );
				//std::cout << "  SYM.CLASS=" << std::hex << val << std::endl;
				
				fread( &val, 1, 2, fp );
				//std::cout << "  SYM.TYPE=" << val << std::endl;
				
				fread( &val, 1, 4, fp );
				//std::cout << "  SYM.SIZE=" << std::dec << val << std::endl;
				
				val = 0;
				fread( &val, 1, 1, fp );
				memset( charbuff, 0, 256 );
				fread( charbuff, 1, val, fp );
				
				//std::cout << "  SYM.NAME=" << charbuff << std::endl;
				
				break;
				
			case 0x96:	// Array symbol
				
				//std::cout << "  <symbol array>" << std::endl;
				
				val = 0;
				fread( &val, 1, 2, fp );
				//std::cout << "  SYM.CLASS=" << std::hex << val << std::endl;
				
				fread( &val, 1, 2, fp );
				//std::cout << "  SYM.TYPE=" << val << std::endl;
				
				fread( &val, 1, 4, fp );
				//std::cout << "  SYM.SIZE=" << std::dec << val << std::endl;
				
				val = 0;
				fread( &val, 1, 2, fp );
				//std::cout << "  SYM.DIM=" << std::dec << val << std::endl;
				
				{
					int val2;
					for( int i=0; i<val; i++ ) {
						val2 = 0;
						fread( &val2, 1, 4, fp );
						//std::cout << "  SYM.ARR[" << i << "]=" << val2 << std::endl;
					}
				}
				
				val = 0;
				fread( &val, 1, 1, fp );
				memset( charbuff, 0, 256 );
				fread( charbuff, 1, val, fp );
				//std::cout << "  SYM.FKNAME=" << val << " " << charbuff << std::endl;
				
				fread( &val, 1, 1, fp );
				memset( charbuff, 0, 256 );
				fread( charbuff, 1, val, fp );
				//std::cout << "  SYM.NAME=" << val << " " << charbuff << std::endl;
				
				break;
				
			default:
				//std::cout << std::endl;
				end = 1;
		}
		
	}
	
	if( source != nullptr ) {
		sources.push_back( source );
	}
	
	fclose( fp );
	
	#ifdef DEBUG
	std::cout << std::dec << std::endl;
	
	if( !end ) {
		std::cout << "SYM loaded successfully." << std::endl;
	}
	
	std::cout << "Found " << sources.size() << " source file(s)" << std::endl;
	std::cout << "Found " << symbols.size() << " symbol(s)" << std::endl;
	#endif
	
	return 0;
	
}