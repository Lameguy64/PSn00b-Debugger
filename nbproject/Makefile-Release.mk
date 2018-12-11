#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/b4460ae0/addressinput.o \
	${OBJECTDIR}/CommsClass.o \
	${OBJECTDIR}/ConfigClass.o \
	${OBJECTDIR}/ProgramClass.o \
	${OBJECTDIR}/ProjectClass.o \
	${OBJECTDIR}/RemoteMemClass.o \
	${OBJECTDIR}/SerialClass.o \
	${OBJECTDIR}/SymbolsClass.o \
	${OBJECTDIR}/about.o \
	${OBJECTDIR}/bookmarks.o \
	${OBJECTDIR}/forms/AboutUI.o \
	${OBJECTDIR}/forms/BookmarksUI.o \
	${OBJECTDIR}/forms/MainUI.o \
	${OBJECTDIR}/forms/MemDumpUI.o \
	${OBJECTDIR}/forms/MemoryUI.o \
	${OBJECTDIR}/forms/MessageUI.o \
	${OBJECTDIR}/forms/SettingsUI.o \
	${OBJECTDIR}/forms/SourceUI.o \
	${OBJECTDIR}/forms/SymbolsUI.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/memory.o \
	${OBJECTDIR}/memorydump.o \
	${OBJECTDIR}/message.o \
	${OBJECTDIR}/settings.o \
	${OBJECTDIR}/source.o \
	${OBJECTDIR}/widgets/Fl_Bookmark_Table.o \
	${OBJECTDIR}/widgets/Fl_Disassembler.o \
	${OBJECTDIR}/widgets/Fl_MemoryView.o \
	${OBJECTDIR}/widgets/Fl_RegView.o \
	${OBJECTDIR}/widgets/Fl_Source_Browser.o \
	${OBJECTDIR}/widgets/Fl_Status_Bar.o \
	${OBJECTDIR}/widgets/Fl_Symbol_Table.o \
	${OBJECTDIR}/widgets/Main_Window.o \
	${OBJECTDIR}/widgets/mips_disassembler.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/C/fltk-1.3.4-1/lib -L/C/tinyxml2 -lfltk -lcomctl32 -lcomdlg32 -lgdi32 -lole32 -luser32 -luuid -ltinyxml2 psdebug.res

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/psdebug.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/psdebug.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/psdebug ${OBJECTFILES} ${LDLIBSOPTIONS} -mwindows -s

${OBJECTDIR}/_ext/b4460ae0/addressinput.o: /C/Users/Lameguy64/Desktop/Projects/psdebug/addressinput.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b4460ae0
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b4460ae0/addressinput.o /C/Users/Lameguy64/Desktop/Projects/psdebug/addressinput.cpp

${OBJECTDIR}/CommsClass.o: CommsClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CommsClass.o CommsClass.cpp

${OBJECTDIR}/ConfigClass.o: ConfigClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ConfigClass.o ConfigClass.cpp

${OBJECTDIR}/ProgramClass.o: ProgramClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProgramClass.o ProgramClass.cpp

${OBJECTDIR}/ProjectClass.o: ProjectClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProjectClass.o ProjectClass.cpp

${OBJECTDIR}/RemoteMemClass.o: RemoteMemClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RemoteMemClass.o RemoteMemClass.cpp

${OBJECTDIR}/SerialClass.o: SerialClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SerialClass.o SerialClass.cpp

${OBJECTDIR}/SymbolsClass.o: SymbolsClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SymbolsClass.o SymbolsClass.cpp

${OBJECTDIR}/about.o: about.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/about.o about.cpp

${OBJECTDIR}/bookmarks.o: bookmarks.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bookmarks.o bookmarks.cpp

${OBJECTDIR}/forms/AboutUI.o: forms/AboutUI.cxx 
	${MKDIR} -p ${OBJECTDIR}/forms
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/forms/AboutUI.o forms/AboutUI.cxx

${OBJECTDIR}/forms/BookmarksUI.o: forms/BookmarksUI.cxx 
	${MKDIR} -p ${OBJECTDIR}/forms
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/forms/BookmarksUI.o forms/BookmarksUI.cxx

${OBJECTDIR}/forms/MainUI.o: forms/MainUI.cxx 
	${MKDIR} -p ${OBJECTDIR}/forms
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/forms/MainUI.o forms/MainUI.cxx

