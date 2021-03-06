// Originally created on June 18, 2018, 1:36 PM

#include <Fl/fl_message.H>
#include <iostream>
#include <vector>

#include "CommsClass.h"
#include "ProgramClass.h"
#include "MainUI.h"
#include "ConfigClass.h"
#include "mips_disassembler.h"
#include "monitor_defs.h"

extern ConfigClass config;


#define COMM_SHORT_DELAY	20
#define COMM_LONG_DELAY		100

#define CRC32_REMAINDER		0xFFFFFFFF


#ifndef __WIN32
void Sleep(int msec) {
	
}
#endif

void CommsClass::cb_UploadCancel(Fl_Widget* w, void* u) {
	
	CommsClass *comm = (CommsClass*)u;
	
	comm->cancel = true;
	
}

CommsClass::CommsClass() {
}

CommsClass::~CommsClass() {
}

void CommsClass::initTable32(unsigned int* table) {

	int i,j;
	unsigned int crcVal;

	for(i=0; i<256; i++) {

		crcVal = i;

		for(j=0; j<8; j++) {

			if (crcVal&0x00000001L)
				crcVal = (crcVal>>1)^0xEDB88320L;
			else
				crcVal = crcVal>>1;

		}

		table[i] = crcVal;

	}

}

unsigned int CommsClass::crc32(void* buff, int bytes, unsigned int crc) {

	int	i;
	unsigned char*	byteBuff = (unsigned char*)buff;
	unsigned int	byte;
	unsigned int	crcTable[256];

    initTable32(crcTable);

	for(i=0; i<bytes; i++) {

		byte = 0x000000ffL&(unsigned int)byteBuff[i];
		crc = (crc>>8)^crcTable[(crc^byte)&0xff];

	}

	return(crc^0xFFFFFFFF);

}

void* CommsClass::LoadCPE(FILE* fp, EXEC* param) {
	
	int v;
	unsigned int uv;
	
	char* exe_buff = nullptr;
	int exe_size = 0;
	unsigned int exe_entry = 0;
	
	std::vector<unsigned int> addr_list;
	
	fseek(fp, 0, SEEK_SET);
	fread(&v, 1, 4, fp);
	
	if ( v != 0x01455043 ) {
		fl_message("ERROR: File is not a PlayStation EXE or CPE file.");
		return nullptr;
	}
	
	memset(param, 0x0, sizeof(EXEC));
	
	v = 0;
	fread(&v, 1, 1, fp);
	
	while( v ) {
		
		switch( v ) {
			case 0x1:
				
				fread(&uv, 1, 4, fp);
				fread(&v, 1, 4, fp);
				
				addr_list.push_back(uv);
				exe_size += v;
				
				fseek(fp, v, SEEK_CUR);
				
				break;
				
			case 0x3:
				
				v = 0;
				fread(&v, 1, 2, fp);
				
				if ( v != 0x90 ) {
					fl_message("ERROR: Unknown SETREG code: %d", v);
					return nullptr;
				}
				
				fread(&exe_entry, 1, 4, fp);
				
				break;
				
			case 0x8:	// Select unit
				
				v = 0;
				fread(&v, 1, 1, fp);
				
				break;
				
			default:
				fl_message("ERROR: Unknown chunk encountered: %d", v);
				return nullptr;
		}
		
		v = 0;
		fread(&v, 1, 1, fp);
		
	}
	
	unsigned int addr_upper=0;
	unsigned int addr_lower=0;
	
	for(int i=0; i<addr_list.size(); i++) {
		if ( addr_list[i] > addr_upper ) {
			addr_upper = addr_list[i];
		}
	}
	
	addr_lower = addr_upper;
	
	for(int i=0; i<addr_list.size(); i++) {
		if ( addr_list[i] < addr_lower ) {
			addr_lower = addr_list[i];
		}
	}
	
	exe_size = 2048*((exe_size+2047)/2048);
	
	exe_buff = (char*)malloc(exe_size);
	memset(exe_buff, 0x0, exe_size);
	
	v = 0;
	fseek(fp, 4, SEEK_SET);
	fread(&v, 1, 1, fp);
	
	while( v ) {
		
		switch( v ) {
			case 0x1:
				
				fread(&uv, 1, 4, fp);
				fread(&v, 1, 4, fp);
				
				fread(exe_buff+(uv-addr_lower), 1, v, fp);
				
				break;
				
			case 0x3:
				
				v = 0;
				fread(&v, 1, 2, fp);
				
				if ( v == 0x90 ) {
					fseek(fp, 4, SEEK_CUR);
				}
				
				break;
				
			case 0x8:
				
				fseek(fp, 1, SEEK_CUR);
				
				break;
		}
		
		v = 0;
		fread(&v, 1, 1, fp);
		
	}
	
	param->pc0 = exe_entry;
	param->t_addr = addr_lower;
	param->t_size = exe_size;
	param->sp_addr = 0x801ffff0;
	
	return exe_buff;
	
}

