ASSIGN 0  g
JMPC LE 2 3  Label5 
DEFAULT Label6  
JMPC LE 6 7  Label5 
DEFAULT Label5  
JMPC GT g 0  Label2 
JMP Label3  
Label2: ASSIGN 3  g
JMP Label4  
Label3: ASSIGN 4  g
DEFAULT Label4  


IN   a2
IN   a3
CALL abc a2a3 
