# The source directory
SRC_DIR := src

# The object file directory
OBJ_DIR := obj

# The output directory
OUT_DIR := out

# The source files to be compiled
SRC_FILES := hello_sdl.cpp

# The object files that the source files will be compiled into
OBJ_FILES := hello_sdl.o

# The output file that the program will create after compiling everything
OUT_FILE := hello_sdl.o

# The C++ compiler
# The '@' symbol just silences the line, meaning that the command will not be echoed to the console
CC := @g++

# The flags to be passed to the C compiler (the $CC)
# -w -> supresses all warnings
COMPILER_FLAGS := -w

# The libraries that our executable is being linked against
LIBRARIES := -l SDL2

# Some miscallenous commands which will prove useful later (if ever)
CP := @cp
ECHO := @echo

# The default target that compiles the entire project
all: compile run

compile : $(SRC_DIR)/$(SRC_FILES)
	$(CC) $(SRC_DIR)/$(SRC_FILES) $(COMPILER_FLAGS) $(LIBRARIES) -o $(OBJ_DIR)/$(OBJ_FILES)
	$(CP) obj/hello_sdl.o out/rosewaltz_journey.o
	$(ECHO) "Compilation done."

run:
	$(ECHO) "Running project..."
	@./out/rosewaltz_journey.o