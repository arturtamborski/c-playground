TARGET  = $(notdir $(CURDIR))

SRCDIR  = src
INCDIR  = src
OBJDIR  = bin

CC      = gcc
LD 	= gcc 

LFLAGS 	= -lasan -lm
CFLAGS  = -c -std=c99 -g -ggdb -Og -ftrapv -fsanitize=address -I$(INCDIR) \
	-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wunreachable-code \
	-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wswitch-enum \
	-Wredundant-decls -Wnested-externs -Winline -Wundef -Wshadow -Wpointer-arith \
	-Wuninitialized -Wconversion -Wstrict-prototypes -Wcast-align -Wswitch-default \
	-Wno-unused-parameter -Wno-missing-prototypes -Wno-missing-declarations \
	-Wformat-extra-args -Wformat-zero-length -Wformat=2 \
	-DDEBUG -Wno-unused-function

INCS	= $(shell find $(INCDIR) -type f -name '*.h')
SRCS	= $(shell find $(SRCDIR) -type f -name '*.c')
SRCTREE	= $(shell find $(SRCDIR) -type d)
OBJTREE = $(foreach D,$(SRCTREE),$(shell echo $(D) | sed 's/$(SRCDIR)/$(OBJDIR)/'))
OBJS	= $(foreach F,$(SRCS), $(shell echo $(F) | sed -e 's/$(SRCDIR)/$(OBJDIR)/' -e 's/\.c/\.o/'))

all: $(TARGET)
	@echo Done.

run: $(TARGET)
	@./$(TARGET)

clean:
	@rm -r $(TARGET) $(OBJDIR)

$(TARGET): $(OBJS) | $(OBJDIR) 
	@$(LD) $(LFLAGS) -o $@ $?

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.c | $(OBJDIR)
	@$(CC) $(CFLAGS) -o $@ $?

$(OBJDIR):
	@mkdir -p $(OBJDIR) $(OBJTREE)

.PHONY: all run clean
