JMP begin
//
Label0: ADD R000 2 R005				// Function Start-abc, Setting temp register to load params
STI R001 R000				// Saving BP
SUB R000 1 R000
MOVI R000 R001				// Saving SP to BP
LDI R005 R008				// Loading param: a2
ADD R005 1 R005
LDF R005 F001				// Loading param: f
ADD R005 1 R005
MOVI 8 R009				// ASSIGN 8  h
MOVI 20 R010				// ASSIGN 20  c
MOVS "\n" R011				// ASSIGN "\n"  n
MOVI 3 R012				// ASSIGN 3  b
MOVI R010 R013				// SHL start: SHL c h r
MOVI R009 R014
MOVI 1 R005
Label2: JMPC GT R005 R014 Label3
MUL R013 2 R013
ADD 1 R005 R005
JMP Label2				// SHL Ends
Label3: MOVI R010 R014				// SHL start: SHR c b s
MOVI R012 R015
MOVI 1 R005
Label4: JMPC GT R005 R015 Label5
DIV R014 2 R014
ADD 1 R005 R005
JMP Label4				// SHL Ends
Label5: PRTS R011				// PRINT n  
PRTI R014				// PRINT s  
PRTS R011				// PRINT n  
PRTI R013				// PRINT r  
PRTS R011				// PRINT n  
MOVI R014 R002
JMP Label1				// JMP Label1  
Label1: MOVI R001 R000				// Function Exit: Restoring BP
ADD R000 1 R000
LDI R000 R001				// Loading BP from stack
ADD R000 1 R000
LDI R000 R004				// Getting Return Address from stack
ADD R000 2 R000
JMPI R004
//
eventLabel_a: INI R014				// IN   a2
INF F014				// IN   a3
STI R014 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F014 R000				// Flushing Registers- a3
SUB R000 1 R000
STF F014 R000				// Pushing parameter- a3
SUB R000 1 R000
STI R014 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label6 R004
STI R004 R000
SUB R000 1 R000
PRTI R000
PRTS "\n"
JMP Label0
Label6: PRTI R000
PRTS "\n"
MOVI R002 R006				// Getting return Value
ADD R000 1 R000
LDF R000 F014				// Loading Back Registers:a3
ADD R000 1 R000
LDI R000 R014				// Loading Back Registers:a2
PRTI R006				// PRINT res  
JMP EventMStart
//
begin: MOVI 10000 R000
MOVI 0 R006				// ASSIGN 0  res
MOVS "\n" R007				// ASSIGN "\n"  s
IN R014
IN R014
IN R014
EventMStart: IN R014
JMPC GT 64 R014 EventMOut
JMPC EQ 97 R014 eventLabel_a
JMP EventMStart
EventMOut: PRTS "\nDone\n"