int CommsClass::UploadExecutable(const char* exefile) {
	
	ProgressUI ui;
	PSEXE exe;
	EXEPARAM param;
	char* buffer;
	
	ui.progressBar->minimum( 0 );
	ui.progressBar->maximum( 100 );
	ui.progressBar->value( 0 );
	ui.progressText->label( "Uploading program..." );
	ui.show();
	
	FILE* fp = fopen(exefile, "rb");
	
	if ( fp == nullptr ) {
		fl_message("ERROR: File not found.");
		return -1;
	}
	
	if ( fread(&exe, 1, sizeof(PSEXE), fp) != sizeof(PSEXE) ) {
		fl_message("ERROR: Read error or invalid file.");
		fclose(fp);
		return -1;
	}
	
	if ( strcmp(exe.header, "PS-X EXE") ) {
		
		buffer = (char*)LoadCPE(fp, &param.params);
		
		if ( buffer == nullptr ) {
			fclose(fp);
			return -1;
		}
		
	} else {
		
		buffer = (char*)malloc(exe.params.t_size);
	
		if ( fread(buffer, 1, exe.params.t_size, fp) != exe.params.t_size ) {
			
			fl_message("ERROR: Incomplete file or read error occurred.");
			free(buffer);
			fclose(fp);
			
			return -1;
			
		}
		
		memcpy(&param.params, &exe.params, sizeof(EXEPARAM));
		
	}
	
	fclose(fp);
	
	param.crc32 = crc32(buffer, param.params.t_size, CRC32_REMAINDER);
	
	serial.SetRate(115200);
	serial.SendBytes((void*)"MEXE", 4);
	
	char reply[4];
	int timeout = true;
	for( int i=0; i<10; i++ ) {
		Sleep(COMM_SHORT_DELAY);
		if ( serial.ReceiveBytes(reply, 1) > 0 ) {
			timeout = false;
			break;
		}
	}
	if ( timeout ) {
		fl_message( "ERROR: No response from console 1." );
		return -1;
	}
	if ( reply[0] != 'K' ) {
		fl_message( "ERROR: No valid response from console 1." );
		return -1;
	}
	
	serial.SendBytes( &param, sizeof(EXEPARAM) );
	Sleep(COMM_SHORT_DELAY);
	
	int bytes_sent = 0;
	
	while( bytes_sent < param.params.t_size ) {
		
		int to_send = param.params.t_size-bytes_sent;
		if ( to_send > 2048 ) {
			to_send = 2048;
		}
		
		serial.SendBytes( buffer+bytes_sent, to_send );
		bytes_sent += to_send;
		
		ui.progressBar->value( 100.f*(bytes_sent/((float)param.params.t_size)) );
		Fl::check();
		
	}
	
	free( buffer );
	
	return 0;
	
}

