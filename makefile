SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c, bin/%.o, $(SRC))
EXE=partViewer.exe

CC=gcc
CFLAGS=-Wall -O3 -std=c99
LDFLAGS=-lglu32 -lfreeglut -lopengl32
RM=del /q

vpath %.o bin/

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all
all: $(EXE)
	@echo Done

$(EXE): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@
	
.PHONY: clean
clean:
	-$(RM) bin\*
	@echo Clean Done!
