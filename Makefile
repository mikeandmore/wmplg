SUFFIX=.c .o
CFLAGS=`pkg-config x11 --cflags` -g -Wall -fPIC
LIBS=`pkg-config x11 --libs`
PREFIX=/usr/local

OBJS=\
	ewmh.o \
	wm.o

HDRS=\
	wm.h

phony: all

.c.o:
	gcc $(CFLAGS) $< -c

$(OBJS): $(HDRS)

all: $(OBJS)
	gcc -shared $(LIBS) $(OBJS) -o libwm.so.1 -W,-soname=libwm.so.1
	ln -sf libwm.so.1 libwm.so

test: all
	gcc test.c $(CFLAGS) $(LIBS) -o test -L. -lwm

clean:
	rm -f libwm.so.1 libwm.so
	rm -f $(OBJS)

install:
	install -m 644 libwm.so.1 $(PREFIX)/lib/
	ln -sf $(PREFIX)/lib/libwm.so.1 $(PREFIX)/lib/libwm.so

uninstall:
	rm $(PREFIX)/lib/libwm.so.1
	rm $(PREFIX)/lib/libwm.so
