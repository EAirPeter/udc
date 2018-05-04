CXXFLAGS := -Wall -Wextra -std=c++17 -flto -ggdb
LIBS := -flto -ggdb

BISONOBJ += Parser.o
FLEXOBJ += Scanner.o
CXXOBJ += Driver.o Main.o

OBJ := $(BISONOBJ) $(FLEXOBJ) $(CXXOBJ)

BISONCPP := $(addsuffix .cpp, $(basename $(BISONOBJ)))
BISONHPP := $(addsuffix .hpp, $(basename $(BISONOBJ)))
FLEXCPP := $(addsuffix .cpp, $(basename $(FLEXOBJ)))
CXXCPP := $(addsuffix .cpp, $(basename $(CXXOBJ)))

all: udc
	#rm -f $(BISONCPP) $(BISONHPP) $(FLEXCPP)

keep: udc

udc: $(OBJ)
	g++ $(LIBS) -o $@ $^

$(OBJ): %.o: %.cpp %.hpp
	g++ $(CXXFLAGS) -c -o $@ $<

$(BISONCPP): %.cpp: %.ypp
	bison -o $@ $<

$(FLEXCPP): %.cpp: %.l
	flex -o $@ $<

$(BISONHPP): %.hpp: %.ypp
$(FLEXCPP): $(BISONHPP)
$(CXXCPP): $(BISONHPP)

.PHONY: clean

clean:
	rm -f udc
	rm -f $(OBJ) $(BISONCPP) $(BISONHPP) $(FLEXCPP)
	rm -f location.hh position.hh stack.hh
