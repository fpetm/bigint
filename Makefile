CXX?=clang++
INCS=
LIBS=-lm

CXXFLAGS=-g -Og $(INCS) -std=c++20
LDFLAGS=$(LIBS)

SRC=bigint.cpp
HEADER=
OBJ=$(SRC:.cpp=.o)

all: bigint

%.cpp.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $<

$(OBJ): $(HEADER)

bigint: $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) bigint
