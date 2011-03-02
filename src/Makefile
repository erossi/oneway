# Copyright (C) 2011 Enrico Rossi
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

PRG_NAME = hospitv
MCU = atmega164p
OPTLEV = 2
FCPU = 1000000UL
PWD = $(shell pwd)
INC = -I/usr/lib/avr/include/

CFLAGS = $(INC) -Wall -Wstrict-prototypes -pedantic -mmcu=$(MCU) -O$(OPTLEV) -D F_CPU=$(FCPU)
LFLAGS =

PRGNAME = $(PRG_NAME)
# Uncomment if git tag is in use
GIT_TAG = "$(shell git describe --tags)"
PRGNAME = $(PRG_NAME)_$(GIT_TAG)

AR = avr-ar
CC = avr-gcc

DUDEPORT = /dev/ttyUSB0
DUDEDEV = stk500v2
DUDEPORT = usb
DUDEDEV = avrispmkII
# Use sudo for USB avrispmkII
DUDE = sudo avrdude -c $(DUDEDEV) -p $(MCU) -P $(DUDEPORT) -e -U flash:w:$(PRGNAME).hex

OBJCOPY = avr-objcopy -j .text -j .data -O ihex
OBJDUMP = avr-objdump
SIZE = avr-size

REMOVE = rm -f

objects = led.o uart.o

.PHONY: clean indent
.SILENT: help
.SUFFIXES: .c, .o

all: master slave

master: $(objects)
	$(CC) $(CFLAGS) -o $(PRGNAME)_master.elf main.c -D MASTER $(objects) $(LFLAGS)
	$(OBJCOPY) $(PRGNAME)_master.elf $(PRGNAME)_master.hex

slave: $(objects)
	$(CC) $(CFLAGS) -o $(PRGNAME)_slave.elf main.c -D SLAVE $(objects) $(LFLAGS)
	$(OBJCOPY) $(PRGNAME)_slave.elf $(PRGNAME)_slave.hex

debug.o:
	$(CC) $(CFLAGS) -D GITREL=$(GITRELEASE) -c debug.c

program:
	$(DUDE)

clean:
	$(REMOVE) *.elf *.hex $(objects)

version:
	# Last Git tag: $(GITRELEASE)
