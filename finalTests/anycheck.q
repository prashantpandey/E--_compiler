PRINT a2  
PRINT s  
PRINT f  
PRINT s  
PLUS a2 1 T0
ASSIGN T0  a2
CALL abc a2f T2
RET a2  
JMP Label1  

ASSIGN 0  res
ASSIGN "HI"  u
ASSIGN "\n"  s

IN   a2
IN   a3
PRINT a2  
PRINT s  
PRINT a3  
PRINT s  
CALL abc a2a3 T0
ASSIGN T0  res
PRINT res  
PRINT u  
