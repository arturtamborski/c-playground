CFLAGS  = -std=gnu99 -fPIC -Wall -Wextra -pedantic
LDLIBS  = -ldl

all : app libapp.so

app : main.c app.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

libapp.so : app.c app.h
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ $< $(LDLIBS)

test : app libapp.so
	./$<

clean :
	$(RM) *.o *.so app
