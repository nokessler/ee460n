/*
        Name: Noah Kessler
        UTEID: njk576
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

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
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {
    IRD,
    COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX1, SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
/* MODIFY: you have to add all your new control signals */
    SPMUX,
    EXCMUX,
    IEMUX,
    LD_TEMP,
    LD_PSR,
    LD_PRIV,
    LD_SaveSSP,
    LD_SaveUSP,
    LD_EX,
    LD_EXCV,
    LD_Vector,
    LD_INTV,
    ALIGN,
    GATE_TEMP,
    GATE_PSR,
    GATE_PCM2,
    GATE_SPMUX,
    GATE_SSP,
    GATE_USP,
    GATE_Vector,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); }
int GetLSHF1(int *x)         { return(x[LSHF1]); }
int GetSPMUX(int *x)         { return(x[SPMUX]); }
int GetEXCMUX(int *x)        { return(x[EXCMUX]); }
int GetIEMUX(int *x)         { return(x[IEMUX]); }
int GetLD_TEMP(int *x)       { return(x[LD_TEMP]); }
int GetLD_PSR(int *x)        { return(x[LD_PSR]); }
int GetLD_PRIV(int *x)       { return(x[LD_PRIV]); }
int GetLD_SaveSSP(int *x)    { return(x[LD_SaveSSP]); }
int GetLD_SaveUSP(int *x)    { return(x[LD_SaveUSP]); }
int GetLD_EX(int *x)         { return(x[LD_EX]); }
int GetLD_EXCV(int *x)       { return(x[LD_EXCV]); }
int GetLD_Vector(int *x)     { return(x[LD_Vector]); }
int GetLD_INTV(int *x)       { return(x[LD_INTV]); }
int GetALIGN(int *x)         { return(x[ALIGN]); }
int GetGATE_TEMP(int *x)     { return(x[GATE_TEMP]); }
int GetGATE_PSR(int *x)      { return(x[GATE_PSR]); }
int GetGATE_PCM2(int *x)     { return(x[GATE_PCM2]); }
int GetGATE_SPMUX(int *x)    { return(x[GATE_SPMUX]); }
int GetGATE_SSP(int *x)      { return(x[GATE_SSP]); }
int GetGATE_USP(int *x)      { return(x[GATE_USP]); }
int GetGATE_Vector(int *x)   { return(x[GATE_Vector]); }

/* MODIFY: you can add more Get functions for your new control signals */

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
   There are two write enable signals, one for each byte. WE0 is used for
   the least significant byte of a word. WE1 is used for the most significant
   byte of a word. */

