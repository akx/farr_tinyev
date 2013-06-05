CC      = gcc
DLLWRAP = dllwrap
OBJ     = farr_tinyev.o
BIN     = farr_tinyev.dll
#CFLAGS  = -dDEBUG
#LFLAGS  = -lws2_32
DEFFILE = farr_tinyev.def
RM      = del

.PHONY: all all-before all-after clean clean-custom

all: all-before farr_tinyev.dll all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

farr_tinyev.o: farr_tinyev.c
	$(CC) -s -Wall -Os -c -o farr_tinyev.o farr_tinyev.c  $(CFLAGS)
	
tinyev.res: tinyev.rc
	windres $< -O coff -o $@	

$(BIN): $(OBJ)
	$(DLLWRAP) --driver-name gcc -A $(OBJ) -o $(BIN)
	strip farr_tinyev.dll
	
install: $(BIN)
	copy /y $(BIN) "C:\Program Files (x86)\FindAndRunRobot\Plugins\farr_tinyev\farr_tinyev.dll"