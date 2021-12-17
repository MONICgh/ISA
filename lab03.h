#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include <string>

using namespace std;

const uint32_t OPCODE = 7;
const uint32_t RD = 5;
const uint32_t FUNCT3 = 3;
const uint32_t RS = 5;
const uint32_t FUNCT7 = 7;

//const char* file_in_name = "test_elf";
//const char* file_out_name = "out.txt";

uint32_t address_name = -1;

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
    uint32_t        st_name;
    uint32_t        st_value;
    uint32_t        st_size;
    unsigned char   st_info;
    unsigned char   st_other;
    uint16_t        st_shndx;
} Elf32_Sym;

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

map <int, string> reg = {
    {0,  "zero  "},
    {1,  "ra    "},
    {2,  "sp    "},
    {3,  "gp    "},
    {4,  "tp    "},
    {5,  "t0    "},
    {6,  "t1    "},
    {7,  "t2    "},
    {8,  "s0    "},
    {9,  "s1    "},
    {10, "a0    "},
    {11, "a1    "},
    {12, "a2    "},
    {13, "a3    "},
    {14, "a4    "},
    {15, "a5    "},
    {16, "a6    "},
    {17, "a7    "},
    {18, "s2    "},
    {19, "s3    "},
    {20, "s4    "},
    {21, "s5    "},
    {22, "s6    "},
    {23, "s7    "},
    {24, "s8    "},
    {25, "s9    "},
    {26, "s10   "},
    {27, "s11   "},
    {28, "t3    "},
    {29, "t4    "},
    {30, "t5    "},
    {31, "t6    "}
};

// Parse symtab

#define ST_BIND(info)          (get_sym_bind((info) >> 4))
#define ST_TYPE(info)          (get_sym_types((info) & 0xf))
#define ST_VISIBILITY(o)       (get_sym_vis((o)&0x3))

map <int, string> sym_bind = {
    {0, "LOCAL"},
    {1, "GLOBAL"},
    {2, "WEAK"},
    {10, "LOOS"},
    {12, "HIOS"},
    {13, "LOPROC"},
    {15, "HIPROS"}
};

string get_sym_bind (int index) {
    if (sym_bind.find(index) == sym_bind.end()) index = 0;
    return sym_bind[index];
}

map <int, string> sym_types = {
    {0, "NOTYPE"},
    {1, "OBJECT"},
    {2, "FUNC"},
    {3, "SECTION"},
    {4, "FILE"},
    {5, "COMMON"},
    {6, "TLS"},
    {10, "LOOS"},
    {12, "HIOS"},
    {13, "LOPROC"},
    {15, "HIPROS"}
};

string get_sym_types (int index) {
    if (sym_types.find(index) == sym_types.end()) index = 0;
    return sym_types[index];
}

map <int, string> sym_vis = {
    {0, "DEFAULT"},
    {1, "INTERNAL"},
    {2, "HIDDEN"},
    {3, "PROTECTED"},
    {4, "EXPORTED"},
    {5, "SINGLETON"},
    {6, "ELIMINATE"}
};

string get_sym_vis (int index) {
    if (sym_vis.find(index) == sym_vis.end()) index = 0;
    return sym_vis[index];
}

map <int, string> sym_index = {
    {0, "UNDEF"},
    {0xff00, "LORESERVE"},
    {0xff00, "LOPROC"},
    {0xff00, "BEFORE"},
    {0xff01, "AFTER"},
    {0xff02, "AMD64_LCOMMON"},
    {0xff1f, "HIPROC"},
    {0xff20, "LOOS"},
    {0xff3f, "LOSUNW"},
    {0xff3f, "SUNW_IGNORE"},
    {0xff3f, "HISUNW"},
    {0xff3f, "HIOS"},
    {0xfff1, "ABS"},
    {0xfff2, "COMMON"},
    {0xffff, "XINDEX"},
    {0xffff, "HIRESERVE"}
};

string get_sym_index (int index) {
    if (sym_index.find(index) != sym_index.end()) 
        return sym_index[index];
    return to_string(index);
}


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
        cout << command[{funct7, funct3}] << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] <<'\n';
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
        cout << command[{opcode, funct3}] << reg[rd] << ", " << reg[rs1] << ", " << cnst << '\n';
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
        cout << command[{opcode, funct3}] << reg[rs1] << ", " << reg[rs2] << ", " << cnst << '\n';
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
        cout << command[{opcode, funct3}] << reg[rs1] << ", " << reg[rs2] << ", " << cnst << '\n';
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
        cout << command[{opcode}] << reg[rd] << ", " << cnst << '\n';
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
        cout << command[{opcode}] << reg[rd] << ", " << cnst << '\n';
    };
};