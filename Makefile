TARGET		= psdebug

SOURCES		= src widgets forms

CPPFILES	= $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))
CXXFILES	= $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cxx))

OFILES		= $(addprefix build/,$(CPPFILES:.cpp=.o) $(CXXFILES:.cxx=.o))

INCLUDE		= -Iforms -Iwidgets -Isrc
LIBDIRS		=
LIBS		= -ltinyxml2 -lfltk

# Windows specific stuff
ifeq "$(OS)" "Windows_NT"
INCLUDE		+= -IC:\fltk-1.3.4-1 -IC:\tinyxml2
LIBDIRS		+= -LC:\fltk-1.3.4-1\lib -LC:\tinyxml2
LIBS		+= -lcomctl32 -lcomdlg32 -lgdi32 -lole32 -luuid
WINRC		= psdebug.rc
WINRES		= psdebug.res
ifneq "$(CONF)" "DEBUG"
LDFLAGS		= -mwindows
endif
else
WINRC		=
WINRES		=
endif

ifeq "$(CONF)" "DEBUG"
CFLAGS		= -g -O2 -DDEBUG
CXXFLAGS	= $(CFLAGS)
else
CFLAGS		= -O2
CXXFLAGS	= $(CFLAGS)
endif

CC			= gcc
CXX			= g++

all: $(OFILES) $(WINRES)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBDIRS) $(OFILES) $(LIBS) $(WINRES) -o $(TARGET)

clean:
	rm -Rf build $(WINRES) $(TARGET)

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@
	
build/%.o: %.cxx
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

%.res: %.rc
	windres	$< -O coff -o $@
	