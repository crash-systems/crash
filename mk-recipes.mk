# $(eval $(call mk-recipe-binary, ...))

# $1 -> binary-name
# $2 -> src-list
# $3 -> extra-flags

_clean :=
_fclean :=

%.h:

define mk-recipe-binary

obj-$(strip $1) := $$($(strip $2):%.c=$$(BUILD_DIR)/$(strip $1)/%.o)
dep-$(strip $1) := $$(obj-$(strip $1):%.o=%.d)

out-$(strip $1) := $(strip $1)

$$(BUILD_DIR)/$(strip $1)/%.d: %.c
	@ mkdir -p $$(dir $$@)
	$$Q $(CC) $(CFLAGS) -MM -MT '$$(patsubst %.d,%.o,$$@)' $$< -MF $$@
	@ $$(log) "$$(green)MT $$(purple)$$< $$(reset)"

-include $$(dep-$(strip $1))

$$(BUILD_DIR)/$(strip $1)/%.o: %.c $$(BUILD_DIR)/$(strip $1)/%.d
	@ mkdir -p $$(dir $$@)
		$$Q $$(CC) \
		$$(CFLAGS) $$(CFLAGS_@$$(notdir $$(@:.o=))) $3 \
		-o $$@ -c $$<
	@ $$(log) "$$(green)CC $$(purple)$$(notdir $$@) $$(reset)"

$$(out-$(strip $1)): $$(obj-$(strip $1))
	@ mkdir -p $$(dir $$@)
	$$Q $$(CC) -o $$@ $$(obj-$(strip $1)) \
		$$(CFLAGS) $$(CFLAGS_@$$(notdir $$(@:.o=))) $3 \
		$$(LDLIBS) $$(LDFLAGS)
	@ $$(log) "$$(green)LD $$(purple)$$@ $$(reset)"

_clean += $$(obj-$(strip $1))
_fclean += $$(out-$(strip $1))

endef
