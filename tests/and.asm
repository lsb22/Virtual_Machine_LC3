.ORIG x3100

; Test AND R1, R2, R3
AND R2, R2, #0	; Clear R2
ADD R2, R2, #6	; R2 = 6
AND R3, R3, #0	; Clear R3
ADD R3, R3, #3	; R3 = 3
AND R1, R2, R3	; R1 = R2 AND R3. Expected R1 = 2 (x0002)

; Check Result
LD R3, EXPECTED_AND_VAL ; R3 = 2
NOT R3, R3  ; negation of R3 will give -3 
ADD R3, R3, #1      ; R3 = -2
ADD R0, R1, R3		; R0 = R1 + R3. R0 should be 0 if R1=2.
BRz AND_PASS
	
AND_FAIL
LEA R0, FAIL_MSG
TRAP x22
BR END_TEST_AND

AND_PASS
LEA R0, AND_PASS_MSG
TRAP x22

; --- Defining labels ---
EXPECTED_AND_VAL .FILL x0002 ; 2
AND_PASS_MSG .STRINGZ "AND Test: Expected Output: 2 "
FAIL_MSG .STRINGZ "AND Test failed"

END_TEST_AND
	HALT
.END