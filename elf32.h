#ifndef ELF32_H
#define ELF32_H
/* This is my ELF */
#include <stdint.h>

/* Global data size */
typedef uint32_t Elf32_Addr; //Unsigned program address
typedef uint16_t Elf32_Half; //Unsigned medium integer
typedef uint32_t Elf32_Off; //Unsigned file offset
typedef int32_t  Elf32_Sword; //Signed large integer
typedef uint32_t Elf32_Word; //Unsigned small integer

/* ELF header related */

/* Index to access infomation in e_ident */
#define EI_MAG0		0	//Magic number byte 0
#define EI_MAG1		1	//Magic number byte 1
#define EI_MAG2		2	//Magic number byte 2
#define EI_MAG3		3	//Magic number byte 3
#define EI_CLASS	4	//File class
#define EI_DATA		5	//Data encoding
#define EI_VERSION	6	//File version
#define EI_PAD		7	//Start of padding bytes

#define EI_NIDENT	16	//Size of e_ident[]

/* ELF magic number in order */
#define ELFMAG0		0x7f
#define ELFMAG1		'E'
#define ELFMAG2		'L'
#define ELFMAG3		'F'

/* File class */
#define ELFCLASSNONE	0	//Invalid class
#define ELFCLASS32		1	//32-bit object
#define ELFCLASS64		2	//64-bit object

/* Data encoding */
#define ELFDATANONE		0	//Invalid data encoding
#define ELFDATA2LSB		1	//2's complement value with the LSB in lowest address
#define ELFDATA2MSB		2	//2's complement value with the MSB in lowest address

/* object file type (used in e_type feild) */
#define ET_NONE		0	//No file type
#define ET_REL		1	//Relocatable file
#define ET_EXEC		2	//Executable file
#define ET_DYN		3	//Shared object file
#define ET_CORE		4	//Core file
#define ET_LOPROC	0xff00	//Processor-specific
#define ET_HIPROC	0xffff	//Processor-specific

/* Architecture type (used in e_machine feild) */
/* Other value is reserved and will be assigned to new machine */
#define EM_NONE		0	//No machine
#define EM_M32		1
#define EM_SPARC	2
#define EM_386		3
#define EM_68K		4
#define EM_88K		5
#define EM_860		7
#define EM_MIPS		8

/* Object file version (used in e_version) */
#define EV_NONE 	0	//Invalid version
#define EV_CURRENT  1	//Current version

typedef struct {
	unsigned char	e_ident[EI_NIDENT];	//Specifiy interperation of file (identification)
	Elf32_Half		e_type;	//File type
	Elf32_Half		e_machine; //Architecture
	Elf32_Word		e_version; //Object file version
	Elf32_Addr		e_entry; //Virtual address to entry point.if no entry point, it remains 0
	Elf32_Off		e_phoff; //Program header table's file offset in bytes. if no program header table, it remains 0
	Elf32_Off		e_shoff; //Section header table's file offset in bytes. if no section header table, it remains 0
	Elf32_Word		e_flags; //Processor-specific flags related to file
	Elf32_Half		e_ehsize; //ELF header's size in bytes
	Elf32_Half		e_phentsize; //Size of entry in program header table (bytes). all entry has same size
	Elf32_Half		e_phnum; //Number of entries in program header table. if no section header table, it remains 0
	Elf32_Half		e_shentsize; //Size of entry in section header table (bytes). all entry has same size
	Elf32_Half		e_shnum; //Number of entries in setction header table. if no section header table, it remains 0
	Elf32_Half		e_shstrndx; //Section header table index of entry for section name string table. if no section name string table, it has value of SHN_UNDEF
} Elf32_Ehdr;


/* Section header related */

/* special section index to section header table*/
#define SHN_UNDEF		0
#define SHN_LORESERVE	0xff00
#define SHN_LOPROC		0xff00
#define SHN_HIPROC		0xff1f
#define SHN_ABS			0xfff1
#define SHN_COMMON		0xfff2
#define SHN_HIRESERVE	0xffff

