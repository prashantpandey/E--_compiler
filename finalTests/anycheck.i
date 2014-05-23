JMP begin
//
Label0: ADD R000 2 R005				// Function Start-abc, Setting temp register to load params
STI R001 R000				// Saving BP
SUB R000 1 R000
MOVI R000 R001				// Saving SP to BP
LDI R005 R009				// Loading param: a2
ADD R005 1 R005
LDF R005 F001				// Loading param: f
ADD R005 1 R005
PRTI R009				// PRINT a2  
PRTS R008				// PRINT s  
PRTF F001				// PRINT f  
PRTS R008				// PRINT s  
ADD R009 1 R009				// PLUS a2 1 a2
STI R009 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F001 R000				// Flushing Registers- f
SUB R000 1 R000
STF F001 R000				// Pushing parameter- f
SUB R000 1 R000
STI R009 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label2 R004
STI R004 R000
SUB R000 1 R000
JMP Label0
Label2: MOVI R002 R011				// Getting return Value
ADD R000 1 R000
LDF R000 F001				// Loading Back Registers:f
ADD R000 1 R000
LDI R000 R009				// Loading Back Registers:a2
MOVI R009 R002
JMP Label1				// JMP Label1  
Label1: MOVI R001 R000				// Function Exit: Restoring BP
ADD R000 1 R000
LDI R000 R001				// Loading BP from stack
ADD R000 1 R000
LDI R000 R004				// Getting Return Address from stack
ADD R000 2 R000
JMPI R004
//
eventLabel_c: INI R009				// IN   a2
INF F001				// IN   a3
PRTI R009				// PRINT a2  
PRTS R008				// PRINT s  
PRTF F001				// PRINT a3  
PRTS R008				// PRINT s  
STI R009 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F001 R000				// Flushing Registers- a3
SUB R000 1 R000
STF F001 R000				// Pushing parameter- a3
SUB R000 1 R000
STI R009 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label3 R004
STI R004 R000
SUB R000 1 R000
JMP Label0
Label3: MOVI R002 R006				// Getting return Value
ADD R000 1 R000
LDF R000 F001				// Loading Back Registers:a3
ADD R000 1 R000
LDI R000 R009				// Loading Back Registers:a2
PRTI R006				// PRINT res  
JMP EventMStart
//
eventLabel_any: PRTS R007				// PRINT u  
JMP EventMStart
//
begin: MOVI 10000 R000
MOVI 0 R006				// ASSIGN 0  res
MOVS "HI" R007				// ASSIGN "HI"  u
MOVS "\n" R008				// ASSIGN "\n"  s
IN R009
IN R009
IN R009
EventMStart: IN R009
JMPC GT 64 R009 EventMOut
JMPC EQ 99 R009 eventLabel_c
JMP eventLabel_any
JMP EventMStart
EventMOut: PRTS "\nDone\n"
