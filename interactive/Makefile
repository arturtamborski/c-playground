TARGET  = $(notdir $(CURDIR))
LIBRARY = libapp

SRCEXT  = .c
INCEXT  = .h
OBJEXT  = .o
LIBEXT  = .so

SRCDIR  = src
INCDIR  = src
LIBDIR  = lib
OBJDIR  = bin

CC      = gcc
LD      = gcc

LDFLAGS = -Wl,-no-as-needed -ldl
CCFLAGS = -std=gnu99 -g -ggdb -Og -Wall -Wextra -pedantic

SRCTREE = $(shell find $(SRCDIR) -type d)
LIBS    = $(shell find $(LIBDIR) -type f -name '*$(SRCEXT)')
INCS    = $(shell find $(INCDIR) $(LIBDIR) -type f -name '*$(INCEXT)')
SRCS    = $(shell find $(SRCDIR) -not -path $(LIBDIR) -type f -name '*$(SRCEXT)')
OBJTREE = $(foreach D,$(SRCTREE),$(shell echo $(D) | sed 's/$(SRCDIR)/$(OBJDIR)/'))
OBJSTMP = $(foreach F,$(SRCS),$(shell echo $(F) | sed -e 's/$(SRCDIR)/$(OBJDIR)/'))
OBJS    = $(foreach F,$(OBJSTMP),$(shell echo $(F) | sed -e 's/\$(SRCEXT)/\$(OBJEXT)/'))

all: $(LIBRARY)$(LIBEXT) $(TARGET)
	@echo Done.

run: $(LIBRARY)$(LIBEXT) $(TARGET)
	@./$(TARGET)

clean:
	@rm -r $(TARGET) $(LIBRARY)$(LIBEXT) $(OBJS) $(OBJDIR) 2>/dev/null || true

$(TARGET): $(OBJS) | $(OBJDIR)
	@$(LD) $(LDFLAGS) -L$(OBJDIR) -L$(LIBDIR) -o $@ $^

$(OBJS): $(OBJDIR)/%$(OBJEXT) : $(SRCDIR)/%$(SRCEXT) | $(OBJDIR)
	@$(CC) $(CCFLAGS) -I$(INCDIR) -c -o $@ $?

$(LIBRARY)$(LIBEXT) : $(LIBS)
	@$(CC) $(CCFLAGS) -I$(INCDIR) -L$(OBJDIR) -L$(LIBDIR) -rdynamic -fpic -shared -o $@ $^

$(OBJDIR):
	@mkdir -p $(OBJDIR) $(OBJTREE)

.PHONY: all run clean
