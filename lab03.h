#include <iostream>
#include <vector>
#include <map>
#include <iomanip>

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

const uint32_t OPCODE = 7;
const uint32_t RD = 5;
const uint32_t FUNCT3 = 3;
const uint32_t RS = 5;
const uint32_t FUNCT7 = 7;

struct R_type {

    uint32_t    opcode;
    uint32_t    rd;
    uint32_t    funct3;
    uint32_t    rs1;
    uint32_t    rs2;
    uint32_t    funct7;

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

    void parse (uint32_t str) {
        opcode = str % (1 << OPCODE);       str = str >> OPCODE;
        rd = str % (1 << RD);               str = str >> RD;
        funct3 = str % (1 << FUNCT3);       str = str >> FUNCT3;
        rs1 = str % (1 << RS);              str = str >> RS;
        rs2 = str % (1 << RS);              str = str >> RS;
        funct7 = str;
    }

    void print() {
        cout << command[{funct7, funct3}] << rd << ", " << rs1 << ", " << rs2 <<'\n';
    };
};

struct I_type {

    uint32_t    opcode;
    uint32_t    rd;
    uint32_t    funct3;
    uint32_t    rs1;
    int32_t     cnst;

    map <vector <uint32_t>, string> command = {
        {{3, 0},            "LB     "},
        {{3, 1},            "LH     "},
        {{3, 2},            "LW     "},
        {{3, 4},            "LBU    "},
        {{3, 5},            "LHU    "},
        {{19, 0},           "ADDI   "},
        {{19, 2},           "SLTI   "},
        {{19, 3},           "SLTIU  "},
        {{19, 4},           "XORI   "},
        {{19, 6},           "ORI    "},
        {{19, 7},           "ANDI   "},
        {{103, 0},          "JALR   "},
    };

    void parse (uint32_t str) {
        opcode = str % (1 <<OPCODE);        str = str >> OPCODE;
        rd = str % (1 << RD);               str = str >> RD;
        funct3 = str % (1 << FUNCT3);       str = str >> FUNCT3;
        rs1 = str % (1 << RS);              str = str >> RS;
        cnst = int32_t (str - (str >> 11) * (2 << 11)); ;
    }

    void print() {
        cout << command[{opcode, funct3}] << rd << ", " << rs1 << ", " << cnst << '\n';
    };
};

struct S_type {

    uint32_t    opcode;
    uint32_t    cnst1;
    uint32_t    funct3;
    uint32_t    rs1;
    uint32_t    rs2;
    uint32_t    cnst2;

    int32_t     cnst;

    map <vector <uint32_t>, string> command = {
        {{35, 0},            "SB     "},
        {{35, 1},            "SH     "},
        {{35, 2},            "SW     "},
    };

    void parse (uint32_t str) {
        opcode = str % (1 << OPCODE);       str = str >> OPCODE;
        cnst1 = str % (1 << RD);            str = str >> RD;
        funct3 = str % (1 << FUNCT3);       str = str >> FUNCT3;
        rs1 = str % (1 << RS);              str = str >> RS;
        rs2 = str % (1 << RS);              str = str >> RS;
        cnst2 = str;

        uint32_t _cnst = (cnst2 << RD) + cnst1;
        cnst = int32_t (_cnst - (_cnst >> 11) * (2 << 11)); 
    }

    void print() {
        cout << command[{opcode, funct3}] << rs1 << ", " << rs2 << ", " << cnst << '\n';
    };
};

struct B_type {

    uint32_t    opcode;
    uint32_t    funct3;
    uint32_t    rs1;
    uint32_t    rs2;
    uint32_t    cnst1;
    uint32_t    cnst2;
    uint32_t    cnst3;
    uint32_t    cnst4;

    int32_t     cnst;

    map <vector <uint32_t>, string> command = {
        {{99, 0},            "BEQ    "},
        {{99, 1},            "BNE    "},
        {{99, 4},            "BLT    "},
        {{99, 5},            "BGE    "},
        {{99, 6},            "BLTU   "},
        {{99, 7},            "BGEU   "},

    };

    void parse (uint32_t str) {
        opcode = str % (1 << OPCODE);   str = str >> OPCODE;
        cnst3 = str % (1 << 1);         str = str >> 1;
        cnst1 = str % (1 << 4);         str = str >> 4;
        funct3 = str % (1 << FUNCT3);   str = str >> FUNCT3;
        rs1 = str % (1 << RS);          str = str >> RS;
        rs2 = str % (1 << RS);          str = str >> RS;
        cnst2 = str % (1 << 6);         str = str >> 6;
        cnst4 = str;

        uint32_t _cnst = (((cnst4 << 1) + cnst3 << 6) + cnst2 << 4) + cnst1 << 1;
        cnst = int32_t (_cnst - (_cnst >> 12) * (2 << 12)); 
    }

    void print() {
        cout << command[{opcode, funct3}] << rs1 << ", " << rs2 << ", " << cnst << '\n';
    };
};

struct U_type {

    uint32_t    opcode;
    uint32_t    rd;
    int64_t     cnst;

    map <vector <uint32_t>, string> command = {
        {{55},               "LUI    "},
        {{23},               "AUIPC  "}
    };

    void parse (uint32_t str) {
        opcode = str % (1 << OPCODE);   str = str >> OPCODE;
        rd = str % (1 << RD);           str = str >> RD;
        cnst = str;

        cnst = cnst << 12;
        cnst -= (cnst >> 31) * (2 << 31); 
    }

    void print() {
        cout << command[{opcode}] << rd << ", " << cnst << '\n';
    };
};

struct J_type {

    uint32_t    opcode;
    uint32_t    rd;
    uint32_t    cnst_19_12;
    uint32_t    cnst_11;
    uint32_t    cnst_10_1;
    uint32_t    cnst_20;
    int64_t     cnst;


    map <vector <uint32_t>, string> command = {
        {{111},               "JAL    "}
    };

    void parse (uint32_t str) {
        opcode = str % (1 << OPCODE);   str = str >> OPCODE;
        rd = str % (1 << RD);           str = str >> RD;
        cnst_19_12 = str % (1 << 8);    str = str >> 8;
        cnst_11 = str % (1 << 1);       str = str >> 1;
        cnst_10_1 = str % (1 << 10);    str = str >> 10;
        cnst_20 = str;

        cnst = (((cnst_20 << 8) + cnst_19_12 << 1) + cnst_11 << 10) + cnst_10_1 << 1;
        cnst -= (cnst >> 20) * (2 << 20); 
    }

    void print() {
        cout << command[{opcode}] << rd << ", " << cnst << '\n';
    };
};