#define WORDS_IN_MEM    0x08000
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you don’t want the memory system to assert it
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microinstruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* Initial value of system stack pointer */
/* MODIFY: You may add system latches that are required by your implementation */
int PSR;
int TEMP;
int VECTOR;
int USP;
int EX;
int INT;

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

  eval_micro_sequencer();
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;

  if (CYCLE_COUNT == 299) {
     NEXT_LATCHES.INT = 1;
     NEXT_LATCHES.INTV = 0x01;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
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
/* Purpose   : Simulate the LC-3b until HALTed.                 */
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

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
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
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
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
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
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

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *program_filename, int num_prog_files) {
    int i;
    init_control_store(ucode_filename);

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(program_filename);
	while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */
    CURRENT_LATCHES.INT = 0;
    CURRENT_LATCHES.PSR = 0x8002;

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
    if (argc < 3) {
	printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argv[2], argc - 2);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated
   with a "MODIFY:" comment.

   Do not modify the rdump and mdump functions.

   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

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

/*
* Evaluate the address of the next state according to the
* micro sequencer logic. Latch the next microinstruction.
*/
void eval_micro_sequencer() {
   if(GetIRD(CURRENT_LATCHES.MICROINSTRUCTION)) {
      printf("Decode\n");
      int inst = CURRENT_LATCHES.IR;
      NEXT_LATCHES.BEN = ((inst & 0x0800) && CURRENT_LATCHES.N) || ((inst & 0x0400) && CURRENT_LATCHES.Z) || ((inst & 0x0200) && CURRENT_LATCHES.P);
      NEXT_LATCHES.STATE_NUMBER = (inst >> 12) & 0x0F;
   } else {
      int j = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
      int cond = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION);
      NEXT_LATCHES.STATE_NUMBER = j;
      if (cond == 7) {
         if (CURRENT_LATCHES.EX & 1) {
            NEXT_LATCHES.STATE_NUMBER |= 0x20;
         }
      } else if (cond == 6) {
         if (CURRENT_LATCHES.EX & 1) {
            NEXT_LATCHES.STATE_NUMBER |= 0x10;
         }
      } else if (cond == 5) {
         if (CURRENT_LATCHES.PSR & 0x0080) {
            NEXT_LATCHES.STATE_NUMBER |= 0x10;
         }
      } else if (cond == 4) {
         if (CURRENT_LATCHES.INT) {
            NEXT_LATCHES.STATE_NUMBER |= 0x08;
            NEXT_LATCHES.INT = 0;
         }
      } else if (cond == 3) {
         if (CURRENT_LATCHES.IR & 0x0800) {
            NEXT_LATCHES.STATE_NUMBER |= 0x01;
         }
      } else if (cond == 2) {
         if (CURRENT_LATCHES.BEN) {
            NEXT_LATCHES.STATE_NUMBER |= 0x04;
         }
      } else if (cond == 1) {
         if (CURRENT_LATCHES.READY) {
            NEXT_LATCHES.STATE_NUMBER |= 0x02;
         }
      }
   }
   int i;
   for (i = 0; i < CONTROL_STORE_BITS; i++) {
      NEXT_LATCHES.MICROINSTRUCTION[i] = CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER][i];
   }
}

/*
* This function emulates memory and the WE logic.
* Keep track of which cycle of MEMEN we are dealing with.
* If fourth, we need to latch Ready bit at the end of
* cycle to prepare microsequencer for the fifth cycle.
*/
int MEMEN = 0;
void cycle_memory() {
   if (!GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION)) return;
   MEMEN ++;
   if (MEMEN == 4) {
      NEXT_LATCHES.READY = 1;
      printf("MEM ready\n");
   }
   if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION)) {
      if (CURRENT_LATCHES.READY) {
         int address = CURRENT_LATCHES.MAR >> 1;
         int byte = CURRENT_LATCHES.MAR & 0x01;
         printf("%x\n", GetR_W(CURRENT_LATCHES.MICROINSTRUCTION));
         if (GetR_W(CURRENT_LATCHES.MICROINSTRUCTION)) {
            printf("write\n");
            if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION)) {
               printf("storing word in: %x\n", address << 1);
               MEMORY[address][0] = CURRENT_LATCHES.MDR & 0x00FF;
               MEMORY[address][1] = (CURRENT_LATCHES.MDR >> 8) & 0x00FF;
            } else {
               printf("storing byte in: %x\n", (address << 1)+byte);
               MEMORY[address][byte] = CURRENT_LATCHES.MDR & 0x00FF;
            }
         }
         NEXT_LATCHES.READY = 0;
         MEMEN = 0;
      }
   }
}

