TARGETS = all clean

SUBDIRS = $(wildcard */.)

$(TARGETS): $(SUBDIRS)
	@echo Done.

$(SUBDIRS):
	@$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TARGETS) $(SUBDIRS)
