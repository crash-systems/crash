/ ?= ./
BUILD := $/.build

CC := gcc
LD := $(CC)

AR ?= ar
RM ?= rm --force

CFLAGS := -std=c2x -pedantic
CFLAGS += -iquote $/include
CFLAGS += $(shell cat $/warning_flags.txt)

CFLAGS_release := -O2 -DNEBUG -fomit-frame-pointer -fanalyzer
CFLAGS_bonus := $(CXXFLAGS_release)

CFLAGS_debug := -g3 -fsanitize=address,leak,undefined -DCR_DEBUG_MODE=1
CFLAGS_cov := --coverage -g3

LDLIBS :=
LDFLAGS :=

DEPS_FLAGS ?= -MMD -MP

MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-print-directory
