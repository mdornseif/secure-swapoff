bin_PROGRAMS = swapoff
swapoff_SOURCES = mt19937-cokus.c rijndael.c swapoff.c tiger.c

clean:
	$(RM) *~

install:
	@echo "Type 'make realinstall' if you really want to overwrite your" 
	@echo "system's swapoff"

realinstall:
	$(RM) /sbin/swapoff
	$(INSTALL) swapoff $(sbindir)/swapoff
	$(LN_S) $(sbindir)/swapoff /sbin/swapoff
	$(RM) /usr/bin/man/man8/swapoff.8
