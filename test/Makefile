##############################################################################
##
## Makefile for avrx-gcc test suite
##
##############################################################################

CC = avr-gcc

LIBS = ../avrx-gcc.a

CFLAGS = -g -mmcu=atmega8 -I../include

SIMULAVR = simulavr
SIMULAVROPTS = -d atmega8 -W 0x20,- -R 0x22,- -T exit -F 8000000
TRACEOPTS = -t trace.txt

TESTS = BasicTest1 BasicTest2 BasicTest3 BasicTest4

TESTEXE = $(addsuffix .elf, $(TESTS))

##############################################################################

all: $(TESTEXE)

%.elf : %.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

##############################################################################
## Run targets
##############################################################################

run1: BasicTest1.elf
	@echo "Running simulation..."
	$(SIMULAVR) $(SIMULAVROPTS) -f $<
	
run2: BasicTest2.elf
	@echo "Running simulation..."
	$(SIMULAVR) $(SIMULAVROPTS) -f $<
	
run3: BasicTest3.elf
	@echo "Running simulation..."
	$(SIMULAVR) $(SIMULAVROPTS) -f $<

run4: BasicTest4.elf
	@echo "Running simulation..."
	$(SIMULAVR) $(SIMULAVROPTS) -f $<
	
##############################################################################
## Cleaning up the mess
##############################################################################

clean:
	rm -f BasicTest*.elf
	rm -f trace.txt
