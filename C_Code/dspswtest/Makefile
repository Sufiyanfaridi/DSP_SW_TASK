CC = gcc
CFLAGS = -Wall -Wextra -I./inc
LIBS = -lm
SRC_FILES = src/main.c src/task1.c src/task2.c src/task3.c 
OBJ_FILES = $(SRC_FILES:.c=.o)
EXECUTABLE = dspswtest

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
    # Use del on Windows, rm on Unix-based systems
    ifdef SystemRoot
        del /Q /F obj\* $(EXECUTABLE).exe
    else
		rm -rf obj/* $(EXECUTABLE) $(OBJ_FILES)
    endif

.PHONY: all clean
