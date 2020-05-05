# paths
PREFIX          = /usr/local
APPLICATIONS    = /usr/share/applications
SDLINC          = /usr/include/SDL2

# SDL
SDLLIB          = SDL2

# libraries
INCS            = -I$(SDLINC)
LIBS            = -l$(SDLLIB)

# flags
CFLAGS          = -Wall -D_REENTRAT $(INCS)
LDFLAGS         = $(LIBS) 

# compiler
CC = gcc
