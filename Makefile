CC = g++
CFLAGS = -c -g -Wno-deprecated
CXX = g++
CXXFLAGS = -c -g -O -Wno-deprecated -std=c++0x -Wall
FLEX = flex
BISON = bison

parser:   driveParse.o E--_lexer.o E--_parser.o RegMgr.o  STEClasses.o SymTabMgr.o Value.o Type.o SymTabEntry.o Error.o ParserUtil.o SymTab.o CodeGen.o Ast.o
	$(CXX) -o $@  $^ -lfl
assembler:	driver.o ICode_lexer.o ICode_parser.o ICode.tab.h 
	$(CC) -g -o  demo ICode_lexer.o ICode_parser.o driver.o -lfl


E--_lexer.o:    E--_lexer.C E--.tab.h
E--_parser.o:	E--_parser.C E--.tab.h

E--_parser.C: 	E--_parser.y++
	$(BISON) -t -d -v -o E--_parser.C E--_parser.y++; \
  mv E--_parser.H E--.tab.h

SymTabMgr.o: SymTabMgr.h SymTabMgr.C
SymTabEntry.o: SymTabEntry.h SymTabEntry.C
STEClasses.o: STEClasses.h STEClasses.C
Ast.o: Ast.h Ast.C
Value.o: Value.h Value.C
Type.o: Type.h Type.C
Error.o: Error.h Error.C
ParserUtil.o: ParserUtil.h ParserUtil.C
SymTab.o: SymTab.h SymTab.C
CodeGen.o: CodeGen.h CodeGen.C
RegMgr.o: RegMgr.h RegMgr.C

ICode_parser.o:	ICode_parser.C ICode.tab.h
	$(CC) $(CFLAGS) ICode_parser.C

ICode_lexer.o: ICode_lexer.C ICode.tab.h
	$(CC) -c $(CFLAGS) ICode_lexer.C

ICode_lexer.C:	ICode.l
	$(FLEX) -p -8 -Ce -oICode_lexer.C ICode.l

ICode_parser.C: ICode.y
	$(BISON) -t -d -v -o ICode_parser.C ICode.y; \
    mv ICode_parser.H ICode.tab.h

driver.o: driver.C ICode_parser.C
	$(CXX) -c $(CFLAGS) driver.C

clean:
	-echo "Removing all object files and compiled lexer and bison files!"
	-rm -f demo *.o ICode_parser.C ICode_lexer.C ICode.tab.h
	-echo "Removing intermediate C files!"
	-rm -f E--_parser.C
