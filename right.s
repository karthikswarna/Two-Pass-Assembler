START
ADD $t5 $t6 $t5
MUL $0 $s1 $s2
SUB $a0 $a1 $a2
DIV $s1 $s2 $s3
P1:
STORE $s2 8($v0)        # a singleline comment.
LOAD $t5 132($a0)
P2:
<a 
multiline 
comment>
AND $t3 $t5 $t1
OR $s5 $s2 $t0
NOR $v1 $t7 $t9
XOR $t3 $t6 $s1
SLT $a0 $s0 $s1
JAL C3
C1:
NORI $s5 $s2 14
XORI $s5 $s2 23
ANDI $s5 $s2 3
ORI $s5 $s2 15
C2:
ADDI $a3 $t6 47
MULI $t2 $t8 0
DIVI $t5 $t4 54
C3:
LSHIFT $t4 $t5 $t6
RSHIFT $t7 $t8 $t7
BEQ $t5 $t7 P1
BNE $s1 $s6 C3
JMP C1
END