# PSn00b Debugger
PSn00b Debugger is an open source debugging utility for the original PlayStation for both commercial and homebrew software. It works on regular consoles including the SCPH-100 series needing only a few very inexpensive modifications to the hardware making this the cheapest development kit solution for debugging homebrew software on the original PlayStation.

While not as powerful as the official debugging tools that worked with official development hardware it is still a significant upgrade over console text messages directed to the serial port. This software is currently a work in progress but is usable enough for basic debugging tasks and source code to ALL components of this debugger are completely open source and improvements by those willing to volunteer are welcome.

## Features
* Basic program trace operations.
* Run to cursor.
* MIPS disassembler.
* Register preview.
* Remote memory browser.
* Remote reset.
* Supports PS-EXE, CPE and ELF executables.
* Supports SN symbols files (does not support ELF symbols yet).
* Resolve program address to source line and vice versa.
* Bookmarks.
* Built-in serial message window.

## Usage Notes
When running the debugger for the first time you must specify the debug patch binary at in the Debugger Settings. This file should be included with the binary download and is typically named pspatch.bin. If you don't specify this file all debugging functionality on the console side will not work.

## To-do list
* Rewrite debugger code to GNU assembler.
* View GTE registers.
* View certain I/O registers such as DMA.
* Support for breakpoints.
* Support for variable watches.
* Support comms through Xplorer parallel port.
* PC side file access support (PCDRV).
* Improve Linux support on PC side debugger.

## Compiling
This program requires tinyxml2 and fltk 1.3.2 and was mainly written to be compiled using MinGW32.

To compile under Windows simply run "make -j<number of threads> CONF=Release". Ideally, you'd want to compile this program throught the Netbeans IDE as it allows for changes to be made to the build configuration if needed.

Linux support is currently not guaranteed mainly because the serial routines need to be reworked and doesn't really work properly under Linux. Everything else should work fine under Linux however.

## Credits & References
Huge thanks goes to nocash for his excellent PlayStation specs document and his file format notes on the PsyQ/SDevTC symbol files. This project would have not been possible without his research.

Essential references if you wish to contribute to this project:
[nocash's PSX specs document](http://problemkaputt.de/psx-spx.htm )
[nocash's PsyQ/SDevTC symbol file format notes](http://www.psxdev.net/forum/viewtopic.php?f=76&t=1318&p=13187#p13137 )
[LSI L64360 datasheet (cop0 registers match the PSX's CPU)](https://www.digchip.com/datasheets/download_datasheet.php?id=488501&part-number=L64360 )

## Changelog
**Version 0.25b (12/11/2018)**
* Initial beta release.