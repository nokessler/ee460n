#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

FILE* infile = NULL;
FILE* outfile = NULL;

typedef struct {
   int address;
   char *label;
} table_entry;

table_entry symbol_table[255];

int toNum(char *pStr) {
  char *t_ptr;
  char *orig_pStr;
  int t_length,k;
  int lNum, lNeg = 0;
  long int lNumLong;
  orig_pStr = pStr;
  if (*pStr == '#') {                                /* decimal */
    pStr++;
    if (*pStr == '-') {                               /* dec is negative */
      lNeg = 1;
      pStr++;
    }
    t_ptr = pStr;
    t_length = strlen(t_ptr);
    for (k=0;k < t_length;k++) {
      if (!isdigit(*t_ptr)) {
         printf("Error: invalid decimal operand, %s\n",orig_pStr);
         exit(4);
      }
      t_ptr++;
    }
    lNum = atoi(pStr);
    if (lNeg) lNum = -lNum;
    return lNum;
  } else if (*pStr == 'x') {       /* hex     */
    pStr++;
    if (*pStr == '-') {                                /* hex is negative */
      lNeg = 1;
      pStr++;
    }
    t_ptr = pStr;
    t_length = strlen(t_ptr);
    for (k=0;k < t_length;k++) {
      if (!isxdigit(*t_ptr)) {
         printf("Error: invalid hex operand, %s\n",orig_pStr);
         exit(4);
      }
      t_ptr++;
    }
    lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
    lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
    if (lNeg) lNum = -lNum;
    return lNum;
  } else {
        printf("Error: invalid operand, %s\n", orig_pStr);
        exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
  }
}

/* checks to see if line starts with opcode or label
returns 1 if starts with opcode, -1 if starts with label */
int isOpcode(char *pt) {
   if (!strcmp(pt, "add")) return(1);
   if (!strcmp(pt, "and")) return(1);
   if (!strncmp(pt, "br", 2)) return(1);
   if (!strcmp(pt, "jmp") || !strcmp(pt, "ret")) return(1);
   if (!strcmp(pt, "jsr") || !strcmp(pt, "jsrr")) return(1);
   if (!strcmp(pt, "ldb")) return(1);
   if (!strcmp(pt, "ldw")) return(1);
   if (!strcmp(pt, "lea")) return(1);
   if (!strcmp(pt, "not") || !strcmp(pt, "xor")) return(1);
   if (!strcmp(pt, "rti")) return(1);
   if (!strcmp(pt, "lshf") || !strcmp(pt, "rshfl") || !strcmp(pt, "rshfa")) return(1);
   if (!strcmp(pt, "stb")) return(1);
   if (!strcmp(pt, "stw")) return(1);
   if (!strcmp(pt, "trap") || !strcmp(pt, "halt")) return(1);
   if (!strcmp(pt, "nop")) return(1);
   return(-1);
}

#define MAX_LINE_LENGTH 255
enum {
   DONE, OK, EMPTY_LINE
};
int readAndParse(FILE *pInfile, char *pLine, char **pLabel, char **pOpcode, char **pArg1, char **pArg2, char **pArg3, char **pArg4) {
   char *lRet, *lPtr;
   int i;
   if (!fgets(pLine, MAX_LINE_LENGTH, pInfile)) return(DONE);
   for (i = 0; i < strlen(pLine); i++) pLine[i] = tolower(pLine[i]); /* convert entire line to lowercase */
   *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine); /* ignore the comments */
   lPtr = pLine;
   while (*lPtr != ';' && *lPtr != '\0' && *lPtr != '\n' ) lPtr++;
   *lPtr = '\0';
   if (!(lPtr = strtok(pLine, "\t\n ,"))) return(EMPTY_LINE);
   if (isOpcode(lPtr) == -1 && lPtr[0] != '.') { /* found a label */
      *pLabel = lPtr;
      if(!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
   }
   *pOpcode = lPtr;
   if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
   *pArg1 = lPtr;
   if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
   *pArg2 = lPtr;
   if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
   *pArg3 = lPtr;
   if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
   *pArg4 = lPtr;
   return(OK);
} /* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */

int start;

