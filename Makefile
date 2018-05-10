export BISON_PKGDATADIR=$(PWD)/bison

CXXFLAGS := -Wall -Wextra -std=c++17 -flto -ggdb
LIBS := -flto -ggdb

BISONOBJ += Parser.o
FLEXOBJ += Scanner.o
CXXOBJ += Driver.o IndentHelper.o Main.o
ASTCLASS += Base
ASTCLASS += ClassDef FnDef Program VarDef
ASTCLASS += AssignExpr BinaryExpr CallExpr CastExpr NewArrayExpr NewClassExpr UnaryExpr
ASTCLASS += BoolLit IntLit NullLit StrLit
ASTCLASS += ArrayAccess InstanceOf ReadInteger ReadLine This VarAccess
ASTCLASS += BlockStmt BreakStmt ExprStmt ForStmt IfStmt PrintStmt ReturnStmt WhileStmt
ASTCLASS += ArrayType BoolType ClassType IntType StrType VoidType

BISONCPP := $(addsuffix .cpp, $(basename $(BISONOBJ)))
BISONHPP := $(addsuffix .hpp, $(basename $(BISONOBJ)))
FLEXCPP := $(addsuffix .cpp, $(basename $(FLEXOBJ)))
CXXCPP := $(addsuffix .cpp, $(basename $(CXXOBJ)))
ASTOBJ := $(addprefix ast/, $(addsuffix .o, $(ASTCLASS)))
ASTCPP := $(addprefix ast/, $(addsuffix .cpp, $(ASTCLASS)))

OBJ := $(BISONOBJ) $(FLEXOBJ) $(CXXOBJ) $(ASTOBJ)

all: udc

keep: udc

udc: $(OBJ)
	g++ $(LIBS) -o $@ $^

$(OBJ): %.o: %.cpp %.hpp
	g++ $(CXXFLAGS) -c -o $@ $<

$(BISONCPP): %.cpp: %.ypp
	bison -Wall -o $@ $<

$(FLEXCPP): %.cpp: %.l
	flex -o $@ $<

$(BISONHPP): %.hpp: %.cpp

$(FLEXCPP): $(BISONHPP)
$(CXXCPP): $(BISONHPP)
$(ASTCPP): $(BISONHPP)

.PHONY: clean

clean:
	rm -f udc
	rm -f $(OBJ) $(BISONCPP) $(BISONHPP) $(FLEXCPP)
	rm -f location.hh position.hh stack.hh