/*
* Datapath routine emulating operations before driving the bus.
* Evaluate the input of tristate drivers
*      Gate_MARMUX,
*		 Gate_PC,
*		 Gate_ALU,
*		 Gate_SHF,
*		 Gate_MDR,
*      Gate_TEMP,
*      Gate_PSR,
*		 Gate_PCM2,
*		 Gate_SPMUX,
*		 Gate_SSP,
*		 Gate_USP,
*      Gate_Vector.
*/
int gate;
void eval_bus_drivers() {
   gate = 0;
   if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
      gate = 0x0001;
      printf("driving GATE_MARMUX\n");
   }
   if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION)) {
      gate = 0x0002;
      printf("driving GATE_PC\n");
   }
   if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION)) {
      gate = 0x0004;
      printf("driving GATE_ALU\n");
   }
   if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION)) {
      gate = 0x0008;
      printf("driving GATE_SHF\n");
   }
   if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION)) {
      gate = 0x0010;
      printf("driving GATE_MDR\n");
   }
   if (GetGATE_TEMP(CURRENT_LATCHES.MICROINSTRUCTION)) {
      gate = 0x0020;
      printf("driving GATE_TEMP\n");
   }
   if (GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION)) {
      gate = 0x0040;
      printf("driving GATE_PSR\n");
   }
   if (GetGATE_PCM2(CURRENT_LATCHES.MICROINSTRUCTION)) {
      gate = 0x0080;
      printf("driving GATE_PCM2\n");
   }
   if (GetGATE_SPMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
      gate = 0x0100;
      printf("driving GATE_SPMUX\n");
   }
   if (GetGATE_SSP(CURRENT_LATCHES.MICROINSTRUCTION)) {
      gate = 0x0200;
      printf("driving GATE_SSP\n");
   }
   if (GetGATE_USP(CURRENT_LATCHES.MICROINSTRUCTION)) {
      gate = 0x0400;
      printf("driving GATE_USP\n");
   }
   if (GetGATE_Vector(CURRENT_LATCHES.MICROINSTRUCTION)) {
      gate = 0x0800;
      printf("driving GATE_Vector\n");
   }
}

