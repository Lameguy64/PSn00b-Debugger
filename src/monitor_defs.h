#ifndef _MONITOR_DEFS_H
#define _MONITOR_DEFS_H

// Error values
#define M_ERR_OK		0xE0
#define M_ERR_INVALID	0xE1
#define M_ERR_NOCONT	0xE2

// Command values
#define M_CMD_PING		0xD0
#define M_CMD_INFO		0xD1
#define M_CMD_PAUSE		0xD2
#define M_CMD_RESUME	0xD3
#define M_CMD_STEP		0xD4
#define M_CMD_GETREGS	0xD5
#define M_CMD_GETMEM	0xD6
#define M_CMD_GETWORD	0xD7
#define M_CMD_SETREG	0xD8
#define M_CMD_SETWORD	0xD9
#define M_CMD_DBREAK	0xDA
#define M_CMD_RUNTO		0xDB
#define M_CMD_LINESTEP	0xDC
#define M_CMD_REBOOT	0xDF

// Event values
#define M_EV_START		0xC0
#define M_EV_PAUSE		0xC1
#define M_EV_EXCEPT		0xC2
#define M_EV_DBGFAIL	0xC3
#define M_EV_BREAK		0xC4
#define M_EV_RUN		0xC5

#endif // _MONITOR_DEFS_H