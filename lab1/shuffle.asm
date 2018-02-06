; Name: Noah Kessler
; UTEID: njk576

   .ORIG x3000             ;0x3000
   ;INITIALIZATION, LOAD x4000 INTO R0 AND CONTROL MASK INTO R1
   LEA R1, ADR             ;0xE219
   LDW R0, R1, #0          ;0x6040
   LDB R1, R0, #4          ;0x2204
   ;STORE DATA IN LOOP
   ;R7 IS COUNTER
   ;FIRST RUN STORES DATA IN x4005
   AND R7, R7, #0          ;0x5FE0
   ;DETERMINE WHICH DATA TO LOAD
LOOP  ADD R2, R1, #0       ;0x1460
   ADD R6, R7, R7          ;0x1DC7
   BRZ MASK                ;0x0409
   ADD R5, R7, #-1         ;0x1BFF
   BRZ ONE                 ;0x0406
   ADD R5, R7, #-2         ;0x1BFE
   BRZ TWO                 ;0x0402
   RSHFL R2, R2, #6        ;0xD496
   BRNZP MASK              ;0x0E03
TWO   RSHFL R2, R2, #4     ;0xD494
   BRNZP MASK              ;0x0E01
ONE   RSHFL R2, R2, #2     ;0xD492
MASK  AND R2, R2, #3       ;0x54A3
   ;LOAD AND STORE DATA
   ADD R2, R0, R2          ;0x1402
   LDB R3, R2, #0          ;0x2680
   ADD R6, R0, #5          ;0x1C25
   ADD R6, R6, R7          ;0x1D87
   STB R3, R6, #0          ;0x3780
   ADD R7, R7, #1          ;0x1FE1
   ADD R6, R7, #-4         ;0x1DFC
   BRN LOOP                ;0x09EB
   HALT                    ;0xF025
ADR   .FILL x4000          ;0x4000
   .END
