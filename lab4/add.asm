   .ORIG x3000

;set x4000 to 1
   LEA R1, ADR1
   LDW R0, R1
   AND R2, #0
   ADD R2, R2, #1
   STW R2, R0

;add first 20 bytes stored at xC000
   LEA R1, ADR2
   LDW R0, R1
   ADD R2, R2, #9
   ADD R2, R2, #10
   AND R1, #0
LOOP LDB R3, R0
   ADD R1, R1, R3
   ADD R0, R0, #1
   ADD R2, R2, #-1
   BRZP LOOP

;store result at xC014
   LEA R2, ADR3
   LDW R0, R2
   STW R1, R0

;uncomment to test protection exception
   ;AND R0, #0
   ;STW R2, R0

;uncomment to test unaligned access exception
   ;ADD R0, R0, #3
   ;STW R1, R0

;uncomment to test unknown opcode exception
   ;.FILL xA000

   HALT

ADR1 .FILL x4000
ADR2 .FILL xC000
ADR3 .FILL xC014
.END
