/* 
 * File:   pstypes.h
 * Author: Lameguy64
 *
 * Created on June 19, 2018, 10:39 AM
 */

#ifndef PSTYPES_H
#define PSTYPES_H

typedef struct {
	unsigned int pc0;
	unsigned int gp0;
	unsigned int t_addr;
	unsigned int t_size;
	unsigned int d_addr;
	unsigned int d_size;
	unsigned int b_addr;
	unsigned int b_size;
	unsigned int sp_addr;
	unsigned int sp_size;
	unsigned int sp;
	unsigned int fp;
	unsigned int gp;
	unsigned int ret;
	unsigned int base;
} EXEC;

typedef struct {
	char header[8];
	char pad[8];
	EXEC params;
	char license[64];
	char pad2[1908];
} PSEXE;

typedef struct {
	EXEC params;
	unsigned int crc32;
	unsigned int flags;
} EXEPARAM;

#endif /* PSTYPES_H */

