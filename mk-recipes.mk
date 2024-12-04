# $(eval $(call mk-recipe-binary, ...))

# $1 -> binary-name
# $2 -> src-list
# $3 -> extra-flags

_clean :=
_fclean :=

define mk-recipe-binary

obj-$(strip $1) := $$($(strip $2):%.c=$$(BUILD_DIR)/$(strip $1)/%.o)
out-$(strip $1) := $(strip $1)

$$(BUILD_DIR)/$(strip $1)/%.o: %.c
	@ mkdir -p $$(dir $$@)
		$$Q $$(CC) \
		$$(CFLAGS) $$(CFLAGS_@$$(notdir $$(@:.o=))) $3 \
		-o $$@ -c $$<
	@ $$(log) "CC $$(C_PURPLE) $$(notdir $$@) $$(C_RESET)"

$$(out-$(strip $1)): $$(obj-$(strip $1))
	@ mkdir -p $$(dir $$@)
	$$Q $$(CC) -o $$@ $$(obj-$(strip $1))              \
		$$(CFLAGS) $$(CFLAGS_@$$(notdir $$(@:.o=))) $3 \
		$$(LDLIBS) $$(LDFLAGS)
	@ $$(log) "LD $$(C_GREEN) $$@ $$(C_RESET)"

_clean += $$(obj-$(strip $1))
_fclean += $$(out-$(strip $1))

endef
