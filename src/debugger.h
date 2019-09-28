// Created on September 14, 2019, 2:08 PM

#ifndef DEBUGGER_H
#define DEBUGGER_H

#define COMM_ERR_TIMEOUT	1
#define COMM_ERR_RUNNING	2
#define COMM_ERR_NOREPLY	3
#define COMM_ERR_NODEBUG	4
#define COMM_ERR_NOCONT		5

void CommErrorMsg(int err);

#endif /* DEBUGGER_H */

