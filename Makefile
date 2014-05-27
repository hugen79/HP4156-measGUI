LIB_SOURCE = ./src/libs/ 
GUI_SOURCE = ./src/gui/
CC         = gcc

CFLAGS   = -I./inc/
OPTFLAGS = `pkg-config gtk+-3.0 --cflags`   
LDFLAGS  = `pkg-config gtk+-3.0 --libs` /usr/local/lib/libgpib.so.0
PROGRAM  = IVsweepGUI

all: _bin
	cd build; \
	mv $(PROGRAM) ../ \

_bin: _libs
	cd $(GUI_SOURCE); \
	$(CC) -c $(OPTFLAGS) *.c; \
	mv *.o ../../build ; \
	cd ../../build ; \
	$(CC) -o $(PROGRAM) *.o $(LDFLAGS);  

_libs: 
	mkdir build; \
	cd $(LIB_SOURCE); \
	$(CC) -c *.c; \
	mv *.o ../../build \

clean:
	rm -rf build; \
	rm $(PROGRAM); \
