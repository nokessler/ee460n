/*
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Noah Kessler
    UTEID 1: njk576
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
*/

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
  int k;

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;
  NEXT_LATCHES = CURRENT_LATCHES;

  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

int signExtend(int num, int signbit) {
   if (num && (1 << signbit)) {
      num = num << (8*sizeof(int)-1-signbit);
      num = num >> (8*sizeof(int)-1-signbit);
   }
   return(num);
}

void setConditionCodes(int result) {
   if (result < 0) {
      NEXT_LATCHES.N = 1;
      NEXT_LATCHES.Z = 0;
      NEXT_LATCHES.P = 0;
   } else if (result > 0) {
      NEXT_LATCHES.N = 0;
      NEXT_LATCHES.Z = 0;
      NEXT_LATCHES.P = 1;
   } else {
      NEXT_LATCHES.N = 0;
      NEXT_LATCHES.Z = 1;
      NEXT_LATCHES.P = 0;
   }
}

void ADD(instruction) {
   printf("executing ADD\n");
   int dr = (instruction >> 9) & 0x07;
   int sr1 = (instruction >> 6) & 0x07;
   int result = Low16bits(CURRENT_LATCHES.REGS[sr1]);
   if (instruction & 0x20) {
      result += signExtend((instruction & 0x1F), 4);
   } else {
      int sr2 = instruction & 0x07;
      result += Low16bits(CURRENT_LATCHES.REGS[sr2]);
   }
   setConditionCodes(signExtend(Low16bits(result), 15));
   NEXT_LATCHES.REGS[dr] = Low16bits(result);
}

void AND(instruction) {
   printf("executing AND\n");
   int dr = (instruction >> 9) & 0x07;
   int sr1 = (instruction >> 6) & 0x07;
   int result = Low16bits(CURRENT_LATCHES.REGS[sr1]);
   if (instruction & 0x20) result &= signExtend((instruction & 0x1F), 4);
   else {
      int sr2 = instruction & 0x07;
      result &= Low16bits(CURRENT_LATCHES.REGS[sr2]);
   }
   setConditionCodes(signExtend(Low16bits(result), 15));
   NEXT_LATCHES.REGS[dr] = Low16bits(result);
}

void takeBranch(int instruction) {
   int pc = Low16bits(NEXT_LATCHES.PC);
   int offset = signExtend((instruction & 0x01FF), 8);
   offset = offset << 1;
   pc += offset;
   NEXT_LATCHES.PC = Low16bits(pc);
   printf("branching to %x\n", Low16bits(pc));
}

void BR(instruction) {
   printf("executing BR\n");
   if(instruction & 0x0200) {
      if (CURRENT_LATCHES.P) {
         takeBranch(instruction);
         return;
      }
   }
   if (instruction & 0x0400) {
      if (CURRENT_LATCHES.Z) {
         takeBranch(instruction);
         return;
      }
   }
   if (instruction & 0x0800) {
      if (CURRENT_LATCHES.N) {
         takeBranch(instruction);
         return;
      }
   }
}

void JMP(instruction) {
   printf("executing JMP\n");
   int location = signExtend(Low16bits(CURRENT_LATCHES.REGS[(instruction >> 6) & 0x07]), 15);
   NEXT_LATCHES.PC = Low16bits(location);
}

void JSR(instruction) {
   printf("executing JSR\n");
   NEXT_LATCHES.REGS[7] = Low16bits(NEXT_LATCHES.PC);
   if (instruction & 0x0800) {
      int offset = signExtend((instruction & 0x07FF), 10);
      offset = offset << 1;
      NEXT_LATCHES.PC += offset;
   } else {
      NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.REGS[(instruction >> 6) & 0x07]);
   }
}

void LDB(instruction) {
   printf("executing LDB\n");
   int dr = (instruction >> 9) & 0x07;
   int br = (instruction >> 6) & 0x07;
   int offset = signExtend((instruction & 0x03F), 5);
   int base = Low16bits(CURRENT_LATCHES.REGS[br]);
   base += offset;
   int result = MEMORY[base >> 1][base & 1] & 0x00FF;
   setConditionCodes(signExtend(result, 7));
   NEXT_LATCHES.REGS[dr] = result & 0x00FF;
}

void LDW(instruction) {
   printf("executing LDW\n");
   int dr = (instruction >> 9) & 0x07;
   int br = (instruction >> 6) & 0x07;
   int offset = signExtend((instruction & 0x03F), 5);
   offset = offset << 1;
   int base = Low16bits(CURRENT_LATCHES.REGS[br]);
   base += offset;
   int result = MEMORY[base >> 1][0] & 0x00FF;
   result |= (MEMORY[base >> 1][1] & 0x00FF) << 8;
   setConditionCodes(signExtend(result,15));
   NEXT_LATCHES.REGS[dr] = Low16bits(result);
}

