.PHONY: clean help build
.DEFAULT_GOAL := help

C_SRC = $(wildcard *.c)
CXX_SRC = $(wildcard *.cc)

SOURCES = $(C_SRC) $(CXX_SRC)

C_EXECUTABLES = $(patsubst %.c,%,$(C_SRC))
CXX_EXECUTABLES = $(patsubst %.cc,%,$(CXX_SRC))

EXECUTABLES = $(C_EXECUTABLES) $(CXX_EXECUTABLES)
CC ?= clang
CXX = clang++

FLAGS = -Wall -Wextra -Werror -Wfatal-errors -pedantic

CFLAGS := -std=c11 ${C_FLAGS}
CXXFLAGS := -std=c++17 ${CXX_FLAGS}
LIBS ?= -lm
LFLAGS ?= 

define PRINT_HELP_PYSCRIPT
import re, sys

for line in sys.stdin:
        match = re.match(r'^([a-zA-Z_-]+):.*?## (.*)$$', line)
        if match:
                target, help = match.groups()
                print("%-20s %s" % (target, help))
endef
export PRINT_HELP_PYSCRIPT

help: ## print help info
	@python -c "$$PRINT_HELP_PYSCRIPT" < $(MAKEFILE_LIST)

clean: ## clean build objects and dist files
	@rm -f $(EXECUTABLES)

build: clean $(EXECUTABLES) ## builds all executables

list_sources: ## show what sources will be compiled
	@echo "The found sources are: $(SOURCES)"

%:%.c
	clang-tidy -checks="modernize-*;llvm-*" -fix -fix-errors $<
	clang-format --style="{ColumnLimit: 73}" -i $<
	$(CC) $(FLAGS) $(CFLAGS) $(LFLAGS) -o $@ $< $(LIBS)

%:%.cc
	clang-tidy -checks="modernize-*;llvm-*" -fix -fix-errors $<
	clang-format --style="{ColumnLimit: 73}" -i $<
	$(CXX) $(FLAGS) $(CXXFLAGS) $(LFLAGS) -o $@ $< $(LIBS)
