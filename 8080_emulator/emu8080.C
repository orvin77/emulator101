#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct ConditionCodes {    
    uint8_t    z:1;    
    uint8_t    s:1;    
    uint8_t    p:1;    
    uint8_t    cy:1;    
    uint8_t    ac:1;    
    uint8_t    pad:3;    
} ConditionCodes;    

typedef struct State8080 {    
    uint8_t    a;    
    uint8_t    b;    
    uint8_t    c;    
    uint8_t    d;    
    uint8_t    e;    
    uint8_t    h;    
    uint8_t    l;    
    uint16_t    sp;    
    uint16_t    pc;    
    uint8_t     *memory;    
    struct      ConditionCodes      cc;    
    uint8_t     int_enable;    
} State8080;    


/*    
*codebuffer is a valid pointer to 8080 assembly code    
pc is the current offset into the code    

returns the number of bytes of the op    
*/    

int Disassemble8080Op(unsigned char *codebuffer, int pc)    
{    
    unsigned char *code = &codebuffer[pc];    
    int opbytes = 1;    
    printf ("%04x ", pc);    
    switch (*code)    
    {    
        case 0x00: printf("NOP"); break;    
        case 0x01: printf("LXI    B,#$%02x%02x", code[2], code[1]); opbytes=3; break;    
        case 0x02: printf("STAX   B"); break;    
        case 0x03: printf("INX    B"); break;    
        case 0x04: printf("INR    B"); break;    
        case 0x05: printf("DCR    B"); break;    
        case 0x06: printf("MVI    B,#$%02x", code[1]); opbytes=2; break;    
        case 0x07: printf("RLC"); break;    
        case 0x08: printf("NOP"); break;    
        /* ........ */    
        case 0x3e: printf("MVI    A,#0x%02x", code[1]); opbytes = 2; break;    
        /* ........ */    
        case 0xc3: printf("JMP    $%02x%02x",code[2],code[1]); opbytes = 3; break;    
        /* ........ */    
    }

    printf("\n");

    return opbytes;    
}    

void UnimplementedInstruction(State8080* state)    
{    
    //pc will have advanced one, so undo that    
    printf ("Error: Unimplemented instruction\n");    
    exit(1);    
}    

 void Emulate8080Op(State8080* state)    
{    
    unsigned char *opcode = &state->memory[state->pc]; 
    printf("PC: %u\n", state->pc);
    printf("opcode: %x\n", *opcode); 
    Disassemble8080Op(state->memory, state->pc);
    switch(*opcode)    
    {    
        case 0x00: break;                   //NOP is easy!    
        case 0x01:                          //LXI   B,word    
            state->c = opcode[1];    
            state->b = opcode[2];    
            state->pc += 2;                  //Advance 2 more bytes    
            break;    
        /*....*/    
        case 0x41: state->b = state->c; break;    //MOV B,C    
        case 0x42: state->b = state->d; break;    //MOV B,D    
        case 0x43: state->b = state->e; break;    //MOV B,E    
        default: 
            UnimplementedInstruction(state);      //quits program for unimplemented opcode for now
            break;
    }    
    state->pc+=1;    
}  

int main ()    
{    
    FILE *f= fopen("roms/spaceinvaders/invaders.concatenated", "rb");    
    if (f==NULL)    
    {    
        printf("error: Couldn't open %s\n", "roms/spaceinvaders/invaders.concatenated");    
        return 1;    
    }    
    State8080* state = (State8080*)calloc(1,sizeof(State8080)); 
    state->memory = (uint8_t*)malloc(0x6000);
    //Get the file size and read it into a memory buffer    
    fseek(f, 0L, SEEK_END);    
    int fsize = ftell(f);    
    fseek(f, 0L, SEEK_SET);       

    fread(state->memory, fsize, 1, f);    
    fclose(f);    

    int pc = 0; 
    

    while (true)    
    {    
        Emulate8080Op(state);
        //pc += Disassemble8080Op(buffer, pc);    
    }    



    return 0;    
}  