PRINT a2  
PRINT s  
PRINT f  
PRINT s  
PLUS a2 1 T0
ASSIGN T0  a2
JMPC EQ a2 85  Label2 
JMP Label4  
Label2: RET a2  
JMP Label1  
DEFAULT Label4  
CALL abc a2f T2
RET a2  
JMP Label1  

ASSIGN 0  res
ASSIGN "\n"  s

IN   a2
IN   a3
CALL abc a2a3 T0
ASSIGN T0  res
PRINT res  
