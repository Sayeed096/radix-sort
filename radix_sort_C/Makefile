## ======================================================================== ##
## Declaration of compiler name, options and directories macros(variables)
## ======================================================================== ##
# Compiler Name and flags
CC = gcc
# CFLAGS = -g -Wall
CFLAGS = -g
# Source code, compiled object, program and other directory define
SRC = src
OBJ = obj
BIN = bin
ASM = asm
PPS = prepos

# Source code, compiled object and and program directory define
SRCS = $(wildcard $(SRC)/*.c)
HDRS = $(wildcard $(SRC)/*.h)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
ASMS = $(patsubst $(SRC)/%.c, $(ASM)/%.s, $(SRCS))
PPSS = $(patsubst $(SRC)/%.c, $(PPS)/%.i, $(SRCS))
MAIN = $(BIN)/rsort

## ======================================================================== ##
## Targates and actions for making program and some intermideate levels
## Main target run while run "make" cmd
## ======================================================================== ##
all: $(OBJ) $(BIN) $(MAIN)

# Compile to make a exectable binary file
$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Compile to make object file for each .c file(c code)
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Make new dir if not exist
$(OBJ):
	mkdir -p $@

$(BIN):
	mkdir -p $@

## ======================================================================== ##
## To generate Preprocessor code
## ======================================================================== ##
.PHONY: pps
pps: $(PPS) $(PPSS)

$(PPS):
	mkdir -p $@

$(PPS)/%.i: $(SRC)/%.c
	$(CC) $(CFLAGS) -E $< -o $@

## ======================================================================== ##
## To generate Assemble code
## ======================================================================== ##
.PHONY: asm
asm: $(ASM) $(ASMS)

$(ASM):
	mkdir -p $@

$(ASM)/%.s: $(SRC)/%.c
	$(CC) $(CFLAGS) -S $< -o $@

## ======================================================================== ##
## To run with details execution time info
## ======================================================================== ##
.PHONY: run
run:
	/usr/bin/time -v ./$(MAIN)

## ======================================================================== ##
## To run test (Not complete yet)
## ======================================================================== ##
.PHONY: test
test:
	$(info    [Makefile]Test is not implemented yet.)
	./$(MAIN)

## ======================================================================== ##
## Clean the object and binary program and oter temporary directories
## ======================================================================== ##
.PHONY: clean
clean:
	$(RM) -r $(OBJ) $(BIN) $(PPS) $(ASM)


## ======================================================================== ##
## ======================================================================== ##
#$(error   VAR is $(SH) shesh)
#$(warning   VAR is $(SH) shesh)
#$(info   VAR is $(SH) shesh)
