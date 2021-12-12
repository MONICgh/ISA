#include "lab03.h"

#define OUT(type, str) type.parse(str);\
                        type.print()


void disassembler (Section_header text, FILE* file) {

  fseek (file, text.sh_offset, SEEK_SET);

  uint32_t addr = text.sh_addr;

  freopen (file_out_name, "w", stdout);

  for (int i = 0; i < text.sh_size / 4; i++) {
  

    cout << hex << setfill('0') << setw (8) << addr << ":    " << dec;
    addr += 4;
    
    uint32_t str;
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

    if (eq(name, ".text")) disassembler(section_headers[i], file);
  }

  fclose(file);
}

int main () {

    read_elf_file ();

    return 0;
}