int CommsClass::UploadProgram(ProgramClass* program) {
	
	ProgressUI	ui;
	EXEPARAM	param;
	
	ui.progressBar->minimum( 0 );
	ui.progressBar->maximum( 100 );
	ui.progressBar->value( 0 );
	ui.progressText->label( "Uploading program..." );
	ui.cancelButton->callback( cb_UploadCancel, this );
	ui.show();
	
	memcpy( &param.params, &program->exe_params, sizeof(EXEC) );
	
	param.crc32		= crc32(program->exe_data, param.params.t_size, CRC32_REMAINDER);
	param.flags		= 0;
	
	if( config.set_bpc ) {
		param.flags |= UploadSetBPC;
	}
	
	serial.SendBytes((void*)"MEXE", 4);
	
	char reply[4];
	int timeout = true;
	
	for( int i=0; i<10; i++ ) {
		
		Sleep(COMM_SHORT_DELAY);
		
		if ( serial.ReceiveBytes(reply, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	if( timeout ) {
		
		fl_message( "ERROR: No response from console 2." );
		return -1;
		
	}
	
	if( reply[0] != 'K' ) {
		
		fl_message( "ERROR: No valid response from console 2." );
		return -1;
		
	}
	
	serial.SendBytes( &param, sizeof(EXEPARAM) );
	Sleep(COMM_SHORT_DELAY);
	
	cancel = false;
	int bytes_sent = 0;
	
	while( bytes_sent < param.params.t_size ) {
		
		int to_send = param.params.t_size-bytes_sent;
		if ( to_send > 2048 ) {
			to_send = 2048;
		}
		
		serial.SendBytes( ((char*)program->exe_data)+bytes_sent, to_send );
		bytes_sent += to_send;
		
		ui.progressBar->value( 100.f*(bytes_sent/((float)param.params.t_size)) );
		Fl::check();
		
		if( ( cancel ) || ( !ui.shown() ) ) {
			return -1;
		}
		
	}
	
	return 0;
	
}

int CommsClass::UploadPatch(const char* binfile) {
	
	ProgressUI	ui;
	BINPARAM	param;
	
	fl_message_title( "Error Uploading Patch" );
	
	FILE* fp = fopen( binfile, "rb" );
	
	if( fp == nullptr ) {
		fl_message( "Unable to open file." );
		return -1;
	}
	
	fseek( fp, 0, SEEK_END );
	size_t size = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	
	char* bin = (char*)malloc( size );
	fread( bin, 1, size, fp );
	
	fclose( fp );
	
	ui.progressBar->minimum( 0 );
	ui.progressBar->maximum( 100 );
	ui.progressBar->value( 0 );
	ui.progressText->label( "Uploading debug patch..." );
	
	
	
	ui.show();
	
	param.crc32 = crc32( bin, size, CRC32_REMAINDER );
	param.size = size;
	param.addr = 0;
	
	serial.SendBytes( (void*)"MPAT", 4 );
	
	char reply[4];
	int timeout = true;
	for( int i=0; i<10; i++ ) {
		
		Sleep(COMM_SHORT_DELAY);
		
		if( serial.ReceiveBytes(reply, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	if( timeout ) {
		fl_message( "ERROR: No response from console." );
		free( bin );
		return -1;
	}
	
	if( reply[0] != 'K' ) {
		fl_message( "ERROR: No valid response from console." );
		free( bin );
		return -1;
	}
	
	serial.SendBytes( &param, sizeof(BINPARAM) );
	Sleep(COMM_SHORT_DELAY);
	
	cancel = false;
	int bytes_sent = 0;
	while( bytes_sent < param.size ) {
		
		int to_send = param.size-bytes_sent;
		if ( to_send > 2048 ) {
			to_send = 2048;
		}
		
		serial.SendBytes( ((char*)bin)+bytes_sent, to_send );
		bytes_sent += to_send;
		
		ui.progressBar->value( 100.f*(bytes_sent/((float)param.size)) );
		Fl::check();
		
		if( ( cancel ) || ( !ui.shown() ) ) {
			return -1;
		}
	}
	
	free( bin );
	
	Sleep(COMM_LONG_DELAY);
	
	return 0;
}

int CommsClass::DownloadMemory(unsigned int addr, unsigned int len, FILE* fp) {
	
	ProgressUI ui;
	int r,received = 0, collected = 0;;
	char buff[1024];
	
	ui.progressBar->minimum(0);
	ui.progressBar->maximum(100);
	ui.progressBar->value(0);
	ui.progressText->label("Downloading memory dump...");
	ui.show();
	
	// Flush serial interface
	while( serial.ReceiveBytes(buff, 16) > 0 ) {
		
		r += 16;
		
	}
	
	serial.SendByte(M_CMD_GETMEM);
	Sleep(COMM_SHORT_DELAY);
	serial.SendBytes(&addr, 4);
	Sleep(COMM_SHORT_DELAY);
	serial.SendBytes(&len, 4);
	
	while( received < len ) {
		
		int toget = len-received;
		
		if( toget > 16 ) {
			toget = 16;
		}
		
		r = serial.ReceiveBytes(buff+collected, toget);
		
		if( r <= 0 ) {
			
			if( received == 0 ) {
				return -1;
			} else {
				break;
			}
			
		}
		
		received += r;
		collected += r;
		
		if( collected > (1024-16) ) {
			fwrite(buff, 1, collected, fp);
			collected = 0;
		}
		
		ui.progressBar->value(100.f*(received/((float)len)));
		Fl::check();
		
		if( ( cancel ) || ( !ui.shown() ) ) {
			return -1;
		}
		
	}
	
	if( collected > 0 ) {
		
		fwrite(buff, 1, collected, fp);
		
	}
	
	return received;
	
}

int CommsClass::dbWaitBegin() {
	
	int reply = 0;
	int timeout = true;
	
	Sleep(COMM_SHORT_DELAY);
	
	for( int i=0; i<10; i++ ) {
		
		if( serial.ReceiveBytes(&reply, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	if( reply != M_EV_START ) {
		
		return 1;
		
	}
	
	return 0;
	
}

int CommsClass::dbPing() {
	
	int ret = 0;
	int timeout = true;
	
	serial.SendByte(M_CMD_PING);
	Sleep(COMM_SHORT_DELAY);
	
	for( int i=0; i<10; i++ ) {
		
		if( serial.ReceiveBytes(&ret, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	if( timeout ) {
		
		return -1;
		
	}
	
	if( ret != M_ERR_OK ) {
		
		return ret;
		
	}
	
	return 0;
	
}

int CommsClass::dbReboot() {
	
	int ret = 0;
	int timeout = true;
	
	serial.SendByte(M_CMD_REBOOT);
	Sleep(COMM_SHORT_DELAY);
	
	for( int i=0; i<10; i++ ) {
		
		if( serial.ReceiveBytes(&ret, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	if( timeout ) {
		
		return -1;
		
	}
	
	if( ret != M_ERR_OK ) {
		
		return ret;
		
	}
	
	return 0;
	
}

void CommsClass::dbPause() {
	
	unsigned int ret = 0;
	serial.SendByte(M_CMD_PAUSE);
	Sleep(COMM_LONG_DELAY);
	
}

int CommsClass::dbRunTo(unsigned int bpc) {
	
	serial.SendByte(M_CMD_RUNTO);
	Sleep(COMM_SHORT_DELAY);
	serial.SendBytes(&bpc, 4);
	Sleep(COMM_SHORT_DELAY);
	
	int ret = 0;
	int timeout = true;
	
	for( int i=0; i<10; i++ ) {
		
		if( serial.ReceiveBytes(&ret, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	Sleep(COMM_LONG_DELAY);
	
	if( timeout ) {
		return -1;
	}
	
	if( ret != M_ERR_OK ) {
		return ret;
	}
	
	return 0;
	
}

int CommsClass::dbStep() {
	
	serial.SendByte(M_CMD_STEP);
	
	int ret = 0;
	int timeout = true;
	
	Sleep(COMM_SHORT_DELAY);
	
	for( int i=0; i<10; i++ ) {
		
		if( serial.ReceiveBytes(&ret, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	if( timeout ) {
		return -1;
	}
	
	if( ret != M_ERR_OK ) {
		return ret;
	}
	
	Sleep(COMM_LONG_DELAY);
	
	return 0;
	
}

int CommsClass::dbSetBreak(unsigned int addr) {
	
	int ret;
	
	serial.SendBytes( (void*)"T", 1 );
	Sleep(COMM_SHORT_DELAY);
	serial.SendBytes( &addr, 4 );
	Sleep(COMM_SHORT_DELAY);
	
	ret = 0;
	serial.ReceiveBytes( &ret, 1 );
	
	if( ret != 'K' ) {
		return 0;
	}
	
	return 1;
	
}

int CommsClass::dbGetBreaks(unsigned int* buff) {
	
	serial.SendBytes( (void*)"V", 1 );
	
	int i = 0;
	
	while( 1 ) {
		
		int val = 0;
		serial.ReceiveBytes( &val, 4 );
		
		if( val == 0xffffffff ) {
			return i;
		} else if( val == 0 ) {
			break;
		}
		
		buff[i] = val;
		i++;
		
	}
	
	return -1;
	
}

int CommsClass::dbSetDataBreak(int flags, unsigned int addr, unsigned int mask) {
	
	int timeout=true;
	int ret=0;
	
	serial.SendByte(M_CMD_DBREAK);
	Sleep(COMM_SHORT_DELAY);
	serial.SendByte(flags);
	Sleep(COMM_SHORT_DELAY);
	serial.SendBytes(&addr, 4);
	Sleep(COMM_SHORT_DELAY);
	serial.SendBytes(&mask, 4);
	Sleep(COMM_LONG_DELAY);
	
	ret = 0;
	for( int i=0; i<10; i++ ) {
		
		if( serial.ReceiveBytes(&ret, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	if( timeout ) {
		return 1;
	}
	
	if( ret != M_ERR_OK ) {
		return 2;
	}
	
	return 0;
	
}

int CommsClass::dbContinue() {
	
	
	serial.SendByte(M_CMD_RESUME);
	
	int ret = 0;
	int timeout = false;
	
	Sleep(COMM_SHORT_DELAY);
	
	for( int i=0; i<10; i++ ) {
		
		if( serial.ReceiveBytes(&ret, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	if( timeout ) {
		return -1;
	}
	
	if( ret != M_ERR_OK ) {
		return ret;
	}
	
	return 0;
	
}

int CommsClass::dbRegisters(unsigned int* regs) {
	
	serial.SendByte(M_CMD_GETREGS);
	
	int ret = 0;
	int timeout = false;
	
	Sleep(COMM_SHORT_DELAY);
	
	for( int i=0; i<10; i++ ) {
		
		if( serial.ReceiveBytes(&ret, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	if( timeout ) {
		return -1;
	}
	
	if( ret != M_ERR_OK ) {
		return ret;
	}
	
	for(int i=0; i<40; i++) {
		
		if( serial.ReceiveBytes(&regs[i], sizeof(unsigned int)) == 0 ) {
			
			return 0;
			
		}
		
	}
	
	return 0;
	
}

int CommsClass::dbGetMemory(unsigned int pos, int len, void* buff) {
	
	serial.SendByte(M_CMD_GETMEM);
	Sleep(COMM_SHORT_DELAY);
	serial.SendBytes(&pos, 4);
	Sleep(COMM_SHORT_DELAY);
	serial.SendBytes(&len, 4);
	Sleep(COMM_SHORT_DELAY);
	
	int ret = 0;
	int timeout = false;
	
	for( int i=0; i<10; i++ ) {
		
		if( serial.ReceiveBytes(&ret, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	if( timeout ) {
		return -1;
	}
	
	if( ret != M_ERR_OK ) {
		return -ret;
	}
	
	int r,received = 0;
	
	while( received < len ) {
		
		int toget = len-received;
		
		if( toget > 16 ) {
			
			toget = 16;
			
		}
		
		r = serial.ReceiveBytes(((char*)buff)+received, toget);
		if( r <= 0 ) {
			
			if( received == 0 ) {
				
				return -1;
				
			} else {
				
				break;
				
			}
			
		}
		
		received += r;
		
	}
	
#ifdef DEBUG
	std::cout << "Read = " << std::dec << received << std::endl;
#endif
	
	return received;
	
}

int CommsClass::dbGetWord(unsigned int addr, int type, int &error) {
	
	serial.SendByte(M_CMD_GETWORD);
	Sleep(COMM_SHORT_DELAY);
	serial.SendByte(type);
	Sleep(COMM_SHORT_DELAY);
	serial.SendBytes(&addr, 4);
	Sleep(COMM_SHORT_DELAY);
	
	int ret = 0;
	int timeout = true;
	
	for( int i=0; i<10; i++ ) {

		if( serial.ReceiveBytes(&ret, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	if( timeout ) {
		error = 1;
		return -1;
	}
	
	if( ret != M_ERR_OK ) {
		error = 1;
		return ret;
	}
	
	Sleep(COMM_SHORT_DELAY);
	
	timeout = true;
	for( int i=0; i<10; i++ ) {
		
		if( serial.ReceiveBytes(&ret, 4) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	if( timeout ) {
		error = 1;
		return -1;
	}
	
	error = 0;
	return ret;
	
}

int CommsClass::dbSetWord(unsigned int addr, int type, unsigned int value) {
	
	int timeout = true;
	int ret = 0;
	
	serial.SendByte(M_CMD_SETWORD);
	Sleep(COMM_SHORT_DELAY);
	serial.SendByte(type);
	Sleep(COMM_SHORT_DELAY);
	serial.SendBytes(&addr, 4);
	Sleep(COMM_SHORT_DELAY);
	serial.SendBytes(&value, 4);
	Sleep(COMM_SHORT_DELAY);
	
	for( int i=0; i<10; i++ ) {
		
		if( serial.ReceiveBytes(&ret, 1) > 0 ) {
			timeout = false;
			break;
		}
		
	}
	
	Sleep(COMM_LONG_DELAY);
	
	if( timeout ) {
		return -1;
	}
	
	if( ret != M_ERR_OK ) {
		return ret;
	}
	
	return 0;
	
}