.ORIG x1200

;push R0 and R1
ADD R6, R6, #-2
STW R0, R6, #0
ADD R0, R6, #2
STW R1, R6, #0

;add 1 to data at x4000
LEA R1, ADR
LDW R0, R1, #0
LDW R1, R0, #0
ADD R1, R1, #1
STW R1, R0, #0

;pop R0 and R1
LDW R1, R6, #0
ADD R6, R6, #2
LDW R0, R6, #0
ADD R6, R6, #2

RTI
ADR .FILL x4000
.END
