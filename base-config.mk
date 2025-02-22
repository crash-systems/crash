KERNEL := $(shell uname)

/ ?= ./
BUILD := $/.build

ifeq ($(KERNEL),Darwin)
CC := clang
else
CC := gcc
endif
LD := $(CC)

AR ?= ar
RM ?= rm --force

CFLAGS := -std=c2x -pedantic
CFLAGS += -iquote $/include
CFLAGS += $(shell cat $/warning_flags.txt)

CFLAGS_release := -O2 -DNEBUG -fomit-frame-pointer
CFLAGS_bonus := $(CXXFLAGS_release)

CFLAGS_debug := -g3 -fsanitize=address,undefined -DCR_DEBUG_MODE=1
ifneq ($(KERNEL),Darwin)
	CFLAGS_debug += -fsanitize=leak
endif
CFLAGS_cov := --coverage -g3

LDLIBS :=
LDFLAGS :=

DEPS_FLAGS ?= -MMD -MP

MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-print-directory
