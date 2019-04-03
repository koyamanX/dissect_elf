CC=gcc
all: dissect_elf.c elf32.h
	$(CC) -o dissect_elf dissect_elf.c 
clean:
	rm -f dissect_elf
