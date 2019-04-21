START
ADDI $t0 $0 -25
ADDI $s0 $0 0				# register which indicates whether the number is positive or negative.
RSHIFT $t1 $t0 31		    # obtaining the MSB
ANDI $t1 $t1 1
BEQ $t1 $zero positive      # if MSB = 0, $s0 = 0
ADDI $s0 $s0 1		        # if MSB = 1, $s0 = 1
positive:
END