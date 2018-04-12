   .ORIG x3000
   
;add first 20 bytes stored at xC000
   LEA R1, ADR2
   LDW R0, R1, #0
   ADD R2, R2, #9
   ADD R2, R2, #10
   AND R1, R1, #0
LOOP LDB R3, R0, #0
   ADD R1, R1, R3
   ADD R0, R0, #1
   ADD R2, R2, #-1
   BRZP LOOP

;store result at xC014
   LEA R2, ADR3
   LDW R0, R2, #0
   STW R1, R0, #0

   JMP R1

   HALT

ADR2 .FILL xC000
ADR3 .FILL xC014
.END
