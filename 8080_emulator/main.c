#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


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
        case 0x09: printf("DAD B"); break;
        case 0x0A: printf("LDAX B"); break;
        case 0x0B: printf("DCX B"); break;
        case 0x0C: printf("INR C"); break;
        case 0x0D: printf("DCR C"); break;
        case 0x0E: printf("MVI C,#$%02x", code[1]); opbytes=2; break;
        case 0x0F: printf("RRC"); break;
        case 0x10: printf("NOP"); break;
        case 0x11: printf("LXI    D,#$%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x12: printf("STAX D"); break;
        case 0x13: printf("INX D"); break;
        case 0x14: printf("INR D"); break;
        case 0x15: printf("DCR D"); break;
        case 0x16: printf("MVI D,#$%02x", code[1]); opbytes=2; break;
        case 0x17: printf("RAL"); break;
        case 0x18: printf("NOP"); break;
        case 0x19: printf("DAD D"); break;
        case 0x1A: printf("LDAX D"); break;
        /* ........ */
        case 0x21: printf("LXI    H,#$%02x%02x", code[2], code[1]); opbytes=3; break;
        /* ........ */
        case 0x31: printf("LXI    SP,$%02x%02x",code[2],code[1]); opbytes = 3; break;
        /* ........ */
        case 0x3e: printf("MVI    A,#0x%02x", code[1]); opbytes = 2; break;
        /* ........ */
        case 0xc3: printf("JMP    $%02x%02x",code[2],code[1]); opbytes = 3; break;
        /* ........ */
        case 0xCD: printf("CALL   $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0x77: printf("MOV   M, A");break;
        case 0x80: printf("ADD   B"); break;
        case 0x81: printf("ADD   C"); break;
        case 0x82: printf("ADD   D"); break;
        case 0x83: printf("ADD   E"); break;
        case 0x84: printf("ADD   H"); break;
        case 0x85: printf("ADD   L"); break;
        case 0x86: printf("ADD   M"); break;
        case 0x87: printf("ADD   B"); break;
        case 0xC6: printf("ADI   D8,#0x%02x", code[1]); break;
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

int parity(uint8_t num) {
    int count = 0;
    while(num) 
    {
        if(num & 1) 
        {
            count+=1;
        }
        num >>= 1;
    }
    return count % 2;
}

//sets neccessary flags when called depending on the instruction
void setFlags(uint16_t answer, State8080* state) {
    state->cc.z = ((answer & 0xff) == 0);    
    state->cc.s = ((answer & 0x80) != 0);    
    state->cc.cy = (answer > 0xff);    
    state->cc.p = parity(answer&0xff);  
}

unsigned char* fetch(State8080* state) {
    unsigned char *opcode = &state->memory[state->pc];
    return opcode;
}

void Emulate8080Op(unsigned char *opcode, State8080* state)    
{    
    // printf("PC: %x\n", state->pc);
    //printf("opcode: %x\n", *opcode); 
    Disassemble8080Op(state->memory, state->pc);
    uint16_t answer;
    switch(*opcode)    
    {    
        case 0x00: break;                   //NOP is easy!    
        case 0x01:                          //LXI   B,word    
            state->c = opcode[1];    
            state->b = opcode[2];    
            state->pc += 2;                  //Advance 2 more bytes    
            break;    
        /*....*/
        case 0x06:
            state->b = opcode[1];
            state->pc += 1;
            break;
        /*....*/
        case 0x11:
            state->e = opcode[1];    
            state->d = opcode[2];   
            state->pc += 2;                  //Advance 2 more bytes    
            break;
        case 0x1A:
            state->a = state->memory[(uint16_t)(state->d << 8 | state->e)]; 
            break;
        case 0x21:
            state->l = opcode[1];    
            state->h = opcode[2];    
            state->pc += 2;                  //Advance 2 more bytes    
            break;
        case 0x31:
            state->sp = ((opcode[2] << 8) | opcode[1]);
            state->pc += 2;
        /*....*/
        case 0x41: state->b = state->c; break;    //MOV B,C    
        case 0x42: state->b = state->d; break;    //MOV B,D    
        case 0x43: state->b = state->e; break;    //MOV B,E    
        case 0xC3:
            state->pc = ((opcode[2] << 8) | opcode[1]) - 1; break;
        case 0xCD:
            // (SP-1)<-PC.hi
            // (SP-2)<-PC.lo
            state->memory[(state->sp)-1] = state->pc >> 8 & 0xFF;
            state->memory[(state->sp)-2] = state->pc & 0xFF;

            state->sp -= 2;
            state->pc = ((opcode[2] << 8) | opcode[1]) - 1;
            break;
        case 0x77:
            state->memory[(uint16_t)(state->h << 8 | state->l)] = state->a; break; //MOV M, A
        case 0x80:
            //ADD B, A + B
            answer = (uint16_t)state->a + (uint16_t)state->b;
            setFlags(answer, state);        
            state->a = answer & 0xff;
            break;    
        case 0x81:
            //ADD C, A + C
            answer = (uint16_t)state->a + (uint16_t)state->c;   
            setFlags(answer, state); 
            state->a = answer & 0xff;    
            break;
        case 0x82:
            //ADD D, A + D
            answer = (uint16_t)state->a + (uint16_t)state->d;    
            setFlags(answer, state);    
            state->a = answer & 0xff;    
            break;
        case 0x83:
            //ADD E, A + E
            answer = (uint16_t)state->a + (uint16_t)state->e;    
            setFlags(answer, state);  
            state->a = answer & 0xff;    
            break;
        case 0x84:
            //ADD H, A + H
            answer = (uint16_t)state->a + (uint16_t)state->h;    
            setFlags(answer, state);   
            state->a = answer & 0xff;    
            break;
        case 0x85:
            //ADD L, A + L
            answer = (uint16_t) state->a + (uint16_t) state->l;    
            setFlags(answer, state);   
            state->a = answer & 0xff;
            break; 
        case 0x86:
            //ADD M, A + (HL)
            //Adding from memory location of HL
            answer = (uint16_t) state->a + state->memory[(uint16_t)(state->h << 8 | state->l)];    
            setFlags(answer, state);  
            state->a = answer & 0xff;
            break;   
        case 0xC6:
            //ADI D8, A + Byte
            //Adding byte
            answer = (uint16_t) state->a + (uint16_t)opcode[1];    
            setFlags(answer, state);  
            state->a = answer & 0xff;
            state->pc+=1;
        default: 
            printf("PC: %x\n", state->pc);
            printf("OPCODE: %x\n", *opcode); 
            UnimplementedInstruction(state);      //quits program for unimplemented opcode for now
            break;
    }    
    state->pc+=1;    
}  

State8080* Initialize() 
{
    FILE *f= fopen("roms/spaceinvaders/invaders.concatenated", "rb");    
    if (f==NULL)    
    {    
        printf("error: Couldn't open %s\n", "8080_emulator/roms/spaceinvaders/invaders.concatenated");    
        return NULL;    
    }    
    State8080* state = (State8080*)calloc(1,sizeof(State8080)); 
    state->memory = (uint8_t*)malloc(0x6000);
    //Get the file size and read it into a memory buffer    
    fseek(f, 0L, SEEK_END);    
    int fsize = ftell(f);    
    fseek(f, 0L, SEEK_SET);       
    fread(state->memory, fsize, 1, f);    
    fclose(f);
    return state;
}

int main ()    
{    
    if(Initialize()==NULL) {
        return 1;
    }
    State8080* s = Initialize();
    while (1)    
    {   
        Emulate8080Op(fetch(s), s);
    }    



    return 0;    
}  