#include "lab03.h"
#include <vector>

using namespace std;


void disassembler (Section_header text, FILE* file) {

  fseek (file, text.sh_offset, SEEK_SET);
  for (int i = 0; i < text.sh_size; i++) {
    uint32_t str;
    fread (&str, sizeof(str), 1, file);
  }
}

void read_elf_file (const char* elf_file) {

  FILE* file = fopen("test_elf", "rb");
  
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

    if (name == ".text") disassembler(section_headers[i], file);
  }

  fclose(file);
}

int main () {

    read_elf_file ("test_elf");

    return 0;
}