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
while_1_begin: ADD R007 1 R013				// while_1_begin: PLUS g 1 T0
MOVI R013 R007				// ASSIGN T0  g
PRTI R007				// PRINT g  
PRTS R011				// PRINT s  
while_1_1_start: JMPC GT 20 R008 while_1_1_begin				// while_1_1_start: JMPC LT h 20  while_1_1_begin 
JMP while_1_1_end				// JMP while_1_1_end  
while_1_1_begin: ADD R008 1 R015				// while_1_1_begin: PLUS h 1 T2
MOVI R015 R008				// ASSIGN T2  h
PRTI R008				// PRINT h  
PRTS R011				// PRINT s  
while_1_1_1_start: JMPC GT 20 R009 while_1_1_1_begin				// while_1_1_1_start: JMPC LT i 20  while_1_1_1_begin 
JMP while_1_1_1_end				// JMP while_1_1_1_end  
while_1_1_1_begin: ADD R009 1 R017				// while_1_1_1_begin: PLUS i 1 T4
MOVI R017 R009				// ASSIGN T4  i
PRTI R009				// PRINT i  
PRTS R011				// PRINT s  
while_1_1_1_1_start: JMPC GT 20 R010 while_1_1_1_1_begin				// while_1_1_1_1_start: JMPC LT j 20  while_1_1_1_1_begin 
JMP while_1_1_1_1_end				// JMP while_1_1_1_1_end  
while_1_1_1_1_begin: ADD R010 1 R019				// while_1_1_1_1_begin: PLUS j 1 T6
MOVI R019 R010				// ASSIGN T6  j
PRTI R010				// PRINT j  
PRTS R011				// PRINT s  
JMP while_1_1_end				// JMP while_1_1_end  
JMP while_1_1_1_1_start				// JMP while_1_1_1_1_start  
while_1_1_1_1_end: JMP while_1_1_1_start				// JMP while_1_1_1_start  
while_1_1_1_end: JMPC GT 10 R009 while_1_1_2_begin				// while_1_1_2_start: JMPC LT i 10  while_1_1_2_begin 
JMP while_1_1_2_end				// JMP while_1_1_2_end  
while_1_1_2_begin: ADD R009 1 R021				// while_1_1_2_begin: PLUS i 1 T8
MOVI R021 R009				// ASSIGN T8  i
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
eventLabel_a: INI R021				// IN   a2
INF F021				// IN   a3
STI R021 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F021 R000				// Flushing Registers- a3
SUB R000 1 R000
STF F021 R000				// Pushing parameter- a3
SUB R000 1 R000
STI R021 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label2 R004
STI R004 R000
SUB R000 1 R000
PRTI R000
PRTS "\n"
JMP Label0
Label2: PRTI R000
PRTS "\n"
ADD R000 1 R000
LDF R000 F021				// Loading Back Registers:a3
ADD R000 1 R000
LDI R000 R021				// Loading Back Registers:a2
JMP EventMStart
//
begin: MOVI 10000 R000
IN R021
IN R021
IN R021
EventMStart: IN R021
JMPC GT 64 R021 EventMOut
JMPC EQ 97 R021 eventLabel_a
JMP EventMStart
EventMOut: PRTS "\nDone\n"