/* first pass, creates symbol table */
int pass1(char *inFile) {
   char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
   int lRet;
   int i;
   int entry = 0;
   int location_counter = 0;
   FILE *lInfile;
   lInfile = fopen(inFile, "r");        /* open the input file */
   do {
      lRet = readAndParse(lInfile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
      if (lRet != DONE && lRet != EMPTY_LINE){
         /* Get line to start program */
         if (!strcmp(lOpcode, ".orig")) {
            start = toNum(lArg1);
            if (start % 2) exit(3);
            location_counter = start;
         }
         if (*lLabel) {
            /* Check if label is valid */
            if (strcmp(lLabel, "getc") || (strcmp(lLabel, "in")) || (strcmp(lLabel, "out")) || (strcmp(lLabel, "puts"))) exit(4);
            if (strlen(lLabel) > 20) exit(4);
            for (i = 0; i < 255; i++) {
               if (!strcmp(lLabel, symbol_table[i].label)) exit(4);
            }
            if (lLabel[0] == 'x') exit(4);
            if ((lLabel[0] < 'a') || (lLabel[0]) > 'z') exit(4);
            if ((lLabel[0] == 'r') && ((lLabel[1] - '0' >= 0) && (lLabel[1] - '0' <= 8))) exit(4);
            for (i = 0; i < 20; i++) {
               if ((lLabel[i] < '0') || ((lLabel[i] > '9') && (lLabel[i] < 'a')) || (lLabel[i] > 'z')) exit(4);
            }
            /* add label to symbol table */
            symbol_table[entry].label = lLabel;
            symbol_table[entry].address = location_counter;
            entry++;
            location_counter += 2;
         }
      }
   } while (lRet != DONE);
   return(0);
}

int registerNum(char *reg) {
   int num = reg[1] - '0';
   if ((num > 7) || (num < 0)) exit(4);
   return(num);
}

int labelOffset(char *label, int location) {
   int i;
   for (i = 0; i < 255; i++) {
      if (!strcmp(lLabel, symbol_table[i].label)) {
         int address = symbol_table[i].address;
         int pc = location + 2;
         int offset = (address - pc) / 2;
         return(offset);
      } else exit(1);
   }
}

int translate(char *opcode, char *arg1, char*arg2, char *arg3, char *arg4, int location) {
   if (!strcmp(opcode, "add")) {
      if (!*arg1 || !*arg2 || !*arg3 || *arg4) exit(4);
      int result = 0x1000;
      result += registerNum(arg1) << 9;
      result += registerNum(arg2) << 6;
      if (arg3[0] == 'r') result += registerNum(arg3);
      else {
         result += 1 << 5;
         int imm = toNum(arg3);
         if ((imm > 15) || (imm < -16)) exit(3);
         result += imm;
      }
      return(result);
   }
   if (!strcmp(opcode, "and")) {
      if (!*arg1 || !*arg2 || !*arg3 || *arg4) exit(4);
      int result = 0x5000;
      result += registerNum(arg1) << 9;
      result += registerNum(arg2) << 6;
      if (arg3[0] == 'r') result += registerNum(arg3);
      else {
         result += 1 << 5;
         int imm = toNum(arg3);
         if ((imm > 15) || (imm < -16)) exit(3);
         result += imm;
      }
      return(result);
   }
   if (!strncmp(opcode, "br", 2)) {
      if (!*arg1 || *arg2 || *arg3 || *arg4) exit(4);
      int result;
      if (!strcmp(opcode, "br") || !strcmp(opcode, "brnzp")) result = 0x0e00
      if (!strcmp(opcode, "brp")) result = 0x0200;
      if (!strcmp(opcode, "brz")) result = 0x0400;
      if (!strcmp(opcode, "brzp")) result = 0x0600;
      if (!strcmp(opcode, "brn")) result = 0x0800;
      if (!strcmp(opcode, "brnp")) result = 0x0a00;
      if (!strcmp(opcode, "brnz")) result = 0x0c00;
      result += labelOffset(arg1, location);
      return(result);
   }
   if (!strcmp(opcode, "jmp"))
   if (!strcmp(opcode, "ret"))
   if (!strcmp(opcode, "jsr"))
   if (!strcmp(opcode, "jsrr"))
   if (!strcmp(opcode, "ldb"))
   if (!strcmp(opcode, "ldw"))
   if (!strcmp(opcode, "lea"))
   if (!strcmp(opcode, "not"))
   if (!strcmp(opcode, "xor"))
   if (!strcmp(opcode, "rti"))
   if (!strcmp(opcode, "lshf"))
   if (!strcmp(opcode, "rshfl"))
   if (!strcmp(opcode, "rshfa"))
   if (!strcmp(opcode, "stb"))
   if (!strcmp(opcode, "stw"))
   if (!strcmp(opcode, "trap"))
   if (!strcmp(opcode, "halt"))
   if (!strcmp(opcode, "nop"))
}

int pass2(char *inFile, char *outFile) {
   char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
   int lRet;
   FILE * lInfile;
   lInfile = fopen(inFile, "r");        /* open the input file */
   FILE * pOutfile;
   pOutfile = fopen(outFile, "w");
   int location_counter = start;
   do {

      lRet = readAndParse(lInfile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
      if(lRet != DONE && lRet != EMPTY_LINE) {
         if (!strcmp(lOpcode, ".orig")) {
            fprintf(pOutfile, "0x%.4X\n", start); /* writes first line to output file */
            continue;
         }
         if (!strcmp(lOpcode, ".end")) break; /* breaks loop if end of file is reached */
         if (isOpcode(lOpcode) == -1) exit(2); /* checks for invalid opcodes */
         fprintf(pOutfile, "0x%.4X\n", translate(lOpcode, lArg1, lArg2, lArg3, lArg4, location_counter)); /* writes instruction in hex to output */
         location_counter += 2;
      }
   } while(lRet != DONE);
   return(0);
}


int main(int argc, char* argv[]) {
   char *prgName   = NULL;
   char *iFileName = NULL;
   char *oFileName = NULL;
   prgName   = argv[0];
   iFileName = argv[1];
   oFileName = argv[2];
   /* open the source file */
   infile = fopen(argv[1], "r");
   outfile = fopen(argv[2], "w");
   if (!infile) {
      printf("Error: Cannot open file %s\n", argv[1]);
      exit(4);
   }
   if (!outfile) {
      printf("Error: Cannot open file %s\n", argv[2]);
      exit(4);
   }
   pass1(argv[1]);
   pass2(argv[1], argv[2]);
   fclose(infile);
   fclose(outfile);
   exit(0);
}
