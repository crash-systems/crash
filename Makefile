# ↓ Compiler configuration
include $/base-config.mk

SRC := $(shell find src -type f -name "*.c")

#? (default): Build the nanotekspice
.PHONY: _start all
_start: all

# call mk-profile bin-name, profile
define mk-profile

out_$(strip $2) := $1
obj_$(strip $2) := $$(SRC:%.c=$$(BUILD)/$(strip $2)/%.o)

$$(BUILD)/$(strip $2)/%.o: %.c
	@ mkdir -p $$(dir $$@)
	$$(COMPILE.c) $$< -o $$@
	@ $$(log) "$$(cyan) CC $$(purple) $$(notdir $$@) $$(reset)"

$$(out_$(strip $2)): CFLAGS += $$(CFLAGS_$(strip $2))
$$(out_$(strip $2)): $$(obj_$(strip $2))
	$$(LINK.c) $$^ $$(LDFLAGS) $$(LDLIBS) -o $$@
	@ $$(log) "$$(cyan) CC $$(green) $$(notdir $$@) $$(reset)"

every_o += $$(obj_$(strip $2))
every_bin += $$(out_$(strip $2))

endef

#? crash: Build the shell
#? debug: Build a developer version
#? tests: Build the test suite
$(eval $(call mk-profile, crash, release))
$(eval $(call mk-profile, debug, debug))
$(eval $(call mk-profile, test_suite, tests))

.PHONY: all
all: $(out_release)

.PHONY: bundle #? bundle: build all afformentioned binaries
bundle: $(every_bin)

.PHONY: tests_run
tests_run: $(out_tests)
	- ./$^

.PHONY: cov #? cov: Run test suite and report coverage metrics
cov: cov_base_flags := --exclude-unreachable-branches
cov: cov_base_flags += --exclude tests --exclude-directories .direnv
cov: tests_run
	gcovr . $(cov_base_flags)
	gcovr . $(cov_base_flags) --txt-metric branch
	gcovr . --html-details html/

.PHONY: clean
clean: #? clean: Remove generated compiled files
	$(RM) $(every_o)

.PHONY: fclean
fclean: clean #? clean: Remove generated compiled file
	$(RM) $(every_bin)

.PHONY: re #? clean: Remove generated compiled file
re:	fclean all
.NOTPARALLEL: re

.PHONY: mrproper #? mrproper: (almost) Full repository cleanup
mrproper: fclean
	$(RM) -r $(BUILD)
	$(RM) -r .cache result $(CC_JSON)

.PHONY: help
help: #? help: Show this help message
	@ grep -P "#[?] " $(MAKEFILE_LIST)          \
      | sed -E 's/.*#\? ([^:]+): (.*)/\1 "\2"/' \
	  | xargs printf "%-12s: %s\n"

#? install: package within the provided dir
.PHONY: install
install: $(out_release)
	install -Dm755 -t $(PREFIX)/bin $(out_release)

$(eval $(call mk-profile, afl_runner, afl))

#? afl: compile fuzzer binary
.PHONY: afl
afl: CC := AFL_USE_ASAN=1 afl-gcc-fast
afl: $(out_afl)

AFL_FLAGS := -i afl/input
AFL_FLAGS += -x afl/tokens
AFL_FLAGS += -o afl/generated

define newline


endef

AFL_PROCS ?= $(shell nproc)

.PHONY: afl_run
afl_run: afl
	@ mkdir -p afl/generated
	screen -dmS main_instance \
		afl-fuzz $(AFL_FLAGS) -M fuzzer_1 -- ./afl_runner
	$(foreach instance, $(shell seq 1 $(AFL_PROCS)),\
		screen -dmS afl_$(instance) \
		afl-fuzz $(AFL_FLAGS) -S fuzzer_$(instance) -- ./afl_runner$(newline))
	watch -n 0.25 -- afl-whatsup -s afl/generated


ifneq ($(shell command -v tput),)
  ifneq ($(shell tput colors),0)

mk-color = \e[$(strip $1)m

reset := $(call mk-color, 00)

red := $(call mk-color, 31)
green := $(call mk-color, 32)
yellow := $(call mk-color, 33)
blue := $(call mk-color, 34)
purple := $(call mk-color, 35)
cyan := $(call mk-color, 36)

  endif
endif

NOW = $(shell date +%s%3N)
STIME := $(call NOW)
TIME_NS = $(shell expr $(call NOW) - $(STIME))
TIME_MS = $(shell expr $(call TIME_NS))

BOXIFY = "[$(blue)$(1)$(reset)] $(2)"

ifneq ($(shell uname),Darwin)
log = printf $(call BOXIFY, %6s , %b\n) "$(call TIME_MS)"
else
log = printf "%b\n"
endif
