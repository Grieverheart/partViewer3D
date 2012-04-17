SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
EXE=partViewer.exe

CC=gcc
CFLAGS=-Wall -O3 -std=c99
LDFLAGS=-lglu32 -lfreeglut -lopengl32
RM=del

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: all
all: $(EXE)
	@echo Done

$(EXE): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@
	
.PHONY: clean
clean:
	-$(RM) $(OBJ)
	@echo Clean Done!
