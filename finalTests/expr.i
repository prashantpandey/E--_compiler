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
MOVI 9 R009				// ASSIGN 9  y
MOVI 11 R010				// ASSIGN 11  z
MUL 1 R010 R012				// MULT 1 z T0
ADD R008 R012 R014				// PLUS a2 T0 T1
ADD R014 R009 R016				// PLUS T1 y T2
MUL R009 9 R018				// MULT y 9 T3
ADD R010 R018 R020				// PLUS z T3 T4
ADD R016 R020 R022				// PLUS T2 T4 T5
DIV R008 R010 R024				// DIV a2 z T6
ADD R022 R024 R026				// PLUS T5 T6 T7
MUL 9 11 R028				// MULT 9 11 T8
DIV R028 3 R030				// DIV T8 3 T9
ADD R026 R030 R032				// PLUS T7 T9 T10
ADD R032 R009 R033				// PLUS T10 y x
PRTI R033				// PRINT x  
PRTS R007				// PRINT s  
MOVI R033 R002
JMP Label1				// JMP Label1  
Label1: MOVI R001 R000				// Function Exit: Restoring BP
ADD R000 1 R000
LDI R000 R001				// Loading BP from stack
ADD R000 1 R000
LDI R000 R004				// Getting Return Address from stack
ADD R000 2 R000
JMPI R004
//
eventLabel_a: INI R032				// IN   a2
INF F032				// IN   a3
STI R032 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F032 R000				// Flushing Registers- a3
SUB R000 1 R000
STF F032 R000				// Pushing parameter- a3
SUB R000 1 R000
STI R032 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label2 R004
STI R004 R000
SUB R000 1 R000
JMP Label0
Label2: MOVI R002 R006				// Getting return Value
ADD R000 1 R000
LDF R000 F032				// Loading Back Registers:a3
ADD R000 1 R000
LDI R000 R032				// Loading Back Registers:a2
JMP EventMStart
//
begin: MOVI 10000 R000
MOVI 0 R006				// ASSIGN 0  res
MOVS "\n" R007				// ASSIGN "\n"  s
IN R032
IN R032
IN R032
EventMStart: IN R032
JMPC GT 64 R032 EventMOut
JMPC EQ 97 R032 eventLabel_a
JMP EventMStart
EventMOut: PRTS "\nDone\n"
