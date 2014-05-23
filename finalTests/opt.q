MOVIF a2  T0
PLUS a2 f T1
ASSIGN T1  x
PLUS a2 1 T2
ASSIGN T2  y
PLUS a2 1 T3
ASSIGN T3  z
MOVIF a2  T4
PLUS a2 f T5
ASSIGN T5  g
while_1_start: JMPC LT a2 1000  while_1_begin 
JMP while_1_end  
while_1_begin: MOVIF a2  T6
PLUS a2 f T7
ASSIGN T7  x
MULT f 2 T9
ASSIGN T9  f
PLUS a2 100 T11
ASSIGN T11  a2
JMP while_1_start  
DEFAULT while_1_end  
PRINT x  
PRINT s  
RET a2  
JMP Label1  

ASSIGN 0  res
ASSIGN "\n"  s

IN   a2
IN   a3
CALL abc a2a3 T0
ASSIGN T0  res
