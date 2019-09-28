# PSn00bDEBUG
PSn00bDEBUG is an open source on-hardware debugger for the original
PlayStation for both existing game titles and homebrew software. It works
on pretty much any stock console including the SCPH-100 series, only
needing a few inexpensive modifications usually consisting of
installing an inexpensive 3.3v USB TTL UART device.

While not as powerful as the official debugging tools that worked with
official development hardware, this is still a significant upgrade over
basic text messages over serial TTY, and the debugging facilities provided
will help greatly with tracking down bugs, crashes, or testing functions
written in MIPS assembly quickly and easily.

## Features
* Exception handler.
* Full MIPS disassembler.
* MIPS register preview.
* Assembly level program trace operations (pause, step).
* Run to cursor.
* RAM browser.
* Watchpoints (works on I/O ports too).
* Single data access breakpoint.
* Watchpoint value edit.
* Soft reset.
* Bookmarks.
* Built-in serial message window.
* Supports PS-EXE, CPE and ELF executables.
* Supports PsyQ SYM symbols files (does not support ELF symbols yet).
* Address to line resolution and vice versa.
* Developed using 100% free and open source tools.

## Usage Notes
When running the debugger for the first time you must specify the debug
patch binary file in the Debugger Settings. This file should be included
with the binary download and is usually named pspatch.bin. If you don't
specify this file, all debugging functionality for the console side will
not work.

As of version 0.30 and onwards, the debugger only works with LITELOAD 1.3
and newer which you can find on its
[github repo](https://github.com/lameguy64/liteload).

## To-do list
* View GTE registers (implemented in monitor, not yet on debugger interface).
* Support for multiple program breakpoints.
* Support for ELF symbol data.
* PC side file access support (PCDRV).
* Linux support needs further testing.
* Source level code stepping.
* On some PCs, the debug monitor cannot communicate properly with the host PC.
  Unknown why this is happening as it does not happen on any of my computers.
  - lameguy64

## Compiling
This program requires tinyxml2 and fltk 1.3.2-1 and was mostly coded for
MinGW32 GCC 6.3.0.

To compile under Windows simply run "make -j<number of threads>".

Linux support needs further testing as the serial routines may not work
properly under Linux. Everything else that does not involve serial
communications should work fine however.

## Credits & References
Developed by Lameguy64 (John Wilbert Villamor) of Meido-Tek Productions

[nocash's PSX specs document](http://problemkaputt.de/psx-spx.htm )
[nocash's PsyQ/SDevTC symbol file format notes](http://www.psxdev.net/forum/viewtopic.php?f=76&t=1318&p=13187#p13137 )
[LSI L64360 datasheet (cop0 registers match the PSX's CPU)](https://www.digchip.com/datasheets/download_datasheet.php?id=488501&part-number=L64360 )

## Changelog
**Version 0.30b (TBR)**
* Moved main source files to src directory.
* Debug monitor and patcher code rewritten for ARMIPS.
* Improved debug step operation in monitor side.
* Now requires LITELOAD 1.3.
* Debug monitor protocol revised entirely.
* Register and disassembly view fonts can now be customized.
* Added follow undo.
* Added watchpoints, value on console side can be modified.
* Added single data access breakpoint via watchpoints.
* Some improvement on code structuring.
* Added auto view update option by interval.
* Added opcode patch capability.

**Version 0.26b (01/23/2019)**
* Fixed memory browser appearing when loading a project when the bookmark window is shown.
* Fixed incorrect register order in sllv, srlv and srav instructions.

**Version 0.25b (12/11/2018)**
* Initial beta release.