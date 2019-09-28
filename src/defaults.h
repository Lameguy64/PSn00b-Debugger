// Created on August 27, 2019, 3:15 PM
 
#ifndef DEFAULTS_H
#define DEFAULTS_H

// Default font for register, memory and source code views
#ifndef __WIN32

#define DEFAULT_FONT		"mono"
#define DEFAULT_FONT_SIZE	11

#else

#define DEFAULT_FONT		"Courier New"
#define DEFAULT_FONT_SIZE	12

#endif

#define RAM_SIZE	1048576*2

#endif /* DEFAULTS_H */

