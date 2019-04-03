#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "elf32.h"

static void print_elf_header(Elf32_Ehdr *);
static void parse_elf_header(Elf32_Ehdr *);
static void print_section_header(Elf32_Shdr *, size_t size, char *);
static int find_section(Elf32_Shdr *, size_t, char *, char *);
static void print_symbol_table(Elf32_Sym *, size_t , char *);
static void usage(void);

int main(int argc, char **argv) {
	char *filename = NULL;
	char *shstr = NULL;
	char *strtab = NULL;
	int opt;
	int fd;
	int i;
	int symtabn = 0;
	int strtabn = 0;
	unsigned int hflags = 0;
	unsigned int Sflags = 0;
	unsigned int sflags = 0;
	Elf32_Ehdr *elf_header;
	Elf32_Shdr *section_header;
	Elf32_Sym  *symtab;

	while((opt = getopt(argc, argv, "hSsf:")) != -1) {
		switch(opt) {
			case 'h':
				hflags = 1;
				break;
			case 'f':
				filename = optarg;
				break;
			case 'S':
				Sflags = 1;
				break;
			case 's':
				sflags = 1;
				break;
			default:
				usage();
				break;
		}
	}
	if(filename == NULL) {
		fprintf(stderr, "filename is not specified with -f option\n");
		usage();
	}

	fd = open(filename, O_RDONLY);
	/* extract ELF header */
	elf_header = malloc(sizeof(Elf32_Ehdr));
	read(fd, elf_header, sizeof(Elf32_Ehdr)); 
	/* extract section header */
	section_header = malloc(sizeof(Elf32_Shdr) * elf_header->e_shnum);
	lseek(fd, 0, SEEK_SET);
	lseek(fd, elf_header->e_shoff, SEEK_CUR);
	read(fd, section_header, (sizeof(Elf32_Shdr) * elf_header->e_shnum));
	/* extract section header string table */
	shstr = malloc(sizeof(char) * section_header[elf_header->e_shstrndx].sh_size);
	lseek(fd, 0, SEEK_SET);
	lseek(fd, section_header[elf_header->e_shstrndx].sh_offset, SEEK_CUR);
	read(fd, shstr, (sizeof(char) * section_header[elf_header->e_shstrndx].sh_size));
	/* extract string table */
	strtabn = find_section(section_header, elf_header->e_shnum, shstr, ".strtab");
	strtab = malloc(sizeof(char) * section_header[strtabn].sh_size);
	lseek(fd, 0, SEEK_SET);
	lseek(fd, section_header[strtabn].sh_offset, SEEK_CUR);
	read(fd, strtab, section_header[strtabn].sh_size);
	/* extract symbol table */
	symtabn = find_section(section_header, elf_header->e_shnum, shstr, ".symtab");
	/* TODO: check if symtabn is non-zero */
	symtab = malloc(sizeof(Elf32_Sym) * section_header[symtabn].sh_size);
	lseek(fd, 0, SEEK_SET);
	lseek(fd, section_header[symtabn].sh_offset, SEEK_CUR);
	read(fd, symtab, section_header[symtabn].sh_size);

	if(hflags) {
		print_elf_header(elf_header);
		return EXIT_SUCCESS;
	}
	if(Sflags) {
		print_section_header(section_header, elf_header->e_shnum, shstr);
	}
	if(sflags) {
		print_symbol_table(symtab, section_header[symtabn].sh_size/sizeof(Elf32_Sym), strtab);
	}
	close(fd);
}

static void print_elf_header(Elf32_Ehdr *p) {
	int i;
	printf("ELF Header\nMagic:\t");
	for(i = 0; i < EI_NIDENT; i++) {
		printf("%02x ", p->e_ident[i], p->e_ident[i]);		
	}
	puts("");
	printf("%-10s%16x\n", "type", p->e_type);
	printf("%-10s%16x\n", "machine", p->e_machine);
	printf("%-10s%16x\n", "version", p->e_version);
	printf("%-10s%16x\n", "entry", p->e_entry);
	printf("%-10s%16d\n", "phoff", p->e_phoff);
	printf("%-10s%16d\n", "shoff", p->e_shoff);
	printf("%-10s%16x\n", "flags", p->e_flags);
	printf("%-10s%16d\n", "ehsize", p->e_ehsize);
	printf("%-10s%16d\n", "phentsize", p->e_phentsize);
	printf("%-10s%16d\n", "phnum", p->e_phnum);
	printf("%-10s%16d\n", "shentsize", p->e_shentsize);
	printf("%-10s%16d\n", "shnum", p->e_shnum);
	printf("%-10s%16x\n", "shstrndx", p->e_shstrndx);
}
static void print_section_header(Elf32_Shdr *p, 
	size_t size, char *shstr) {
	int i; 
	for(i = 0; i < size; i++) {
		printf("[Nr] %-6d Name %-8s Type %-20x Address %-20x Offset %-20x\n", i, &shstr[p[i].sh_name],
		p[i].sh_type, p[i].sh_addr, p[i].sh_offset);
		printf("%-6s Size %-8x Entsize %-20x Flags %-2x Link %-2x Info Align %-2x %+20x\n", "", p[i].sh_size,
			p[i].sh_entsize, p[i].sh_flags, p[i].sh_link, 
			p[i].sh_info, p[i].sh_addralign);
		puts("");
	}
}
static int find_section(Elf32_Shdr *p, size_t size, char *shstr, char *symbol) {
	int i, f = 0;
	for(i = 0; i < size; i++) {
		if(strcmp(symbol, &shstr[p[i].sh_name]) == 0) {
			f = 1;
			break;
		}
	}
	return (f) ? i : 0;
}
static void print_symbol_table(Elf32_Sym *symtab, size_t size, char *strtab) {
	int i;
	for(i = 0; i < size; i++) {
		printf("%s\n", &strtab[symtab[i].st_name]);
	}
}
static void usage(void) {
	fprintf(stderr, "dissect ELF :) \n");
	fprintf(stderr, "Usage: [-f filename] [-t]\n");
	exit(EXIT_FAILURE);	
}