/*
* Datapath routine for driving the bus from one of the 5 possible
* tristate drivers.
*/
void drive_bus() {
   if (gate == 0) BUS = 0;
   else if (gate == 0x0001) {
      if (GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
         printf("optput IR\n");
         BUS = (CURRENT_LATCHES.IR & 0x00FF) << 1;
      } else {
         printf("adder\n");
         int op1, op2;
         if (GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
            if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
               op1 = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 6) & 0x07];
               printf("source 1 = 8:6\n");
            }
            else {
               op1 = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 9) & 0x07];
               printf("source 1 = 11:9\n");
            }
         } else {
            op1 = CURRENT_LATCHES.PC;
            printf("source 1 = PC\n");
         }
         if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            op2 = signExtend(CURRENT_LATCHES.IR & 0x003F, 5);
            printf("source 2 = offset6\n");
         }
         else if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2) {
            op2 = signExtend(CURRENT_LATCHES.IR & 0x01FF, 8);
            printf("source 2 = offset9\n");
         }
         else if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3) {
            op2 = signExtend(CURRENT_LATCHES.IR & 0x07FF, 10);
            printf("source 2 = offset11\n");
         }
         else {
            op2 = 0;
            printf("source 2 = 0\n");
         }
         if (GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION)) {
            op2 = op2 << 1;
            printf("LSHF source 2 1 bit\n");
         }
         BUS = op1 + op2;
      }
   } else if (gate == 0x0002) {
      BUS = CURRENT_LATCHES.PC;
   } else if (gate == 0x0004) {
      int sr1, sr2;
      if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
         sr1 = (CURRENT_LATCHES.IR >> 6) & 0x07;
         printf("source 1 = 8:6\n");
      }
      else {
         sr1 = (CURRENT_LATCHES.IR >> 9) & 0x07;
         printf("source 1 = 11:9\n");
      }
      if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 3) {
         BUS = Low16bits(CURRENT_LATCHES.REGS[sr1]);
         printf("pass\n");
      } else if (CURRENT_LATCHES.IR & 0x0020) {
         printf("immediate\n");
         sr2 = signExtend(CURRENT_LATCHES.IR & 0x001F, 4);
         if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
            printf("add\n");
            BUS = Low16bits(CURRENT_LATCHES.REGS[sr1] + sr2);
         } else if(GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            printf("and\n");
            BUS = Low16bits(CURRENT_LATCHES.REGS[sr1] & sr2);
         } else if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 2) {
            printf("xor\n");
            BUS = Low16bits(CURRENT_LATCHES.REGS[sr1] ^ sr2);
         }
      } else {
         printf("source 2 = register\n");
         sr2 = (CURRENT_LATCHES.IR & 0x07);
         if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
            printf("add\n");
            BUS = Low16bits(CURRENT_LATCHES.REGS[sr1] + CURRENT_LATCHES.REGS[sr2]);
         } else if(GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            printf("and\n");
            BUS = Low16bits(CURRENT_LATCHES.REGS[sr1] & CURRENT_LATCHES.REGS[sr2]);
         } else if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 2) {
            printf("xor\n");
            BUS = Low16bits(CURRENT_LATCHES.REGS[sr1] ^ CURRENT_LATCHES.REGS[sr2]);
         }
      }
   } else if (gate == 0x0008) {
      int sr1;
      if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)) sr1 = (CURRENT_LATCHES.IR >> 6) & 0x07;
      else sr1 = (CURRENT_LATCHES.IR >> 9) & 0x07;
      int shift = CURRENT_LATCHES.IR & 0x0F;
      if (CURRENT_LATCHES.IR & 0x0010) {
         if (CURRENT_LATCHES.IR & 0x0020) {
            BUS = Low16bits(signExtend(CURRENT_LATCHES.REGS[sr1], 15) >> shift);
         } else BUS = Low16bits(CURRENT_LATCHES.REGS[sr1] >> shift);
      } else BUS = Low16bits(CURRENT_LATCHES.REGS[sr1] << shift);
   } else if (gate == 0x0010) {
      if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION)) BUS = Low16bits(CURRENT_LATCHES.MDR);
      else {
         BUS = Low16bits(signExtend(CURRENT_LATCHES.MDR & 0x00FF, 7));
      }
   } else if (gate == 0x0020) {
      BUS = CURRENT_LATCHES.TEMP;
   } else if (gate == 0x0040) {
      BUS = CURRENT_LATCHES.PSR;
   } else if (gate == 0x0080) {
      BUS = (CURRENT_LATCHES.PC - 2);
   } else if (gate == 0x0100) {
      if (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0x10) {
         if (GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            int n = signExtend(Low16bits(CURRENT_LATCHES.REGS[6]), 15) + 2;
            BUS = Low16bits(n);
         } else {
            int n = signExtend(Low16bits(CURRENT_LATCHES.REGS[6]), 15) - 2;
            BUS = Low16bits(n);
         }
      }
   } else if (gate == 0x0200) {
      BUS = CURRENT_LATCHES.SSP;
   } else if (gate == 0x0400) {
      BUS = CURRENT_LATCHES.USP;
   } else if (gate == 0x0800) {
      BUS = CURRENT_LATCHES.VECTOR;
   }
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