void LEA(instruction) {
   printf("executing LEA\n");
   int result = signExtend((instruction & 0x1FF), 8);
   result = result << 1;
   result += NEXT_LATCHES.PC;
   int dr = ((instruction >> 9) & 0x07);
   NEXT_LATCHES.REGS[dr] = result;
}

void SHF(instruction) {
   printf("executing SHF\n");
   int dr = ((instruction >> 9) & 0x07);
   int sr = ((instruction >> 6) & 0x07);
   int result = Low16bits(CURRENT_LATCHES.REGS[sr]);
   switch ((instruction >> 4) & 0x03) {
      case 0: result = result << (instruction & 0x0F); break;
      case 1: result = result >> (instruction & 0x0F); break;
      case 3: result = signExtend(Low16bits(result), 15); result = result >> (instruction & 0x0F); break;
   }
   setConditionCodes(signExtend(Low16bits(result), 15));
   NEXT_LATCHES.REGS[dr] = Low16bits(result);
}

void STB(instruction) {
   printf("executing STB\n");
   int sr = (instruction >> 9) & 0x07;
   int br = (instruction >> 6) & 0x07;
   int offset = signExtend((instruction & 0x03F), 5);
   int base = Low16bits(CURRENT_LATCHES.REGS[br]);
   base += offset;
   int store = CURRENT_LATCHES.REGS[sr] & 0x00FF;
   MEMORY[base >> 1][base & 1] = store;
}

void STW(instruction) {
   printf("executing STW\n");
   int sr = (instruction >> 9) & 0x07;
   int br = (instruction >> 6) & 0x07;
   int offset = signExtend((instruction & 0x03F), 5);
   offset = offset << 1;
   int base = Low16bits(CURRENT_LATCHES.REGS[br]);
   base += offset;
   int storelow = CURRENT_LATCHES.REGS[sr] & 0x00FF;
   int storehigh = (CURRENT_LATCHES.REGS[sr] >> 8) & 0x00FF;
   MEMORY[base >> 1][0] = storelow;
   MEMORY[base >> 1][1] = storehigh;
}

void TRAP(instruction) {
   printf("executing TRAP\n");
   NEXT_LATCHES.REGS[7] = Low16bits(NEXT_LATCHES.PC);
   int vector = Low16bits((instruction & 0x00FF) << 1);
   int address = MEMORY[vector >> 1][0] & 0x00FF;
   address |= (MEMORY[vector >> 1][1] & 0x00FF) << 8;
   NEXT_LATCHES.PC = address;
}

void XOR(instruction) {
   printf("executing XOR\n");
   int dr = (instruction >> 9) & 0x07;
   int sr1 = (instruction >> 6) & 0x07;
   int result = Low16bits(CURRENT_LATCHES.REGS[sr1]);
   if (instruction & 0x20) result ^= signExtend((instruction & 0x1F), 4);
   else {
      int sr2 = instruction & 0x07;
      result ^= Low16bits(CURRENT_LATCHES.REGS[sr2]);
   }
   setConditionCodes(signExtend(Low16bits(result), 15));
   NEXT_LATCHES.REGS[dr] = Low16bits(result);
}

void process_instruction(){
  /*  function: process_instruction
   *
   *    Process one instruction at a time
   *       -Fetch one instruction
   *       -Decode
   *       -Execute
   *       -Update NEXT_LATCHES
   */
   int instruction = MEMORY[CURRENT_LATCHES.PC >> 1][0] & 0x00FF;
   instruction |= (MEMORY[CURRENT_LATCHES.PC >> 1][1] & 0x00FF) << 8;
   instruction = Low16bits(instruction);
   int opcode = (instruction >> 12) & 0x0F;
   NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
   switch (opcode) {
      case 0x01: ADD(instruction); break;
      case 0x05: AND(instruction); break;
      case 0x00: BR(instruction); break;
      case 0x0C: JMP(instruction); break;
      case 0x04: JSR(instruction); break;
      case 0x02: LDB(instruction); break;
      case 0x06: LDW(instruction); break;
      case 0x0E: LEA(instruction); break;
      case 0x0D: SHF(instruction); break;
      case 0x03: STB(instruction); break;
      case 0x07: STW(instruction); break;
      case 0x0F: TRAP(instruction); break;
      case 0x09: XOR(instruction); break;
   }
}
