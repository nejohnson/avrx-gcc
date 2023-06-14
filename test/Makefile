CC = avr-gcc

LIBS = ../avrx-gcc.a

CFLAGS = -mmcu=atmega8 -I../include

SIMULAVR = simulavr.exe
SIMULAVROPTS = -d atmega8 -W 0x20,- -R 0x22,- -T exit -F 8000000
TRACEOPTS = -t trace.txt

TESTS = BasicTest1 BasicTest2 BasicTest3

TESTEXE = $(addsuffix .elf, $(TESTS))



all: $(TESTEXE)


%.elf : %.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@


run1: BasicTest1.elf
	@echo "Running simulation..."
	$(SIMULAVR) $(SIMULAVROPTS) -f $<
	
run2: BasicTest2.elf
	@echo "Running simulation..."
	$(SIMULAVR) $(SIMULAVROPTS) -f $<
	
run3: BasicTest3.elf
	@echo "Running simulation..."
	$(SIMULAVR) $(SIMULAVROPTS) -f $<

run1trace: BasicTest1.elf
	@echo "Running simulation with trace (-> trace.txt)..."
	$(SIMULAVR) $(SIMULAVROPTS) $(TRACEOPTS) -f $<


run3trace: BasicTest3.elf
	@echo "Running simulation with trace (-> trace.txt)..."
	$(SIMULAVR) $(SIMULAVROPTS) $(TRACEOPTS) -f $<



clean:
	rm -f BasicTest1.elf
	rm -f trace.txt
