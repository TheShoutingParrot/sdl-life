# sdl-life's Makefile
# See the LICENSE file for the details about the license
# Copyright © 2020 Victor Ocampo
# All rights reserved

include config.mk

OBJ = src/main.o src/cell.o src/util.o

all: clean sdl-life 

.c.o:
	$(CC) -c $(CFLAGS) $<

$(OBJ): src/sdl-life.h 

sdl-life: $(OBJ)
	$(CC) -o $@ main.o cell.o util.o $(LDFLAGS)

install: all
	mkdir -p $(PREFIX)/bin
	mkdir -p $(APPLICATIONS)
	mkdir -p $(PREFIX)/share/sdl-life
	cp sdl-life $(PREFIX)/bin/sdl-life
	chmod +x $(PREFIX)/bin/sdl-life
	cp sdl-life.desktop $(APPLICATIONS)/sdl-life.desktop 
	cp images/logo.png $(PREFIX)/share/sdl-life/

uninstall: clean
	rm -f $(PREFIX)/bin/sdl-life
	rm -f $(APPLICATIONS)/sdl-life.desktop
	rm -f -r $(PREFIX)/share/sdl-life/

clean:
	rm -f sdl-life main.o cell.o util.o 

.PHONY: all sdl-life install uninstall clean
