# Useful directories variable
SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj
OUT_DIR := out

# The source files to be compiled
SRC_FILES := $(shell find $(SRC_DIR) -name "*.cpp")
INC_FILES := $(shell find $(INC_DIR) -name "*.hpp")

# The object files that the source files will be compiled into
OBJ_FILES := $(patsubst $(SRC_DIR)%.cpp, $(OUT_DIR)%.o, $(SRC_FILES))

# The output file that the program will create after compiling everything
OUT_FILE := rosewaltz_journey.out

# The C++ compiler
# The '@' symbol just silences the line, meaning that the command will not be echoed to the console
CC := @g++

# The flags to be passed to the C compiler (the $CC)
# -w -> supresses all warnings
COMPILER_FLAGS := -w -I $(INC_DIR)

# The libraries that our executable is being linked against
LIBRARIES := -l SDL2

# Some miscallenous commands which will prove useful later (if ever)
CP := @cp
ECHO := @echo

# The default target that compiles the entire project
all: compile run

compile : $(SRC_FILES)
	$(CC) $(SRC_FILES) $(COMPILER_FLAGS) $(LIBRARIES) -o $(OUT_FILE)
	$(ECHO) "Compilation done."

run:
	$(ECHO) "Running project..."
	@./$(OUT_FILE)