ASSIGN 0  x
ASSIGN 9  y
ASSIGN 11  z
MULT 1 z T0
PLUS a2 T0 T1
PLUS T1 y T2
MULT y 9 T3
PLUS z T3 T4
PLUS T2 T4 T5
DIV a2 z T6
PLUS T5 T6 T7
MULT 9 11 T8
DIV T8 3 T9
PLUS T7 T9 T10
PLUS T10 y T11
ASSIGN T11  x
PRINT x  
PRINT s  
RET x  
JMP Label1  

ASSIGN 0  res
ASSIGN "\n"  s

IN   a2
IN   a3
CALL abc a2a3 T0
ASSIGN T0  res
