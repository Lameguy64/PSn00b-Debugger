/* 
 * File:   ProgramClass.cpp
 * Author: Lameguy64
 * 
 * Created on June 19, 2018, 10:20 AM
 */

#include <Fl/fl_message.H>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <vector>
#include <iostream>

#include "ProgramClass.h"
#include "pstypes.h"


#pragma pack(push, 1)

typedef struct {
	
	unsigned int magic;				// 0-3
	unsigned char word_size;		// 4
	unsigned char endianness;		// 5
	unsigned char elf_version;		// 6
	unsigned char os_abi;			// 7
	unsigned int unused[2];			// 8-15
	
	unsigned short type;			// 16-17
	unsigned short instr_set;		// 18-19
	unsigned int elf_version2;		// 20-23
	
	unsigned int prg_entry_addr;	// 24-27	
	unsigned int prg_head_pos;	// 28-31
	unsigned int sec_head_pos;	// 32-35
	unsigned int flags;				// 36-39
	unsigned short head_size;		// 40-41
	unsigned short prg_entry_size;	// 42-23
	unsigned short prg_entry_count;	// 44-45
	unsigned short sec_entry_size;	// 46-47
	unsigned short sec_entry_count;	// 48-49
	unsigned short sec_names_index;	// 50-51
	
} ELF_HEADER;

typedef struct {
	unsigned int seg_type;
	unsigned int p_offset;
	unsigned int p_vaddr;
	unsigned int undefined;
	unsigned int p_filesz;
	unsigned int p_memsz;
	unsigned int flags;
	unsigned int alignment;
} PRG_HEADER;

#pragma pack(pop)


ProgramClass::ProgramClass() {
	
	memset( &exe_params, 0, sizeof(EXEC) );
	
	exe_data = nullptr;
	
	/*
	remote_data = nullptr;
	remote_addr = 0;
	remote_size = 0;
	
	mode = 0;
	*/
	
}

ProgramClass::~ProgramClass() {
	
	if( exe_data ) {
		
		free( exe_data );
		
	}
	
	/*if( remote_data ) {
		
		free( remote_data );
		
	}*/
	
}

void ProgramClass::clear() {
	
	if( exe_data ) {
		
		free( exe_data );
		
	}
	
	/*if( remote_data ) {
		
		free( remote_data );
		
	}*/
	
	memset( &exe_params, 0, sizeof(EXEC) );
	
	exe_data = nullptr;
	/*
	remote_data = nullptr;
	mode = 0;
	*/
	
}

int ProgramClass::LoadProgram(const char* filename) {
	
	PSEXE exe;
	
	fl_message_title( "Error Loading Executable" );
	
	FILE* fp = fopen( filename, "rb" );
	
	if( fp == nullptr ) {
		fl_message( "ERROR: File not found." );
		return -1;
	}
	
	if( fread( &exe, 1, sizeof(PSEXE), fp ) != sizeof(PSEXE) ) {
		fl_message( "ERROR: Read error or invalid file." );
		fclose( fp );
		return -1;
	}
	
	if( strcmp( exe.header, "PS-X EXE" ) ) {
		
		exe_data = (unsigned int*)LoadCPE(fp, &exe.params);
		
		if( exe_data == nullptr ) {
			
			exe_data = (unsigned int*)LoadELF(fp, &exe.params);
			
			if( exe_data == nullptr ) {
				fl_message( "ERROR: File is not a supported executable file." );
				fclose(fp);
				return -1;
			}
			
		}
		
	} else {
		
		if( exe_data ) {
			free( exe_data );
		}
		
		exe_data = (unsigned int*)malloc( exe.params.t_size );
	
		if ( fread( exe_data, 1, exe.params.t_size, fp ) != exe.params.t_size ) {
			
			fl_message( "ERROR: Incomplete file or read error occurred." );
			
			free( exe_data );
			exe_data = nullptr;
			fclose( fp );
			
			return -1;
			
		}
		
	}
	
	fclose( fp );
	
	memcpy( &exe_params, &exe.params, sizeof(EXEC) );
	
	#ifdef DEBUG
	std::cout << "t_addr: " << std::hex << exe_params.t_addr << std::endl;
	std::cout << "pc0   : " << exe_params.pc0 << std::dec << std::endl;
	#endif
	
	return 0;
	
}

