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
ADD R008 1 R010				// PLUS a2 1 T0
MOVI R010 R008				// ASSIGN T0  a2
JMPC EQ R008 85 Label2				// JMPC EQ a2 85  Label2 
JMP Label4				// JMP Label4  
Label2: MOVI R008 R002
JMP Label1				// JMP Label1  
Label4: STI R008 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F001 R000				// Flushing Registers- f
SUB R000 1 R000
STI R010 R000				// Flushing Registers- T0
SUB R000 1 R000
STF F001 R000				// Pushing parameter- f
SUB R000 1 R000
STI R008 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label5 R004
STI R004 R000
SUB R000 1 R000
JMP Label0
Label5: MOVI R002 R012				// Getting return Value
LDI R000 R010				// Loading Back Registers:T0
ADD R000 1 R000
LDF R000 F001				// Loading Back Registers:f
ADD R000 1 R000
LDI R000 R008				// Loading Back Registers:a2
ADD R000 1 R000
MOVI R008 R002
JMP Label1				// JMP Label1  
Label1: MOVI R001 R000				// Function Exit: Restoring BP
ADD R000 1 R000
LDI R000 R001				// Loading BP from stack
ADD R000 1 R000
LDI R000 R004				// Getting Return Address from stack
ADD R000 2 R000
JMPI R004
//
eventLabel_a: INI R010				// IN   a2
INF F010				// IN   a3
STI R010 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F010 R000				// Flushing Registers- a3
SUB R000 1 R000
STF F010 R000				// Pushing parameter- a3
SUB R000 1 R000
STI R010 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label6 R004
STI R004 R000
SUB R000 1 R000
JMP Label0
Label6: MOVI R002 R014				// Getting return Value
LDF R000 F010				// Loading Back Registers:a3
ADD R000 1 R000
LDI R000 R010				// Loading Back Registers:a2
ADD R000 1 R000
MOVI R014 R006				// ASSIGN T0  res
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
