START
ADDI $t0 $0 3214
ADDI $t1 $0 0			# indicates previous bit.
ADDI $t2 $0 0			# count number of 1 sequences.
ADDI $t3 $0 0			# count number of 0 sequences.
ADDI $t4 $0 31			# loop counter.
ANDI $t5 $t0 1
BEQ $t5 $t1 P1          # if LSB is 0, branch to "P1".
ADDI $t2 $t2 1	        # increment to 1's sequence.
ADDI $t1 $0 1
JMP F1
P1:
ADDI $t3 $t3 1	        # increment to 0's sequence.
ADDI $t1 $0 0
F1:
BLE $t4 $0 P4
LSHIFT $t0 $t0 1
ANDI $t5 $t0 1
BEQ $t5 $t1 P3
BNE $t5 $zero P2
ADDI $t3 $t3 1
ADDI $t1 $0 0
JMP P3
P2:
ADDI $t2 $t2 1
ADDI $t1 $0 1
JMP P3
P3:
ADDI $t4 $t4 -1
JMP F1
P4:
END