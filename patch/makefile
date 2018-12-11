TARGET	= pspatch.bin

FILES	= dbstub.bin

CFLAGS	= -Xm -Wall -O2
LDFLAGS	= /q /m /c

CC		= ccpsx
AS		= asmpsx
LD		= psylink

all: $(FILES)
	$(AS) /q /p install.s,$(TARGET)
	
dbstub.bin: dbstub.s dbstub.inc serial.inc
	$(AS) /q /p dbstub.s,dbstub.bin

clean:
	rm $(FILES) $(TARGET)