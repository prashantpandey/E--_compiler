ASSIGN 8  h
ASSIGN 20  c
ASSIGN "\n"  n
ASSIGN 3  b
SHL c h T0
ASSIGN T0  r
SHR c b T1
ASSIGN T1  s
PRINT n  
PRINT s  
PRINT n  
PRINT r  
PRINT n  
RET s  
JMP Label1  

ASSIGN 0  res
ASSIGN "\n"  s

IN   a2
IN   a3
CALL abc a2a3 T0
ASSIGN T0  res
PRINT res  