/*
* Datapath routine for computing all functions that need to latch
* values in the data path at the end of this cycle.  Some values
* require sourcing the bus; therefore, this routine has to come
* after drive_bus.
*/
void latch_datapath_values() {
   if (GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION)) {
      NEXT_LATCHES.MAR = Low16bits(BUS);
   }
   if (GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION)) {
      if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION)) {
         if (CURRENT_LATCHES.READY) {
            int address = CURRENT_LATCHES.MAR >> 1;
            int byte = CURRENT_LATCHES.MAR & 0x01;
               printf("read\n");
               if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION)) {
                  NEXT_LATCHES.MDR = MEMORY[address][0] & 0x00FF;
                  NEXT_LATCHES.MDR |= (MEMORY[address][1] & 0x00FF) << 8;
               } else {
                  NEXT_LATCHES.MDR = signExtend(MEMORY[address][byte] & 0x00FF, 7);
               }
            NEXT_LATCHES.READY = 0;
            MEMEN = 0;
         }
      } else {
         if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION)) NEXT_LATCHES.MDR = Low16bits(BUS);
         else NEXT_LATCHES.MDR = BUS & 0x00FF;
      }
   }
   if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION)) {
      NEXT_LATCHES.IR = Low16bits(BUS);
   }
   if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION)) {
      if (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0) NEXT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 9) & 0x07] = Low16bits(BUS);
      else NEXT_LATCHES.REGS[7] = Low16bits(BUS);
   }
   if (GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION)) {
      setConditionCodes(signExtend(Low16bits(BUS), 15));
   }
   if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION)) {
      if (GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
         NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
         printf("loading pc + 2\n");
      }
      else if (GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
         NEXT_LATCHES.PC = Low16bits(BUS);
         printf("loading pc from bus\n");
      }
      else {
         printf("loading pc from adder\n");
         int op1, op2;
         if (GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
            if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)) op1 = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 6) & 0x07];
            else op1 = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 9) & 0x07];
         } else {
            op1 = CURRENT_LATCHES.PC;
         }
         if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) op2 = signExtend(CURRENT_LATCHES.IR & 0x003F, 5);
         else if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2) op2 = signExtend(CURRENT_LATCHES.IR & 0x01FF, 8);
         else if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3) op2 = signExtend(CURRENT_LATCHES.IR & 0x07FF, 10);
         else op2 = 0;
         if (GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION)) op2 = op2 << 1;
         NEXT_LATCHES.PC = op1 + op2;
      }
   }
   if (GetLD_TEMP(CURRENT_LATCHES.MICROINSTRUCTION)) {
      NEXT_LATCHES.TEMP = Low16bits(BUS);
   }
   if (GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION)) {
      NEXT_LATCHES.PSR = Low16bits(BUS);
   }
   if (GetLD_PRIV(CURRENT_LATCHES.MICROINSTRUCTION)) {
      NEXT_LATCHES.PSR &= 0x007F;
   }
   if (GetLD_SaveSSP(CURRENT_LATCHES.MICROINSTRUCTION)) {
      NEXT_LATCHES.SSP = Low16bits(BUS);
   }
   if (GetLD_SaveUSP(CURRENT_LATCHES.MICROINSTRUCTION)) {
      NEXT_LATCHES.USP = Low16bits(BUS);
   }
   if (GetLD_EX(CURRENT_LATCHES.MICROINSTRUCTION)) {
      int ex = 0;
      if ((CURRENT_LATCHES.PSR & 0x8000) && (CURRENT_LATCHES.MAR < 0x3000)) {
         ex = 3;
      } else if ((CURRENT_LATCHES.MAR & 0x01) && GetALIGN(CURRENT_LATCHES.MICROINSTRUCTION)) {
         ex = 1;
      }
      NEXT_LATCHES.EX = ex;
   }
   if (GetLD_EXCV(CURRENT_LATCHES.MICROINSTRUCTION)) {
      int mux1 = GetEXCMUX(CURRENT_LATCHES.MICROINSTRUCTION);
      int mux2;
      else if (mux1 == 1) mux2 = 0;
      else mux2 = CURRENT_LATCHES.EX;
      if (mux2 == 0) NEXT_LATCHES.EXCV = 0x04;
      else if (mux2 == 1) NEXT_LATCHES.EXCV = 0x03;
      else if (mux2 == 3) NEXT_LATCHES.EXCV = 0x02;
   }
   if (GetLD_Vector(CURRENT_LATCHES.MICROINSTRUCTION)) {
      if (GetIEMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
         NEXT_LATCHES.VECTOR = (CURRENT_LATCHES.EXCV << 1) | 0x0200;
      } else NEXT_LATCHES.VECTOR = (CURRENT_LATCHES.INTV << 1) | 0x0200;
   }
}
