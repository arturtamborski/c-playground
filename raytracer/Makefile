TARGET  = $(notdir $(CURDIR))

SRCEXT 	= .c
INCEXT  = .h
OBJEXT  = .o

SRCDIR  = src
INCDIR  = src
OBJDIR  = bin

CC      = gcc
LD 	= gcc 

LDFLAGS = -lm
CCFLAGS = -std=c11 -g -ggdb -Og -Wall -Wextra -pedantic -Wno-missing-braces

SRCTREE	= $(shell find $(SRCDIR) -type d)
INCS	= $(shell find $(INCDIR) -type f -name '*$(INCEXT)')
SRCS	= $(shell find $(SRCDIR) -type f -name '*$(SRCEXT)')
OBJTREE = $(foreach D,$(SRCTREE),$(shell echo $(D) | sed 's/$(SRCDIR)/$(OBJDIR)/'))
OBJSTMP	= $(foreach F,$(SRCS),$(shell echo $(F) | sed -e 's/$(SRCDIR)/$(OBJDIR)/'))
OBJS	= $(foreach F,$(OBJSTMP),$(shell echo $(F) | sed -e 's/\$(SRCEXT)/\$(OBJEXT)/'))


all: $(TARGET)
	@echo Done.

run: $(TARGET)
	@./$(TARGET)

clean:
	@rm -r $(TARGET) $(OBJS) $(OBJDIR) 2>/dev/null || true

$(TARGET): $(OBJS) | $(OBJDIR) 
	@$(LD) $(LDFLAGS) -L$(OBJDIR) -o $@ $^

$(OBJS): $(OBJDIR)/%$(OBJEXT) : $(SRCDIR)/%$(SRCEXT) | $(OBJDIR)
	@$(CC) $(CCFLAGS) -I$(INCDIR) -c -o $@ $?

$(OBJDIR):
	@mkdir -p $(OBJDIR) $(OBJTREE)

.PHONY: all run clean