${OBJECTDIR}/forms/MemDumpUI.o: forms/MemDumpUI.cxx 
	${MKDIR} -p ${OBJECTDIR}/forms
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/forms/MemDumpUI.o forms/MemDumpUI.cxx

${OBJECTDIR}/forms/MemoryUI.o: forms/MemoryUI.cxx 
	${MKDIR} -p ${OBJECTDIR}/forms
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/forms/MemoryUI.o forms/MemoryUI.cxx

${OBJECTDIR}/forms/MessageUI.o: forms/MessageUI.cxx 
	${MKDIR} -p ${OBJECTDIR}/forms
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/forms/MessageUI.o forms/MessageUI.cxx

${OBJECTDIR}/forms/SettingsUI.o: forms/SettingsUI.cxx 
	${MKDIR} -p ${OBJECTDIR}/forms
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/forms/SettingsUI.o forms/SettingsUI.cxx

${OBJECTDIR}/forms/SourceUI.o: forms/SourceUI.cxx 
	${MKDIR} -p ${OBJECTDIR}/forms
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/forms/SourceUI.o forms/SourceUI.cxx

${OBJECTDIR}/forms/SymbolsUI.o: forms/SymbolsUI.cxx 
	${MKDIR} -p ${OBJECTDIR}/forms
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/forms/SymbolsUI.o forms/SymbolsUI.cxx

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/memory.o: memory.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/memory.o memory.cpp

${OBJECTDIR}/memorydump.o: memorydump.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/memorydump.o memorydump.cpp

${OBJECTDIR}/message.o: message.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/message.o message.cpp

${OBJECTDIR}/settings.o: settings.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/settings.o settings.cpp

${OBJECTDIR}/source.o: source.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/source.o source.cpp

${OBJECTDIR}/widgets/Fl_Bookmark_Table.o: widgets/Fl_Bookmark_Table.cpp 
	${MKDIR} -p ${OBJECTDIR}/widgets
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/widgets/Fl_Bookmark_Table.o widgets/Fl_Bookmark_Table.cpp

${OBJECTDIR}/widgets/Fl_Disassembler.o: widgets/Fl_Disassembler.cpp 
	${MKDIR} -p ${OBJECTDIR}/widgets
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/widgets/Fl_Disassembler.o widgets/Fl_Disassembler.cpp

${OBJECTDIR}/widgets/Fl_MemoryView.o: widgets/Fl_MemoryView.cpp 
	${MKDIR} -p ${OBJECTDIR}/widgets
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/widgets/Fl_MemoryView.o widgets/Fl_MemoryView.cpp

${OBJECTDIR}/widgets/Fl_RegView.o: widgets/Fl_RegView.cpp 
	${MKDIR} -p ${OBJECTDIR}/widgets
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/widgets/Fl_RegView.o widgets/Fl_RegView.cpp

${OBJECTDIR}/widgets/Fl_Source_Browser.o: widgets/Fl_Source_Browser.cpp 
	${MKDIR} -p ${OBJECTDIR}/widgets
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/widgets/Fl_Source_Browser.o widgets/Fl_Source_Browser.cpp

${OBJECTDIR}/widgets/Fl_Status_Bar.o: widgets/Fl_Status_Bar.cpp 
	${MKDIR} -p ${OBJECTDIR}/widgets
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/widgets/Fl_Status_Bar.o widgets/Fl_Status_Bar.cpp

${OBJECTDIR}/widgets/Fl_Symbol_Table.o: widgets/Fl_Symbol_Table.cpp 
	${MKDIR} -p ${OBJECTDIR}/widgets
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/widgets/Fl_Symbol_Table.o widgets/Fl_Symbol_Table.cpp

${OBJECTDIR}/widgets/Main_Window.o: widgets/Main_Window.cpp 
	${MKDIR} -p ${OBJECTDIR}/widgets
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/widgets/Main_Window.o widgets/Main_Window.cpp

${OBJECTDIR}/widgets/mips_disassembler.o: widgets/mips_disassembler.cpp 
	${MKDIR} -p ${OBJECTDIR}/widgets
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWIN32 -I/C/fltk-1.3.4-1 -I/C/tinyxml2 -Iwidgets -Iforms -I. -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/widgets/mips_disassembler.o widgets/mips_disassembler.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/psdebug.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
