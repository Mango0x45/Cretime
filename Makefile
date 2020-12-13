CC=gcc
CFLAGS=-Ofast -lm -std=c99 -pedantic -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition
FILES=src/cretime.c
MANDIR?=$(DESTDIR)$(PREFIX)/man
PREFIX=/usr/local

ifndef NAME
	NAME=cretime
endif

install:
	$(CC) -o $(NAME) $(FILES) $(CFLAGS)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(NAME) $(DESTDIR)$(PREFIX)/bin/$(NAME)
	cp src/shretime.sh $(DESTDIR)$(PREFIX)/bin/shretime
	chmod +x $(DESTDIR)$(PREFIX)/bin/shretime
	mkdir -p $(MANDIR)/man1
	cp manpages/cretime.1 $(MANDIR)/man1/cretime.1
	cp manpages/shretime.1 $(MANDIR)/man1/shretime.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(NAME)
	rm -f $(DESTDIR)$(PREFIX)/bin/shretime
	rm -f $(MANDIR)/man1/cretime.1
	rm -f $(MANDIR)/man1/shretime.1

clean:
	rm -f $(NAME)
