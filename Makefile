CC = gcc
CFLAGS=-Wall -g -pg
PROGS=swapoff
DIST = Makefile mt19937-cokus.c rijndael.c swapoff.1 swapoff.c tiger.c
VERSION =0.01

srcdir = .

all: $(PROGS)

swapoff: swapoff.o mt19937-cokus.o rijndael.o tiger.o
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f *.o swapoff core

FTPDIR   = .
FTP_DIST = $(FTPDIR)/swapoff-$(VERSION).tar.gz     

# build zsh distribution
distdir = ./swapoff-$(VERSION)
dist: $(DIST)
	@echo "Copying distribution files in $(srcdir)" ; \
	echo "$(distdir)" > distname
	(test -d $(distdir) || mkdir -p $(distdir)) || exit 1 ; \
	for file in $(DIST); do    \
	  cp -p $$file $(distdir); \
	done; \
	tar cf - $(distdir)/* | gzip -9 > $(FTP_DIST)
	rm -Rf $(distdir)  
	@echo "Done building distribution"

md5sum:
	cd $(FTPDIR) ; md5sum *.gz > MD5SUM

$(FTPDIR)/zsh-doc.tar.gz: $(srcdir)/Doc/zsh.texi
	if test -d Doc/zsh-doc ; then \
	        (cd Doc ; \
	        make everything ; \
	        mv zsh*.html zsh*.ps zsh.dvi zsh.info* zsh-doc) ; \
	        (cd Doc ; tar cf - zsh-doc) | gzip > $@ ; \
	fi

