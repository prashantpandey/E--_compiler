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
MOVI 0 R007				// ASSIGN 0  g
MOVI 5 R008				// ASSIGN 5  h
MOVI 10 R009				// ASSIGN 10  i
MOVI 15 R010				// ASSIGN 15  j
MOVS "\n" R011				// ASSIGN "\n"  s
while_1_start: JMPC GT 20 R007 while_1_begin				// while_1_start: JMPC LT g 20  while_1_begin 
JMP while_1_end				// JMP while_1_end  
while_1_begin: ADD R007 1 R007				// while_1_begin: PLUS g 1 g
PRTI R007				// PRINT g  
PRTS R011				// PRINT s  
while_1_1_start: JMPC GT 20 R008 while_1_1_begin				// while_1_1_start: JMPC LT h 20  while_1_1_begin 
JMP while_1_1_end				// JMP while_1_1_end  
while_1_1_begin: ADD R008 1 R008				// while_1_1_begin: PLUS h 1 h
PRTI R008				// PRINT h  
PRTS R011				// PRINT s  
while_1_1_1_start: JMPC GT 20 R009 while_1_1_1_begin				// while_1_1_1_start: JMPC LT i 20  while_1_1_1_begin 
JMP while_1_1_1_end				// JMP while_1_1_1_end  
while_1_1_1_begin: ADD R009 1 R009				// while_1_1_1_begin: PLUS i 1 i
PRTI R009				// PRINT i  
PRTS R011				// PRINT s  
while_1_1_1_1_start: JMPC GT 20 R010 while_1_1_1_1_begin				// while_1_1_1_1_start: JMPC LT j 20  while_1_1_1_1_begin 
JMP while_1_1_1_1_end				// JMP while_1_1_1_1_end  
while_1_1_1_1_begin: ADD R010 1 R010				// while_1_1_1_1_begin: PLUS j 1 j
PRTI R010				// PRINT j  
PRTS R011				// PRINT s  
JMP while_1_1_end				// JMP while_1_1_end  
JMP while_1_1_1_1_start				// JMP while_1_1_1_1_start  
while_1_1_1_1_end: JMP while_1_1_1_start				// JMP while_1_1_1_start  
while_1_1_1_end: JMPC GT 10 R009 while_1_1_2_begin				// while_1_1_2_start: JMPC LT i 10  while_1_1_2_begin 
JMP while_1_1_2_end				// JMP while_1_1_2_end  
while_1_1_2_begin: ADD R009 1 R009				// while_1_1_2_begin: PLUS i 1 i
PRTI R009				// PRINT i  
PRTS R011				// PRINT s  
JMP while_1_1_end				// JMP while_1_1_end  
JMP while_1_1_1_end				// JMP while_1_1_2_start  
while_1_1_2_end: JMP while_1_1_start				// JMP while_1_1_start  
while_1_1_end: JMP while_1_start				// JMP while_1_start  
while_1_end: PRTI R007				// PRINT g  
Label1: MOVI R001 R000				// Function Exit: Restoring BP
ADD R000 1 R000
LDI R000 R001				// Loading BP from stack
ADD R000 1 R000
LDI R000 R004				// Getting Return Address from stack
ADD R000 2 R000
JMPI R004
//
eventLabel_a: INI R011				// IN   a2
INF F011				// IN   a3
STI R011 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F011 R000				// Flushing Registers- a3
SUB R000 1 R000
STF F011 R000				// Pushing parameter- a3
SUB R000 1 R000
STI R011 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label2 R004
STI R004 R000
SUB R000 1 R000
JMP Label0
Label2: ADD R000 1 R000
LDF R000 F011				// Loading Back Registers:a3
ADD R000 1 R000
LDI R000 R011				// Loading Back Registers:a2
JMP EventMStart
//
begin: MOVI 10000 R000
IN R011
IN R011
IN R011
EventMStart: IN R011
JMPC GT 64 R011 EventMOut
JMPC EQ 97 R011 eventLabel_a
JMP EventMStart
EventMOut: PRTS "\nDone\n"
