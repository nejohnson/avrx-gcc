##############################################################################
#
# Makefile for AvrX for GCC (avrx-gcc)
#
##############################################################################
#
# Copyright (C) 2023-2025 Neil Johnson
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
# make what      Show help
#
##############################################################################

CC = avr-gcc
AR = avr-ar -rcsv

MCU = atmega8

TARGET = avrx-gcc

SRCDIR     = src
INCDIR     = inc
BUILDDIR   = build
INSTALLDIR = ../lib

VPATH    = $(SRCDIR)

##############################################################################

CSRC_CORE = 	avrxc_queuepid.c		\
		avrxc_appendobj.c		\
		avrxc_removeobj.c		\
		avrxc_removenextobj.c

CSRC =  $(CSRC_CORE)				\
	avrx_delay.c				\
	avrx_halt.c				\
	avrx_intsendmessage.c			\
	avrx_intsetsemaphore.c			\
	avrx_kernel.c				\
	avrx_priority.c 			\
	avrx_recvmessage.c			\
	avrx_resetsemaphore.c			\
	avrx_resume.c				\
	avrx_runtask.c				\
	avrx_sendmessage.c			\
	avrx_setsemaphore.c			\
	avrx_suspend.c				\
	avrx_systemobj.c			\
	avrx_taskexit.c 			\
	avrx_taskinit.c 			\
	avrx_terminate.c			\
	avrx_testsemaphore.c			\
	avrx_waitmessage.c			\
	avrx_waitsemaphore.c			\
	avrx_yield.c
	

ASRC  = avrx_canceltimer.S 			\
	avrx_canceltimermessage.S 		\
	avrx_message.S 				\
	avrx_recvmessage.S 			\
	avrx_reschedule.S 			\
	avrx_semaphores.S 			\
	avrx_starttimermessage.S 		\
	avrx_suspend.S 				\
	avrx_tasking.S 				\
	avrx_timequeue.S 	

OBJS = $(addprefix $(BUILDDIR)/,$(CSRC:.c=.o) $(ASRC:.S=.o))

##############################################################################

OPT = s

CFLAGS  = -mmcu=$(MCU)
CFLAGS += -I./$(INCDIR)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections
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

##############################################################################

what:
	@echo "Make avrx-gcc. Targets:"
	@echo "--------------------------"
	@echo "   (all)     Build AvrX library (default)"
	@echo "   install   Install AvrX library in INSTALLDIR"
	@echo "   clean     Clean up the mess"
	@echo "   what      Show this help"

##############################################################################

.PHONY : all clean install Makefile what

##############################################################################
##############################################################################
##############################################################################