void* ProgramClass::LoadCPE(FILE* fp, EXEC* param) {
	
	int v;
	unsigned int uv;
	
	char* exe_buff = nullptr;
	unsigned int exe_size = 0;
	unsigned int exe_entry = 0;
	
	std::vector<unsigned int> addr_list;
	
	fseek( fp, 0, SEEK_SET);
	fread( &v, 1, 4, fp );
	
	if( v != 0x01455043 ) {
		return nullptr;
	}
	
	memset( param, 0x0, sizeof(EXEC) );
	
	v = 0;
	fread( &v, 1, 1, fp );
	
	while( v ) {
		
		switch( v ) {
			case 0x1:
				
				fread( &uv, 1, 4, fp );
				fread( &v, 1, 4, fp );
				
				addr_list.push_back( uv );
				exe_size += v;
				
				fseek( fp, v, SEEK_CUR );
				
				break;
				
			case 0x3:
				
				v = 0;
				fread( &v, 1, 2, fp );
				
				if ( v != 0x90 ) {
					fl_message("ERROR: Unknown SETREG code: %d", v);
					return nullptr;
				}
				
				fread( &exe_entry, 1, 4, fp );
				
				break;
				
			case 0x8:	// Select unit
				
				v = 0;
				fread( &v, 1, 1, fp );
				
				break;
				
			default:
				fl_message( "ERROR: Unknown chunk encountered: %d", v );
				return nullptr;
		}
		
		v = 0;
		fread(&v, 1, 1, fp);
		
	}
	
	unsigned int addr_upper=0;
	unsigned int addr_lower=0;
	
	for( int i=0; i<addr_list.size(); i++ ) {
		
		if( addr_list[i] > addr_upper ) {
			addr_upper = addr_list[i];
		}
		
	}
	
	addr_lower = addr_upper;
	
	for( int i=0; i<addr_list.size(); i++ ) {
		
		if( addr_list[i] < addr_lower ) {
			addr_lower = addr_list[i];
		}
		
	}
	
	exe_size = 2048*((exe_size+2047)/2048);
	
	exe_buff = (char*)malloc( exe_size );
	memset( exe_buff, 0x0, exe_size );
	
	v = 0;
	fseek( fp, 4, SEEK_SET );
	fread( &v, 1, 1, fp );
	
	while( v ) {
		
		switch( v ) {
			case 0x1:
				fread( &uv, 1, 4, fp );
				fread( &v, 1, 4, fp );
				fread( exe_buff+(uv-addr_lower), 1, v, fp );
				break;
			case 0x3:
				v = 0;
				fread( &v, 1, 2, fp );
				if( v == 0x90 ) {
					fseek( fp, 4, SEEK_CUR );
				}
				break;
			case 0x8:
				fseek( fp, 1, SEEK_CUR );
				break;
		}
		
		v = 0;
		fread( &v, 1, 1, fp );
		
	}
	
	param->pc0 = exe_entry;
	param->t_addr = addr_lower;
	param->t_size = exe_size;
	param->sp_addr = 0x801ffff0;
	
	return exe_buff;
	
}

void* ProgramClass::LoadELF(FILE* fp, EXEC* param) {
	
	ELF_HEADER head;
	
	memset( param, 0, sizeof(EXEC) );
	fseek( fp, 0, SEEK_SET );
	fread( &head, 1, sizeof(head), fp );
	
	// Check header
	if( head.magic != 0x464c457f ) {
		fl_message( "File is not an Executable and Linkable Format file." );
		return nullptr;
	}
	
	if( head.type != 2 ) {
		fl_message( "Only executable ELF files are supported." );
		return nullptr;
	}
	
	if( head.instr_set != 8 ) {
		fl_message( "ELF file is not a MIPS binary.\n" );
		return nullptr;
	}
	
	if( head.word_size != 1 ) {
		fl_message( "Only 32-bit ELF files are supported.\n" );
		return nullptr;
	}
	
	if( head.endianness != 1 ) {
		fl_message( "Only little endian ELF files are supported.\n" );
		return nullptr;
	}
	
	
	// Load program headers and determine binary size and load address
	PRG_HEADER prg_heads[head.prg_entry_count];
	
	param->t_addr = 0xffffffff;
	unsigned int exe_haddr = 0;
	param->t_size = 0;
	
	fseek( fp, head.prg_head_pos, SEEK_SET );
	for( int i=0; i<head.prg_entry_count; i++ ) {
	
		fread( &prg_heads[i], 1, sizeof(PRG_HEADER), fp );
		
		if( prg_heads[i].flags == 4 ) {
			continue;
		}
		
		if( prg_heads[i].p_vaddr < param->t_addr ) {
			param->t_addr = prg_heads[i].p_vaddr;
		}
		
		if( prg_heads[i].p_vaddr > exe_haddr ) {
			exe_haddr = prg_heads[i].p_vaddr;
		}
		
	}
	
	param->t_size = (exe_haddr-param->t_addr);
	param->t_size += prg_heads[head.prg_entry_count-1].p_filesz;
	
	
	// Check if load address is appropriate in main RAM locations
	/*if( ( ( param->t_addr>>24 ) == 0x0 ) || ( ( param->t_addr>>24 ) == 0x80 ) || 
		( ( param->t_addr>>24 ) == 0xA0 ) ) {
		
		if( ( param->t_addr&0x00ffffff ) < 65536 ) {
			
			printf( "Warning: Load address overwrites kernel area.\n" );
			
		}
		
	}*/
	
	
	// Pad out the size to multiples of 2KB
	param->t_size = 2048*((param->t_size+2047)/2048);
	
	// Load the binary data
	unsigned char* binary = (unsigned char*)malloc( param->t_size );
	memset( binary, 0x0, param->t_size );
	
	for( int i=0; i<head.prg_entry_count; i++ ) {
		
		if( prg_heads[i].flags == 4 ) {
			continue;
		}
		
		fseek( fp, prg_heads[i].p_offset, SEEK_SET );
		fread( &binary[(int)(prg_heads[i].p_vaddr-param->t_addr)], 
			1, prg_heads[i].p_filesz, fp );
		
	}
	
	param->pc0 = head.prg_entry_addr;
	param->sp_addr = 0x801ffff0;
	
	return binary;
	
}
