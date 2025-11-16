.ORIG x3000 ; program starts from this address

; Test ADD R1, R2, #5

AND R2, R2, #0 ; Clear R2 (R2 = 0)
ADD R2, R2, #10 ; Add 10 to R2 and store it in R2
ADD R1, R2, #5 ; R1 = R2 + 5. Expected R1 = 15 (x000F)

; Check Result
LD R3, EXPECTED_ADD_VAL ; Load EXPECTED_ADD_VAL to R3, EXPECTED_ADD_VAL is a label defined below
NOT R3, R3 ; R3 = -16 (negation)
ADD R3, R3, #1 ; R3 = R3 + 1, which is R3 = 15
ADD R0, R1, R3 ; R0 = R1 - R3. R0 should be 0 if R1=15.
BRz ADD_PASS ; Branch to label ADD_PASS if R0 is Zero else just go to next instruction i.e. ADD_FAIL, this cheks the conditional register

ADD_FAIL
LEA R0, FAIL_MSG
TRAP x22 ; PUTS (Print FAIL_MSG)
BR END_TEST

ADD_PASS
LEA R0, ADD_PASS_MSG
TRAP x22 ; PUTS (Print ADD_PASS_MSG)

; --- Defining labels ---
EXPECTED_ADD_VAL .FILL x000F ; 15
ADD_PASS_MSG .STRINGZ "ADD Test: Expected Output: 15 "
FAIL_MSG .STRINGZ "Test: Failed"

END_TEST
HALT
.END