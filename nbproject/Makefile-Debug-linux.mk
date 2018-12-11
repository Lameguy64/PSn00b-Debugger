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
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug-linux
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/AboutUI.o \
	${OBJECTDIR}/CommsClass.o \
	${OBJECTDIR}/ConfigClass.o \
	${OBJECTDIR}/Fl_Disassembler.o \
	${OBJECTDIR}/Fl_RegView.o \
	${OBJECTDIR}/Fl_Source_Browser.o \
	${OBJECTDIR}/MainUI.o \
	${OBJECTDIR}/ProgramClass.o \
	${OBJECTDIR}/ProjectClass.o \
	${OBJECTDIR}/SerialClass.o \
	${OBJECTDIR}/SettingsUI.o \
	${OBJECTDIR}/SourceUI.o \
	${OBJECTDIR}/SymbolsClass.o \
	${OBJECTDIR}/about.o \
	${OBJECTDIR}/disassembler.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/settings.o


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
LDLIBSOPTIONS=-lfltk -ltinyxml2

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/psdebug

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/psdebug: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/psdebug ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/AboutUI.o: AboutUI.cxx 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AboutUI.o AboutUI.cxx

${OBJECTDIR}/CommsClass.o: CommsClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CommsClass.o CommsClass.cpp

${OBJECTDIR}/ConfigClass.o: ConfigClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ConfigClass.o ConfigClass.cpp

${OBJECTDIR}/Fl_Disassembler.o: Fl_Disassembler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Fl_Disassembler.o Fl_Disassembler.cpp

${OBJECTDIR}/Fl_RegView.o: Fl_RegView.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Fl_RegView.o Fl_RegView.cpp

${OBJECTDIR}/Fl_Source_Browser.o: Fl_Source_Browser.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Fl_Source_Browser.o Fl_Source_Browser.cpp

${OBJECTDIR}/MainUI.o: MainUI.cxx 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MainUI.o MainUI.cxx

${OBJECTDIR}/ProgramClass.o: ProgramClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProgramClass.o ProgramClass.cpp

${OBJECTDIR}/ProjectClass.o: ProjectClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProjectClass.o ProjectClass.cpp

${OBJECTDIR}/SerialClass.o: SerialClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SerialClass.o SerialClass.cpp

${OBJECTDIR}/SettingsUI.o: SettingsUI.cxx 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SettingsUI.o SettingsUI.cxx

${OBJECTDIR}/SourceUI.o: SourceUI.cxx 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SourceUI.o SourceUI.cxx

${OBJECTDIR}/SymbolsClass.o: SymbolsClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SymbolsClass.o SymbolsClass.cpp

${OBJECTDIR}/about.o: about.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/about.o about.cpp

${OBJECTDIR}/disassembler.o: disassembler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/disassembler.o disassembler.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/settings.o: settings.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/settings.o settings.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/psdebug

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
