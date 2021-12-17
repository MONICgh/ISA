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

void write_symtab (vector <Elf32_Sym> &symtab, FILE* file) {
  
  cout << '\n';

  printf ("%s %-15s %7s %-8s %-8s %-8s %6s %s\n", 
    "Symbol", "Value", "Size", "Type", "Bind", "Vis", "Index", "Name");

  for (int i = 0; i < symtab.size(); i++) {

    Elf32_Sym tag = symtab[i];

    char name[512];
    fseek (file, address_name + tag.st_name, SEEK_SET);
    fscanf(file, "%s", name);  

    printf ("[%4i] 0x%-15X %5i %-8s %-8s %-8s %6s %s\n",
     i, tag.st_value, tag.st_size, ST_TYPE(tag.st_info).c_str(), 
     ST_BIND(tag.st_info).c_str(), ST_VISIBILITY(tag.st_info).c_str(), 
     get_sym_index(tag.st_shndx).c_str(), string(name).c_str());
  }
}

void disassembler (Section_header text, FILE* file,
 vector <Elf32_Sym> &symtab, const char* fout_name) {

  uint32_t addr = text.sh_addr;

  freopen (fout_name, "w", stdout);

  for (int i = 0; i < text.sh_size / 4; i++) {

    cout << hex << setfill('0') << setw (8) << addr << ": " << dec;

    int index_tag = -1;
    for (int i = 0; i < symtab.size(); i++) {
      if (symtab[i].st_value == addr) {
        index_tag = i;
      }
    }


    cout << setfill(' ') << setw (10);
    if (index_tag != -1) {
      
      char name[512];
      fseek (file, address_name + symtab[index_tag].st_name, SEEK_SET);
      fscanf(file, "%s", name);
      
       cout << name << ' ';
    }
    else cout << ' ' << ' ';

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

void read_elf_file (FILE* file, const char* fout_name) {
  
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

  address_name = section_headers[header.e_shstrndx].sh_offset;

  for (int i = 0; i < header.e_shnum; i++) {

    char name[512];
    fseek (file, address_name + section_headers[i].sh_name, SEEK_SET);
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
  disassembler(section_headers[text_index], file, symtab, fout_name);
  write_symtab(symtab, file);

}

int main (int argc, char const* argv[]) {

    const char* fin_name = argv[1];
    const char* fout_name = argv[2];

    FILE* fin = fopen(fin_name, "rb");

    read_elf_file (fin, fout_name);

    fclose(fin);

    return 0;
}