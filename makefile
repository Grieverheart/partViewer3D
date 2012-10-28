SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c, bin/%.o, $(SRC))
OBJ+=bin/displaylistPrimitive.o
EXE=partViewer.exe

CC=gcc
CXX=g++
CFLAGS=-Wall -O3 -g -std=c99
CXXFLAGS=-Wall -O3 -g
LDFLAGS=-L"$(CURDIR)" -lglu32 -lfreeglut -lopengl32 -lfftw3f-3 -lopencsg -lglew32
RM=del /q

vpath %.o bin/

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

bin/displaylistPrimitive.o: src/displaylistPrimitive.cpp include/displaylistPrimitive.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: all
all: $(EXE)
	@echo Done

$(EXE): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@
	
.PHONY: clean
clean:
	-$(RM) bin\*
	@echo Clean Done!
