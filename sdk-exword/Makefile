# Source-build libdataplus with the HHK3/Hollyhock SH4A compiler.
.DEFAULT_GOAL := all
CROSS_PREFIX ?= sh4a_nofpueb-elf-
CC := $(CROSS_PREFIX)gcc
AR := $(CROSS_PREFIX)ar
LIBDATAPLUS_SRC ?= .deps/libdataplus
SDK_DIR ?= .mini-sdk
SRC := $(abspath $(LIBDATAPLUS_SRC))
DEST := $(abspath $(SDK_DIR))
OBJ := $(DEST)/obj
LIB := $(DEST)/lib
INC := $(DEST)/include

DP_C := $(wildcard $(SRC)/src/util/*.c $(SRC)/src/util/fixedptc/*.c $(SRC)/src/syscall/*.c $(SRC)/src/libc/*.c)
DP_S := $(wildcard $(SRC)/src/libc/*.s)
GR_C := $(wildcard $(SRC)/src/graphics/*.c)
SH_C := $(wildcard $(SRC)/src/cpu/sh4a/input/*.c)
SH_S := $(wildcard $(SRC)/src/cpu/sh4a/*.s)
ASOBJ = $(patsubst $(SRC)/%.s,$(OBJ)/%.o,$(1))
COBJ = $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(1))
DP_O := $(call COBJ,$(DP_C)) $(call ASOBJ,$(DP_S))
GR_O := $(call COBJ,$(GR_C))
SH_O := $(call COBJ,$(SH_C)) $(call ASOBJ,$(SH_S))

CFLAGS := -std=gnu17 -O2 -ffreestanding -fno-builtin -ffunction-sections -Wall -Wextra -I$(SRC)/include -MMD -MP
.PHONY: all check clean
all: check $(INC)/.ready $(LIB)/libdataplus.a $(LIB)/libgraphics.a $(LIB)/libsh4a.a
check:
	@test -f "$(SRC)/src/graphics/drawing.c" || { echo "Missing libdataplus checkout"; exit 1; }
	@command -v $(CC) >/dev/null
	@$(CC) -dumpmachine
$(INC)/.ready:
	@mkdir -p "$(INC)"
	cp -a "$(SRC)/include/." "$(INC)/"
	@touch $@
$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ)/%.o: $(SRC)/%.s
	@mkdir -p "$(dir $@)"
	$(CC) -x assembler-with-cpp -c $< -o $@
$(LIB)/libdataplus.a: $(DP_O)
	@mkdir -p "$(LIB)"
	$(AR) rcs $@ $(DP_O)
$(LIB)/libgraphics.a: $(GR_O)
	@mkdir -p "$(LIB)"
	$(AR) rcs $@ $(GR_O)
$(LIB)/libsh4a.a: $(SH_O)
	@mkdir -p "$(LIB)"
	$(AR) rcs $@ $(SH_O)
clean:
	rm -rf "$(DEST)"
-include $(patsubst %.o,%.d,$(DP_O) $(GR_O) $(SH_O))
