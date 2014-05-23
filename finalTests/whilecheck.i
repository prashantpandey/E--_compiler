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
MOVF F001 F002				// ASSIGN f  tp
MOVI 0 R009				// ASSIGN 0  tp1
ADD R008 1 R010				// PLUS a2 1 tp3
MOVI 0 R011				// ASSIGN 0  i
while_1_start: JMPC GT 100 R008 while_1_begin				// while_1_start: JMPC LT a2 100  while_1_begin 
JMP while_1_end				// JMP while_1_end  
while_1_begin: PRTI R011				// while_1_begin: PRINT i  
while_1_1_start: MOVIF R008 F004				// while_1_1_start: MOVIF a2  T1
MOVIF R008 F007
FADD F007 1.0 F006				// PLUS a2 1 T2
JMPC FGT F006 F002 while_1_1_begin				// JMPC LT tp T2  while_1_1_begin 
JMP while_1_1_end				// JMP while_1_1_end  
while_1_1_begin: FADD F002 10.0 F002				// while_1_1_begin: PLUS tp 10 tp
ADD R009 1 R009				// PLUS tp1 1 tp1
JMP while_1_1_start				// JMP while_1_1_start  
while_1_1_end: MOVF F001 F002				// ASSIGN f  tp
ADD R008 1 R008				// PLUS a2 1 a2
JMP while_1_start				// JMP while_1_start  
while_1_end: PRTS R007				// PRINT s  
PRTI R009				// PRINT tp1  
PRTS R007				// PRINT s  
JMPC GT 100 R010 Label2				// JMPC LT tp3 100  Label2 
JMP Label4				// JMP Label4  
Label2: FADD F001 1.8 F012				// Label2: PLUS f 1.8 T10
STI R008 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F001 R000				// Flushing Registers- f
SUB R000 1 R000
STF F002 R000				// Flushing Registers- tp
SUB R000 1 R000
STI R009 R000				// Flushing Registers- tp1
SUB R000 1 R000
STI R010 R000				// Flushing Registers- tp3
SUB R000 1 R000
STI R011 R000				// Flushing Registers- i
SUB R000 1 R000
STF F004 R000				// Flushing Registers- T1
SUB R000 1 R000
STF F006 R000				// Flushing Registers- T2
SUB R000 1 R000
STF F012 R000				// Flushing Registers- T10
SUB R000 1 R000
STF F012 R000				// Pushing parameter- T10
SUB R000 1 R000
STI R010 R000				// Pushing parameter- tp3
SUB R000 1 R000
MOVL Label5 R004
STI R004 R000
SUB R000 1 R000
JMP Label0
Label5: MOVI R002 R013				// Getting return Value
ADD R000 1 R000
LDF R000 F012				// Loading Back Registers:T10
ADD R000 1 R000
LDF R000 F006				// Loading Back Registers:T2
ADD R000 1 R000
LDF R000 F004				// Loading Back Registers:T1
ADD R000 1 R000
LDI R000 R011				// Loading Back Registers:i
ADD R000 1 R000
LDI R000 R010				// Loading Back Registers:tp3
ADD R000 1 R000
LDI R000 R009				// Loading Back Registers:tp1
ADD R000 1 R000
LDF R000 F002				// Loading Back Registers:tp
ADD R000 1 R000
LDF R000 F001				// Loading Back Registers:f
ADD R000 1 R000
LDI R000 R008				// Loading Back Registers:a2
Label4: MOVI R008 R002
JMP Label1				// JMP Label1  
Label1: MOVI R001 R000				// Function Exit: Restoring BP
ADD R000 1 R000
LDI R000 R001				// Loading BP from stack
ADD R000 1 R000
LDI R000 R004				// Getting Return Address from stack
ADD R000 2 R000
JMPI R004
//
eventLabel_a: INI R011				// IN   a2
INF F012				// IN   a3
STI R011 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F012 R000				// Flushing Registers- a3
SUB R000 1 R000
STF F012 R000				// Pushing parameter- a3
SUB R000 1 R000
STI R011 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label6 R004
STI R004 R000
SUB R000 1 R000
JMP Label0
Label6: MOVI R002 R006				// Getting return Value
ADD R000 1 R000
LDF R000 F012				// Loading Back Registers:a3
ADD R000 1 R000
LDI R000 R011				// Loading Back Registers:a2
JMP EventMStart
//
begin: MOVI 10000 R000
MOVI 0 R006				// ASSIGN 0  res
MOVS "\n" R007				// ASSIGN "\n"  s
IN R011
IN R011
IN R011
EventMStart: IN R011
JMPC GT 64 R011 EventMOut
JMPC EQ 97 R011 eventLabel_a
JMP EventMStart
EventMOut: PRTS "\nDone\n"
