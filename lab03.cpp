#include "lab03.h"

#define OUT(type, str) type.parse(str);\
                        type.print()


void read_tags (Section_header tags, FILE* file, vector <Elf32_Sym> &symtab) {

  fseek (file, tags.sh_offset, SEEK_SET);

  uint32_t addr = tags.sh_addr;

  for (int i = 0; i < tags.sh_size / sizeof(Elf32_Sym); i++) {
    Elf32_Sym elem;
    fread (&elem, sizeof(Elf32_Sym), 1, file);
    symtab.push_back(elem);
  }
}

void disassembler (Section_header text, FILE* file, vector <Elf32_Sym> &symtab, uint32_t addr_name) {

  uint32_t addr = text.sh_addr;

  freopen (file_out_name, "w", stdout);

  for (int i = 0; i < text.sh_size / 4; i++) {

    cout << hex << setfill('0') << setw (8) << addr << ": " << dec;

    bool have_tag = 0;
    for (auto tag : symtab) {
      if (tag.st_value == addr) {
        char name[512];
        fseek (file, addr_name + tag.st_name, SEEK_SET);
        fscanf(file, "%s", name);
        cout << setfill(' ') << setw (0) << "<" << name << ">  ";
        have_tag = 1;  
      }
    }

    if (!have_tag) cout << "           ";

    addr += 4;

    uint32_t str;
    fseek (file, text.sh_offset + i * 4, SEEK_SET);
    fread (&str, sizeof(str), 1, file);

    uint32_t opcode = str % (1 << 7);

    if (opcode == 51) {
      R_type type;
      OUT(type, str);
    }
    else if (opcode == 3 || opcode == 19 || opcode == 103) {
      I_type type;
      OUT(type, str);
    }
    else if (opcode == 35) {
      S_type type;
      OUT(type, str);
    }
    else if (opcode == 99) {
      B_type type;
      OUT(type, str);
    }
    else if (opcode == 23 || opcode == 55 || opcode == 15 || opcode == 115) {
      U_type type;
      OUT(type, str);
    }
    else if (opcode == 111) {
      J_type type;
      OUT (type, str);
    }
  }
}

void read_elf_file () {

  FILE* file = fopen(file_in_name, "rb");
  
  Elf32_Ehdr header;
  fread (&header, sizeof(header), 1, file);

  vector <Section_header> section_headers;
  fseek (file, header.e_shoff, SEEK_SET);
  for (int i = 0; i < header.e_shnum; i++) {
    
    Section_header header_sect;
    fread (&header_sect, sizeof(header_sect), 1, file);

    section_headers.push_back (header_sect);
  }
  
  int symtab_index = -1;
  int text_index = -1;

  vector <Elf32_Sym> symtab;

  for (int i = 0; i < header.e_shnum; i++) {

    char name[512];
    fseek (file, section_headers[header.e_shstrndx].sh_offset + section_headers[i].sh_name, SEEK_SET);
    fscanf(file, "%s", name);

    auto eq = [] (char *s1, char *s2) {
      char *symbol1 = s1;
      char *symbol2 = s2;
      while (*symbol1 && *symbol1 == *symbol2) symbol1++, symbol2++;
      return *symbol1 == *symbol2;
    };

    if (eq(name, ".symtab")) symtab_index = i;
    else if (eq(name, ".text")) text_index = i;
  }

  read_tags(section_headers[symtab_index], file, symtab);
  disassembler(section_headers[text_index], file, symtab, section_headers[header.e_shstrndx].sh_offset);


  fclose(file);
}

int main () {

    read_elf_file ();

    return 0;
}