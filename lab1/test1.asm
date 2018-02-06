;
; Program to count occurrences of a character in a File.
; Character to be input from the keyboard.
; Result to be displayed on the monitor.
; Program works only if no more than 9 occurrences are found.
;
;
; Initialization
;
.ORIG x3000

AND R2,R2,#0 ; R2 is counter, initialize to 0
LEA R3,PTR ; R3 is pointer to characters
LDW R3,R3,#0
TRAP x23 ; R0 gets character input
LDB R1,R3,#0 ; R1 gets the next character
;
; Test character for end of file
;
TESTTESTTESTTESTTEST1 ADD R4,R1,#-4 ; Test for EOT
BRz OUTPUT ; If done, prepare the output
;
; Test character for match. If a match, increment count.
;
NOT R1,R1
ADD R1,R1,R0 ; If match, R1 = xFFFF
NOT R1,R1 ; If match, R1 = x0000
BRnp GETCHAR ; If no match, do not increment
ADD R2,R2,#1
;
; Get next character from the file
;
GETCHAR ADD R3,R3,#1 ; Increment the pointer
LDB R1,R3,#0 ; R1 gets the next character to test
BRnzp TESTTESTTESTTESTTEST1
;
; Output the count.
;
OUTPUT LEA R0, ASCII ; Load the ASCII template
LDW R0,R0,#0
ADD R0,R0,R2 ; Convert binary to ASCII
TRAP x21 ; ASCII code in R0 is displayed
TRAP x25 ; Halt machine
;
; Storage for pointer and ASCII template
;

ASCII .FILL x0030
PTR .FILL x4000
.END


;0011000000000000    0x3000
;0101010010100000    0x54A0
;1110011000010011    0xE613
;0110011011000000    0x66C0
;1111000000100011    0xF023
;0010001011000000    0x22C0
;0001100001111100    0x187C
;0000010000001000    0x0408
;1001001001111111    0x927F
;0001001001000000    0x1240
;1001001001111111    0x927F
;0000101000000001    0x0A01
;0001010010100001    0x14A1
;0001011011100001    0x16E1
;0010001011000000    0x22C0
;0000111111110110    0x0FF6
;1110000000000100    0xE004
;0110000000000000    0x6000
;0001000000000010    0x1002
;1111000000100001    0xF021
;1111000000100101    0xF025
;0000000000110000    0x0030
;0100000000000000    0x4000
