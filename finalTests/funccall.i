JMP begin
//
Label0: ADD R000 2 R005				// Function Start-abc, Setting temp register to load params
STI R001 R000				// Saving BP
SUB R000 1 R000
MOVI R000 R001				// Saving SP to BP
LDI R005 R006				// Loading param: a2
ADD R005 1 R005
LDF R005 F001				// Loading param: f
ADD R005 1 R005
MOVS "Hello World..!!!" R007				// ASSIGN "Hello World..!!!"  c
MOVS "\n" R008				// ASSIGN "\n"  s
PRTI R006				// PRINT a2  
PRTS R008				// PRINT s  
PRTF F001				// PRINT f  
PRTS R008				// PRINT s  
PRTS R007				// PRINT c  
PRTS R008				// PRINT s  
Label1: MOVI R001 R000				// Function Exit: Restoring BP
ADD R000 1 R000
LDI R000 R001				// Loading BP from stack
ADD R000 1 R000
LDI R000 R004				// Getting Return Address from stack
ADD R000 2 R000
JMPI R004
//
eventLabel_a: INI R008				// IN   a2
INF F008				// IN   a3
STI R008 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F008 R000				// Flushing Registers- a3
SUB R000 1 R000
STF F008 R000				// Pushing parameter- a3
SUB R000 1 R000
STI R008 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label2 R004
STI R004 R000
SUB R000 1 R000
JMP Label0
Label2: ADD R000 1 R000
LDF R000 F008				// Loading Back Registers:a3
ADD R000 1 R000
LDI R000 R008				// Loading Back Registers:a2
JMP EventMStart
//
begin: MOVI 10000 R000
IN R008
IN R008
IN R008
EventMStart: IN R008
JMPC GT 64 R008 EventMOut
JMPC EQ 97 R008 eventLabel_a
JMP EventMStart
EventMOut: PRTS "\nDone\n"
