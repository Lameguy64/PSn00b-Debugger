/* 
 * File:   RemoteMemClass.h
 * Author: Lameguy64
 *
 * Created on November 2, 2018, 7:51 PM
 */

#ifndef REMOTEMEMCLASS_H
#define REMOTEMEMCLASS_H

#include "CommsClass.h"

#define REMOTE_READAHEAD_WORDS	64

class RemoteMemClass {
public:
	
	RemoteMemClass();
	virtual ~RemoteMemClass();
	
	void SetMemSize(int size);
	void SetExpMem(int enable, int size);
	
	int Update(CommsClass *comms, unsigned int addr, int size, int force = false);
	
	int AddressValid(unsigned int addr);
	int TestUpdate(unsigned int addr, int size);
	
	unsigned int	remote_addr;
	int				remote_size;
	unsigned int	*remote_data;
	
private:
	
	int _memsize;
	
	int _expram_enabled;
	int _expram_size;
	
};

#endif /* REMOTEMEMCLASS_H */

