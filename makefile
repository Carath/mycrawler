##########################################################
# Settings:

# Executable name:
EXE = exec

# Source and objects files location:
SRC_DIR = src
OBJ_DIR = obj

# Creates the OBJ_DIR folder, if necessary:
$(shell mkdir -p $(OBJ_DIR))

##########################################################
# Libraries:

##########################################################
# Compiler options:

# N.B: gcc for C, g++ for C++, alternative: clang.
CC = gcc
CPPFLAGS =
CFLAGS = -std=c99 -Wall -O2
LDFLAGS =
LDLIBS = -lm -lcurl

##########################################################
# Compiling rules:

# The following names are not associated with files:
.PHONY: all clean

# All executables to be created:
all: $(EXE)

# Sources and objects files:
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Linking the program:
$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compiling the source files:
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

##########################################################
# Cleaning with 'make clean' the object files:
clean:
	rm -fv $(EXE) $(OBJ_DIR)/*
