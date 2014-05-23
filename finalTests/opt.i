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
MOVIF R008 F003				// MOVIF a2  T0
MOVIF R008 F006
FADD F006 F001 F005				// PLUS a2 f T1
MOVF F005 F008				// ASSIGN T1  x
ADD R008 1 R010				// PLUS a2 1 T2
MOVI R010 R011				// ASSIGN T2  y
MOVI R010 R012				// ASSIGN T2  z
MOVIF R008 F010				// MOVIF a2  T4
MOVF F005 F011				// ASSIGN T1  g
while_1_start: JMPC GT 1000 R008 while_1_begin				// while_1_start: JMPC LT a2 1000  while_1_begin 
JMP while_1_end				// JMP while_1_end  
while_1_begin: MOVIF R008 F013				// while_1_begin: MOVIF a2  T6
MOVIF R008 F014
FADD F014 F001 F008				// PLUS a2 f x
FMUL F001 2.0 F001				// MULT f 2 f
ADD R008 100 R008				// PLUS a2 100 a2
JMP while_1_start				// JMP while_1_start  
while_1_end: PRTF F008				// PRINT x  
PRTS R007				// PRINT s  
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
INF F013				// IN   a3
STI R010 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F013 R000				// Flushing Registers- a3
SUB R000 1 R000
STF F013 R000				// Pushing parameter- a3
SUB R000 1 R000
STI R010 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label2 R004
STI R004 R000
SUB R000 1 R000
JMP Label0
Label2: MOVI R002 R006				// Getting return Value
ADD R000 1 R000
LDF R000 F013				// Loading Back Registers:a3
ADD R000 1 R000
LDI R000 R010				// Loading Back Registers:a2
JMP EventMStart
//
begin: MOVI 10000 R000
MOVI 0 R006				// ASSIGN 0  res
MOVS "\n" R007				// ASSIGN "\n"  s
IN R010
IN R010
IN R010
EventMStart: IN R010
JMPC GT 64 R010 EventMOut
JMPC EQ 97 R010 eventLabel_a
JMP EventMStart
EventMOut: PRTS "\nDone\n"