/* section type (sh_type) */
#define SHT_NULL		0	//section header is inactive
#define SHT_PROGBITS	1	//section holds info defined by program
/* either one of section can reside in object file */
#define SHT_SYMTAB		2	//symbol table for link editing(complete symbol table)
#define SHT_DYNSYM		11 	//minimam symbol table (for dynamic link)

#define SHT_STRTAB		3	//string table
#define SHT_RELA		4	//relocation entries with explict addends
#define SHT_HASH		5	//symbol hash table, if dynamic link is used, this section must reside in object file. object fie may have only one hash table
#define SHT_DYNAMIC		6	//infomation for dynamic linking, only one
#define SHT_NOTE		7
#define SHT_NOBITS		8	//no space in the file
#define SHT_REL			9	//relocation entries without explict addedns
#define SHT_SHLIB		10
#define SHT_LOPROC		0x70000000
#define SHT_HIPROC		0x7fffffff
#define SHT_LOUSER		0x80000000
#define SHT_HIUSER		0xffffffff

/* attributes (sh_flags) */
#define SHF_WRITE		0x1	//section has data that should be writable during execution
#define SHF_ALLOC		0x2 //section occupies memory during execution
#define SHF_EXECINSTR	0x4	//executable instruction
#define SHF_MASKPROC	0xf0000000	//processor-specific semantics

typedef struct {
	Elf32_Word		sh_name; //name of section used as index to section header string table
	Elf32_Word		sh_type; //categoraize sectin content and semanitcs
	Elf32_Word		sh_flags; //attributes of secttion
	Elf32_Addr		sh_addr; //address of section in process image, zero otherwise
	Elf32_Off		sh_offset; //byte offset form file to the first byte in section
	Elf32_Word		sh_size; //section size in byte unless section type is SHT_NOBITS. in this case no space in the file
	Elf32_Word		sh_link; //section header table index link interpration depends on section type
	Elf32_Word		sh_info; //extra infomation whose interpretion depends on section type
	Elf32_Word		sh_addralign; //alignment restriction of sectin
	Elf32_Word		sh_entsize; //if section has fixed size, specify the size otherwise 0
} Elf32_Shdr;

/* symbol table entry */
typedef struct {
	Elf32_Word	st_name; //index into object file's symbol string table
	Elf32_Addr	st_value; //symbol value(address)
	Elf32_Word	st_size;  //data object size
	unsigned char st_info; //symbol's type and binding attributes
	unsigned char st_other; //0
	Elf32_Half	st_shndx; 
}Elf32_Sym;

/* Symbol Binding (st_info) */
#define STB_LOCAL		0 //symbol are not visible outside of object file defining it
#define STB_GLOBAL		1 //visible to all object files
#define STB_WEAK		2 //resembles to global, but have lower precedence
#define STB_LOPROC		13
#define STB_HIPROC		15

#define ELF32_ST_BIND(i)	((i)>>4)
#define ELF32_ST_TYPE(i)	((i)&0xf)
#define ELF32_ST_INFO(b,t)	(((b)<<4)+((t)&0xf))

/* Symbol types (st_info) */
#define STT_NOTYPE		0 //not specifed
#define STT_OBJECT		1 //data object
#define STT_FUNC		2 //function or executable
#define STT_SECTION		3 //mainly for relocation
#define STT_FILE		4 //st_name is name of source file
#define STT_LOPROC		13 
#define STT_HIPROC		15

typedef struct {
	Elf32_Addr	r_offset; //offset into section(location relative to section)
	Elf32_Word	r_info;
}Elf32_Rel;

typedef struct {
	Elf32_Addr	r_offset;
	Elf32_Word	r_info;
	Elf32_Sword r_addend;
}Elf32_Rela;

#define ELF32_R_SYM(i)		((i)>>8)
#define ELF32_R_TYPE(i) 	((unsigned char)(i))
#define ELF32_R_INFO(s,t)	((s<<8)+(unsigned char)(t))

#endif
