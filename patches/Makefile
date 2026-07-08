TARGET = patches.elf

CC      ?= clang
LD      ?= ld.lld

CFLAGS   := -target mips -mips2 -mabi=32 -O2 -G0 -mno-abicalls -mno-odd-spreg -mno-check-zero-division \
			-fomit-frame-pointer -ffast-math -fno-unsafe-math-optimizations -fno-builtin-memset \
			-Wall -Wextra -Wno-incompatible-library-redeclaration -Wno-unused-parameter -Wno-unknown-pragmas -Wno-unused-variable -Wno-missing-braces -Wno-unsupported-floating-point-opt
CPPFLAGS := -nostdinc -D_LANGUAGE_C -DMIPS -I dummy_headers -I ../lib/mm-decomp/include -I ../lib/mm-decomp/src -I ../lib/mm-decomp/assets -I../lib/rt64/include
LDFLAGS  := -nostdlib -T patches.ld -T syms.ld -Map patches.map --unresolved-symbols=ignore-all --emit-relocs

C_SRCS := $(wildcard *.c)

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

ifeq ($(ZELDA_ANDROID_BUILTIN_DPAD),1)
CPPFLAGS += -DZELDA_ANDROID_BUILTIN_DPAD=1
else
C_SRCS := $(filter-out dpad_builtin.c,$(C_SRCS))
endif

ifeq ($(ZELDA_ANDROID_BUILTIN_PMM),1)
CPPFLAGS += -DZELDA_ANDROID_BUILTIN_PMM=1 -DYAZMT_PMM_NO_API_IMPORTS -DLOGGER_PROJECT_NAME=\"PlayerModelManager\" \
	-I compat_pmm/include -I compat_pmm/deps/CoreLib/include -I compat_pmm/deps/GlobalObjects/include \
	-I compat_pmm_fsmodels/include \
	-I compat_pmm/include/dummy_headers -I compat_pmm/include/dummy_headers/assets -I compat_pmm/include/mm \
	-idirafter compat_pmm/include/libc -idirafter ../lib/mm-decomp/include/libc \
	-include compat_pmm/pmm_build_shims.h
C_SRCS += $(call rwildcard,compat_pmm/src,*.c)
C_SRCS += $(call rwildcard,compat_pmm/deps/CoreLib/src,*.c)
C_SRCS += $(call rwildcard,compat_pmm/deps/GlobalObjects/src,*.c)
C_SRCS += $(call rwildcard,compat_pmm_fsmodels/src,*.c)
C_SRCS := $(filter-out compat_pmm_fsmodels/src/oot_objects.c compat_pmm_fsmodels/src/oot_equipment.c,$(C_SRCS))
endif

ifeq ($(ZELDA_ANDROID_BUILTIN_CHEATS),1)
CPPFLAGS += -DZELDA_ANDROID_BUILTIN_CHEATS=1 -I . -I compat_cheats/include
C_SRCS += $(call rwildcard,compat_cheats/src,*.c)
endif

ifeq ($(ZELDA_ANDROID_BUILTIN_BOMB_ARROWS),1)
CPPFLAGS += -DZELDA_ANDROID_BUILTIN_BOMB_ARROWS=1 -I . -I compat_bomb_arrows/include
C_SRCS += $(call rwildcard,compat_bomb_arrows/src,*.c)
endif

ifeq ($(ZELDA_ANDROID_BUILTIN_MCO),1)
CPPFLAGS += -DZELDA_ANDROID_BUILTIN_MCO=1 -I . -I compat_mco/include -I compat_mco/include/dummy_headers \
	-include compat_mco/mco_build_shims.h
C_SRCS += $(call rwildcard,compat_mco/src,*.c)
endif

compat_pmm_fsmodels/%.o: CPPFLAGS += -UYAZMT_PMM_NO_API_IMPORTS

C_OBJS := $(C_SRCS:.c=.o)
C_DEPS := $(C_SRCS:.c=.d)

$(TARGET): $(C_OBJS) patches.ld syms.ld
	$(LD) $(C_OBJS) $(LDFLAGS) -o $@

$(C_OBJS): %.o : %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -MMD -MF $(@:.o=.d) -c -o $@

clean:
	rm -rf $(C_OBJS) $(TARGET) $(C_DEPS)

-include $(C_DEPS)

.PHONY: clean
