#include <iostream>
#include <vector>
#include <map>

using namespace std;

typedef struct {
    unsigned char   e_ident[16];
    uint16_t        e_type;
    uint16_t        e_machine;
    uint32_t        e_version;
    uint32_t        e_entry;
    uint32_t        e_phoff;
    uint32_t        e_shoff;
    uint32_t        e_flags;
    uint16_t        e_ehsize;
    uint16_t        e_phentsize;
    uint16_t        e_phnum;
    uint16_t        e_shentsize;
    uint16_t        e_shnum;
    uint16_t        e_shstrndx;
} Elf32_Ehdr;


typedef struct { 
    uint16_t        sh_name;
    uint32_t        sh_type;
    uint32_t        sh_flags;
    uint32_t        sh_addr;
    uint32_t        sh_offset;
    uint32_t        sh_size;
    uint32_t        sh_link;
    uint32_t        sh_info;
    uint32_t        sh_addralign;
    uint32_t        sh_entsize;
} Section_header;


const uint32_t OPCODE = (1 << 7);
const uint32_t RD = (1 << 5);
const uint32_t FUNCT3 = (1 << 3);
const uint32_t RS = (1 << 5);
const uint32_t FUNCT7 = (1 << 7);

struct R_type {

    uint32_t     opcode;
    uint32_t    rd;
    uint32_t    funct3;
    uint32_t    rs1;
    uint32_t    rs2;
    uint32_t    funct7;

    void parse (uint32_t str) {
        opcode = str % OPCODE; str /= OPCODE;
        rd = str % RD; str /= RD;
        funct3 = str % FUNCT3; str /= FUNCT3;
        rs1 = str % RS; str /= RS;
        rs2 = str % RS; str /= RS;
        funct7 = str % FUNCT7;
    }

    void print() {

        map <vector <uint32_t>, string> command = {
            {{0, 0},            "ADD    "},
            {{(1 << 5), 0},     "SUB    "},
            {{0, 1},            "SLL    "},
            {{0, 2},            "SLT    "},
            {{0, 3},            "SLTU   "},
            {{0, 4},            "XOR    "},
            {{0, 5},            "SRL    "},
            {{(1 << 5), 5},     "SRA    "},
            {{0, 6},            "OR     "},
            {{0, 7},            "AND    "},
            {{1, 0},            "MUL    "},
            {{1, 1},            "MULH   "},
            {{1, 2},            "MULHSU "},
            {{1, 3},            "MULHU  "},
            {{1, 4},            "DIV    "},
            {{1, 5},            "DIVU   "},
            {{1, 6},            "REM    "},
            {{1, 7},            "REMU   "},
        };

        cout << command[{funct7, funct3}] << rd << ", " << rs1 << ", " << rs2 <<'\n';

    };
};