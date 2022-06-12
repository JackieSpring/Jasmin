

SRC_PATH := src/
SRC_PATH += src/shared/
SRC_PATH += src/arch/
SRC_PATH += src/arch/x86/


MAIN_SRC = $(foreach path, $(SRC_PATH), $(wildcard $(path)*.c) )
ADDITIONAL_SRC = test.c

SRC = $(ADDITIONAL_SRC) $(MAIN_SRC)

OUTPUT_PATH = dist/
OUTPUT_NAME = out
OUTPUT = $(OUTPUT_PATH)$(OUTPUT_NAME)

DEBUG_MODE = 1

ifeq ($(DEBUG_MODE),1)
	DEBUG_FLAG = -D DEBUG_ON=1
else
	DEBUG_FLAG = -w
endif

EXT_LIB_PATH = src/shared/external_include/

KEYSTONE_PATH = $(EXT_LIB_PATH)keystone/
CAPSTONE_PATH = $(EXT_LIB_PATH)capstone/

INC_KEYSTONE = -I $(KEYSTONE_PATH)include/
INC_KEYSTONE += -L $(KEYSTONE_PATH)lib/
INC_KEYSTONE += -l keystone

INC_CAPSTONE = -I $(CAPSTONE_PATH)include/
INC_CAPSTONE += -L $(CAPSTONE_PATH)lib/
INC_CAPSTONE += -l capstone

CC = gcc
CFLAGS = -lstdc++ -lm $(DEBUG_FLAG) $(foreach path, $(SRC_PATH), -I $(path)) $(INC_KEYSTONE) $(INC_CAPSTONE) -o $(OUTPUT)

FOO = ok

compile: SRC
	@$(CC) $(SRC) $(CFLAGS)

run: compile
	@./$(OUTPUT)
