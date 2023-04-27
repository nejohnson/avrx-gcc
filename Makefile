##############################################################################
#
# Makefile for AvrX for GCC (avrx-gcc)
#
##############################################################################
#
# Copyright (C) 2023 Neil Johnson
#
# See LICENSE.txt for license terms.
#
##############################################################################
#
# Targets:
#
# make (all)     Build AvrX library
# make install   Install AvrX library in INSTALLDIR
# make clean     Clean up the mess
#
##############################################################################

CC = avr-gcc
AR = avr-ar -rcsv

MCU = atmega8

TARGET = avrx-gcc

SRCDIR     = src
INCDIR     = include
BUILDDIR   = build
INSTALLDIR = ../lib

VPATH    = $(SRCDIR)

##############################################################################

CSRC = AvrXKernel.c

ASRC  = avrx_canceltimer.S 			\
		avrx_canceltimermessage.S 	\
		avrx_eeprom.S 				\
		avrx_message.S 				\
		avrx_priority.S 			\
		avrx_recvmessage.S 			\
		avrx_reschedule.S 			\
		avrx_resetsemaphore.S 		\
		avrx_semaphores.S 			\
		avrx_starttimermessage.S 	\
		avrx_suspend.S 				\
		avrx_tasking.S 				\
		avrx_terminate.S 			\
		avrx_testsemaphore.S 		\
		avrx_timequeue.S 	

OBJS = $(addprefix $(BUILDDIR)/,$(CSRC:.c=.o) $(ASRC:.S=.o))

##############################################################################

OPT = s

CFLAGS  = -mmcu=$(MCU)
CFLAGS += -I./$(INCDIR)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -std=gnu99

AFLAGS = -mmcu=$(MCU) -I./$(INCDIR) -x assembler-with-cpp

##############################################################################

all:  $(TARGET).a

clean:
	rm -f $(TARGET).a
	rm -rf $(BUILDDIR)

install: $(TARGET).a | $(INSTALLDIR)
	cp $(TARGET).a $(INSTALLDIR)
	cp $(INCDIR)/avrx.h $(INSTALLDIR)
	
##############################################################################

%.a: $(OBJS) Makefile
	$(AR) $@ $(OBJS)
	
$(BUILDDIR)/%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o : %.S
	$(CC) -c $(AFLAGS) $< -o $@

$(OBJS): | $(BUILDDIR)

$(BUILDDIR):
	mkdir $(BUILDDIR)

$(INSTALLDIR):
	mkdir -p $(INSTALLDIR)

.PHONY : all clean install Makefile

##############################################################################
##############################################################################
##############################################################################
