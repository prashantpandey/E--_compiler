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
JMPC GE 3 2 Label5				// JMPC LE 2 3  Label5 
Label6: JMPC GE 7 6 Label5				// JMPC LE 6 7  Label5 
Label5: JMPC GT R007 0 Label2				// JMPC GT g 0  Label2 
JMP Label3				// JMP Label3  
Label2: MOVI 3 R007				// Label2: ASSIGN 3  g
JMP Label4				// JMP Label4  
Label3: MOVI 4 R007				// Label3: ASSIGN 4  g
Label4: MOVI R001 R000				// Function Exit: Restoring BP
ADD R000 1 R000
LDI R000 R001				// Loading BP from stack
ADD R000 1 R000
LDI R000 R004				// Getting Return Address from stack
ADD R000 2 R000
JMPI R004
//
eventLabel_xyz: INI R007				// IN   a2
INF F007				// IN   a3
STI R007 R000				// Flushing Registers- a2
SUB R000 1 R000
STF F007 R000				// Flushing Registers- a3
SUB R000 1 R000
STF F007 R000				// Pushing parameter- a3
SUB R000 1 R000
STI R007 R000				// Pushing parameter- a2
SUB R000 1 R000
MOVL Label7 R004
STI R004 R000
SUB R000 1 R000
JMP Label0
Label7: ADD R000 1 R000
LDF R000 F007				// Loading Back Registers:a3
ADD R000 1 R000
LDI R000 R007				// Loading Back Registers:a2
JMP EventMStart
//
begin: MOVI 10000 R000
IN R007
IN R007
IN R007
EventMStart: IN R007
JMPC GT 64 R007 EventMOut
JMPC EQ 120 R007 eventLabel_xyz
JMP EventMStart
EventMOut: PRTS "\nDone\n"
