/**
 * Author: Caleb Tillman
 * 
 * This program disassembles a series of MOV instructions of the type that moves r/m to a register.
*/

#define USAGE "usage: decode <file>"
#define BUFFER_SIZE 100

#define IS_MOV(INS)         ((INS >> 2) == 0x22)
#define MOV_DIR(INS)        ((0x2 & INS) >> 1)
#define MOV_WORD(INS)       (0x1 & INS)
#define MOV_DIR_DEST        1
#define MOV_MODE(OPS)       (OPS >> 6)
#define REG_TO_REG_MODE     0x3
#define MOV_REG(OPS)        ((0x38 & OPS) >> 3)
#define MOV_RM(OPS)         (0x7 & OPS)

#include <stdio.h>
#include <assert.h>

typedef unsigned char byte;
typedef unsigned char bool;
typedef unsigned int u32;

byte buffer[BUFFER_SIZE];
char *reg_tbl[16] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh", 
                     "ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};


int main(int argc, char *argsv[]) {
    if (argc < 2) {
        fprintf(stderr, "%s\n", USAGE);
        return 1;
    }

    FILE *file = fopen(argsv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", argsv[1]);
        return 1;
    }

    printf("bits 16\n\n");

    size_t read = fread(buffer, sizeof(byte), BUFFER_SIZE, file);

    while (read != 0) {
        int c = 0;
        while (c < read) {
            byte ins = buffer[c];
            assert (IS_MOV(ins));
            
            byte ops = buffer[c + 1];
            assert (MOV_MODE(ops) == REG_TO_REG_MODE);


            u32 mov_reg = MOV_REG(ops); // if mov_dir = 0 then mov_reg is the source
            u32 mov_rm = MOV_RM(ops);   // if mov_dir = 0 then mov_rm is the destination
            bool mov_dir = MOV_DIR(ins); 
            bool mov_word = MOV_WORD(ins); // mov_word = 0 -> first half of reg_tbl

            byte reg_index[2] = { mov_rm, mov_reg };
            byte reg_tbl_index = mov_word << 3;

            printf("mov %s, %s\n", reg_tbl[reg_tbl_index + reg_index[mov_dir]], reg_tbl[reg_tbl_index + reg_index[!mov_dir]]);

            c += 2;
        }
        read = fread(buffer, sizeof(byte), BUFFER_SIZE, file);
    }

    fclose(file);
}