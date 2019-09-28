/* 
 * File:   RemoteMemClass.cpp
 * Author: Lameguy64
 * 
 * Created on November 2, 2018, 7:51 PM
 */

#include <iostream>

#include "RemoteMemClass.h"
#include "AboutUI.h"


RemoteMemClass::RemoteMemClass() {
	
	remote_addr = 0;
	remote_size = 0;
	remote_data = nullptr;
	
	_memsize = 1048576*2;	// Default 2MB setting
	
	_expram_enabled = 0;	// Default expansion RAM settings
	_expram_size = 0;
	
}

RemoteMemClass::~RemoteMemClass() {
	
	if( remote_data ) {
		free( remote_data );
	}
	
}

void RemoteMemClass::SetMemSize(int size) {
	
	_memsize = 1048576*size;
	
}

void RemoteMemClass::SetExpMem(int enable, int size) {
	
	_expram_enabled = enable;
	_expram_size = size;
	
}

int RemoteMemClass::Update(CommsClass* comms, unsigned int addr, int size, int force) {
	
	unsigned int fetch_addr;
	unsigned int fetch_size;
	int fetch = false;
	
	if( !comms->serial.IsOpen() ) {
		return 0;
	}
	
	// Top
	if( ( addr < remote_addr ) || ( force ) ) {
		
		fetch_addr = addr-REMOTE_READAHEAD_WORDS;
		fetch_size = size+(REMOTE_READAHEAD_WORDS*2);
		fetch = true;
		
	// Bottom
	} else if( ( addr+size ) > ( remote_addr+remote_size ) ) {
		
		fetch_addr = addr-REMOTE_READAHEAD_WORDS;
		fetch_size = size+(REMOTE_READAHEAD_WORDS*2);
		fetch = true;
		
	}
	
	if( ( fetch ) || ( force ) ) {
		
		// Check if start and end is in invalid
		if( !AddressValid( fetch_addr ) && 
			!AddressValid( fetch_addr+(fetch_size-4) ) ) {
			
			return 0;
			
		}
		
		// Clip address and make sure it's within a valid region
		while( !AddressValid( fetch_addr ) ) {
			fetch_addr += 4;
		}
		
		while( !AddressValid( fetch_addr+(fetch_size-4) ) ) {
			fetch_addr -= 4;
		}
		
		if( ( remote_addr != fetch_addr ) || ( force ) ) {
			
			remote_addr = fetch_addr;
			remote_size = fetch_size;

			if( remote_data ) {
				
				free( remote_data );
				
			}
			
			#ifdef DEBUG
			std::cout << "Fetch=" << std::hex << remote_addr 
				<< std::dec << ":" << remote_size 
				<< std::hex << " E:" << (remote_addr+(remote_size-4)) << std::endl;
			#endif
			
			
			remote_data = (unsigned int*)malloc( remote_size );
			memset( remote_data, 0, remote_size );
	
			if( comms->dbGetMemory( remote_addr, remote_size, remote_data ) != remote_size ) {
				
				return 2;
				
			}
			
			return 1;
			
		}
		
	}
	
	return 0;
	
}

int RemoteMemClass::TestUpdate(unsigned int addr, int size) {
	
	unsigned int fetch_addr;
	unsigned int fetch_size;
	int fetch = false;
	
	// Top
	if( addr < remote_addr ) {
		
		fetch_addr = addr-REMOTE_READAHEAD_WORDS;
		fetch_size = size+(REMOTE_READAHEAD_WORDS*2);
		fetch = true;
		
	// Bottom
	} else if( ( addr+size ) > ( remote_addr+remote_size ) ) {
		
		fetch_addr = addr-REMOTE_READAHEAD_WORDS;
		fetch_size = size+(REMOTE_READAHEAD_WORDS*2);
		fetch = true;
		
	}
	
	if( fetch ) {
		
		// Check if start and end is in invalid
		if( !AddressValid( fetch_addr ) && 
			!AddressValid( fetch_addr+(fetch_size-4) ) ) {
			
			return 0;
			
		}
		
		// Clip address and make sure it's within a valid region
		while( !AddressValid( fetch_addr ) ) {
			fetch_addr += 4;
		}
		
		while( !AddressValid( fetch_addr+(fetch_size-4) ) ) {
			fetch_addr -= 4;
		}
		
		if( remote_addr != fetch_addr ) {
			
			return 1;
			
		}
		
	}
	
	return 0;
	
}

int RemoteMemClass::AddressValid(unsigned int addr) {
	
	int ram_ok = false;
	
	// Check if in RAM region
	switch( (addr>>24) ) {
		case 0x00:
		case 0x80:
		case 0xa0:
			ram_ok = true;
			break;
	}
	
	if( ram_ok ) {
		if( ( addr&0x00ffffff ) < _memsize ) {
			return true;
		}
		return false;
	}
	
	// Check if in BIOS ROM region
	if( (addr>>20) == 0xbfc ) {
		if( ( addr&0x000fffff ) < 524288 ) {
			return true;
		}
	}
	
	// Check if in scratch pad
	if( (addr>>20) == 0x1f8 ) {
		if( ( addr&0x000fffff ) < 1024 ) {
			return true;
		}
	}
	
	// Check if in expansion region 1 (ie. cheat cart ROM)
	if( (addr>>20) == 0x1f0 ) {
		if( ( addr&0x000fffff ) < 524288 ) {
			return true;
		}
	}
	
	// Check if in expansion region 3 (for RAM expansion, useless as there's 
	// currently no such accessory but the H2000 has 1MB of RAM expansion)
	if( _expram_enabled ) {
		if( ( (addr>>20) == 0x1fa ) || (addr>>20) == 0x1fb ) {
			if( ( addr&0x001fffff ) < _expram_size ) {
				return true;
			}
		}
	}
	
	return false;
	
}