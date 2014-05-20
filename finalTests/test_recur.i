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
PRTI R008				// PRINT a2  
PRTS R007				// PRINT s  
PRTF F001				// PRINT f  
PRTS R007				// PRINT s  
ADD R008 1 R008				// PLUS a2 1 a2
FADD F001 1 F001				// PLUS f 1 f
MOVI R008 R002
MOVI R001 R000				// Function Exit: Restoring BP
ADD R000 1 R000
LDI R000 R001				// Loading BP from stack
ADD R000 1 R000
LDI R000 R004				// Getting Return Address from stack
ADD R000 2 R000
JMPI R004
//
eventLabel_a: INI R008				// IN   a2
INF F001				// IN   a3
PRTI R008				// PRINT a2  
PRTS R007				// PRINT s  
PRTF F001				// PRINT a3  
PRTS R007				// PRINT s  
STI R008 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F001 R000				// Flushing Registers- a3
SUB R000 1 R000
STF F001 R000				// Pushing parameter- a3
SUB R000 1 R000
STI R008 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label1 R004
STI R004 R000
SUB R000 1 R000
JMP Label0
Label1: MOVI R002 R006				// Getting return Value
LDF R000 F001				// Loading Back Registers:a3
ADD R000 1 R000
LDI R000 R008				// Loading Back Registers:a2
ADD R000 1 R000
PRTI R006				// PRINT res  
JMP EventMStart
//
begin: MOVI 10000 R000
MOVI 0 R006				// ASSIGN 0  res
MOVS "\n" R007				// ASSIGN "\n"  s
IN R008
IN R008
IN R008
EventMStart: IN R008
JMPC GT 64 R008 EventMOut
JMPC EQ 97 R008 eventLabel_a
JMP EventMStart
EventMOut: PRTS "\nDone\n"
