ASSIGN f  tp
ASSIGN 0  tp1
PLUS a2 1 T0
ASSIGN T0  tp3
ASSIGN 0  i
while_1_start: JMPC LT a2 100  while_1_begin 
JMP while_1_end  
while_1_begin: PRINT i  
while_1_1_start: MOVIF a2  T1
PLUS a2 1 T2
JMPC LT tp T2  while_1_1_begin 
JMP while_1_1_end  
while_1_1_begin: PLUS tp 10 T3
ASSIGN T3  tp
PLUS tp1 1 T5
ASSIGN T5  tp1
JMP while_1_1_start  
DEFAULT while_1_1_end  
ASSIGN f  tp
PLUS a2 1 T8
ASSIGN T8  a2
JMP while_1_start  
DEFAULT while_1_end  
PRINT s  
PRINT tp1  
PRINT s  
JMPC LT tp3 100  Label2 
JMP Label4  
Label2: PLUS f 1.8 T10
CALL abc tp3T10 T11
DEFAULT Label4  
RET a2  
JMP Label1  

ASSIGN 0  res
ASSIGN "\n"  s

IN   a2
IN   a3
CALL abc a2a3 T0
ASSIGN T0  res
