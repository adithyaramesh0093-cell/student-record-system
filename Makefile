# Student Record Management System - Makefile (Linux, macOS, or MSYS2/Git Bash)
# Windows users with plain Command Prompt: use build.bat instead.
#
#   make          build the program   -> build/student
#   make run      build and start it
#   make test     build and run the automated tests
#   make clean    delete the build folder

# (make's built-in default for CC is "cc", which MinGW/MSYS2 does not provide,
#  so we set it explicitly. Override with e.g. `make CC=clang`.)
CC       = gcc
CFLAGS   ?= -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS += -Iinclude

BUILD_DIR := build
EXE       :=
ifeq ($(OS),Windows_NT)
    EXE := .exe
endif

TARGET      := $(BUILD_DIR)/student$(EXE)
TEST_TARGET := $(BUILD_DIR)/run_tests$(EXE)

# Every .c file in src/ except main.c is a reusable "library" part; the program
# and the tests both link against it.
LIB_SRCS := $(filter-out src/main.c,$(wildcard src/*.c))
LIB_OBJS := $(LIB_SRCS:src/%.c=$(BUILD_DIR)/%.o)
APP_OBJS := $(BUILD_DIR)/main.o $(LIB_OBJS)

.PHONY: all run test clean

all: $(TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR)

$(TARGET): $(APP_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_TARGET): tests/test_main.c $(LIB_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

# -MMD -MP makes make rebuild a .c file when a header it includes changes.
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

-include $(wildcard $(BUILD_DIR)/*.d)
