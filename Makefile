CC = gcc
VERSION = 0.01
DISTRIBUTION = b
CFLAGS = -Wall -O3 -DVERSION=\"$(VERSION)\" -DDISTRIBUTION=\"$(DISTRIBUTION)\"
PROGS = swapoff
DIST = Makefile mt19937-cokus.c rijndael.c swapoff.c tiger.c swapoff.8 CHANGES README  

srcdir = .

all: $(PROGS) buildnr.h

swapoff: swapoff.o mt19937-cokus.o rijndael.o tiger.o
	$(CC) -o $@ $^
	strip $@
	rm buildnr.h

buildnr.h:
	sh -c "echo `cat build` + 1 | bc > build"
	echo "#define BUILD " `cat build` > buildnr.h
clean:
	rm -f *.o swapoff core

FTPDIR   = .
FTP_DIST = $(FTPDIR)/swapoff-$(VERSION)$(DISTRIBUTION).tar.gz     

distdir = ./swapoff-$(VERSION)$(DISTRIBUTION)
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
	scp ./$(FTP_DIST)  koeln.ccc.de:public_html/

md5sum:
	cd $(FTPDIR) ; md5sum *.gz > MD5SUM

$(FTPDIR)/zsh-doc.tar.gz: $(srcdir)/Doc/zsh.texi
	if test -d Doc/zsh-doc ; then \
	        (cd Doc ; \
	        make everything ; \
	        mv zsh*.html zsh*.ps zsh.dvi zsh.info* zsh-doc) ; \
	        (cd Doc ; tar cf - zsh-doc) | gzip > $@ ; \
	fi

