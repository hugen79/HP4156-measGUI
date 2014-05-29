LIB_SOURCE = ./src/libs/
GUI_SOURCE = ./src/gui/
CC         = gcc

CFLAGS   = -c -I../inc/
OFLAGS 	 = -I./src/inc/
OPTFLAGS = `pkg-config gtk+-3.0 --cflags`   
LDFLAGS  = `pkg-config gtk+-3.0 --libs` /usr/local/lib/libgpib.so.0
PROGRAM  = IVsweepGUI

all: _bin
	cd build; \
	mv $(PROGRAM) ../ \

_bin: _libs
	cd $(GUI_SOURCE); \
	$(CC) $(CFLAGS) $(OPTFLAGS) gui_version2.c; \
	mv *.o ../../build ; \
	cd ../../build ; \
	$(CC) -o $(PROGRAM) *.o $(LDFLAGS) ;  

_libs: 
	mkdir build ; \
	cd $(LIB_SOURCE); \
	$(CC) $(CFLAGS) *.c; \
	mv *.o ../../build ;

clean:
	rm -rf build; \
	rm $(PROGRAM); \
