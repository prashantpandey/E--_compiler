ASSIGN 0  g
ASSIGN 5  h
ASSIGN 10  i
ASSIGN 15  j
ASSIGN "\n"  s
while_1_start: JMPC LT g 20  while_1_begin 
JMP while_1_end  
while_1_begin: PLUS g 1 T0
ASSIGN T0  g
PRINT g  
PRINT s  
while_1_1_start: JMPC LT h 20  while_1_1_begin 
JMP while_1_1_end  
while_1_1_begin: PLUS h 1 T2
ASSIGN T2  h
PRINT h  
PRINT s  
while_1_1_1_start: JMPC LT i 20  while_1_1_1_begin 
JMP while_1_1_1_end  
while_1_1_1_begin: PLUS i 1 T4
ASSIGN T4  i
PRINT i  
PRINT s  
while_1_1_1_1_start: JMPC LT j 20  while_1_1_1_1_begin 
JMP while_1_1_1_1_end  
while_1_1_1_1_begin: PLUS j 1 T6
ASSIGN T6  j
PRINT j  
PRINT s  
JMP while_1_1_end  
JMP while_1_1_1_1_start  
DEFAULT while_1_1_1_1_end  
JMP while_1_1_1_start  
DEFAULT while_1_1_1_end  
while_1_1_2_start: JMPC LT i 10  while_1_1_2_begin 
JMP while_1_1_2_end  
while_1_1_2_begin: PLUS i 1 T8
ASSIGN T8  i
PRINT i  
PRINT s  
JMP while_1_1_end  
JMP while_1_1_2_start  
DEFAULT while_1_1_2_end  
JMP while_1_1_start  
DEFAULT while_1_1_end  
JMP while_1_start  
DEFAULT while_1_end  
PRINT g  


IN   a2
IN   a3
CALL abc a2a3 
