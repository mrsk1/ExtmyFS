################################################################################
#
#
#
#  Author :  Karhik M
#  Date   :  20-Nov-2015
#
#  History
#  ----------------------------------------------------------------------------
#  1-Aug-2016   karthik M             Adding Basic Makefile
#
###############################################################################

LDFLAGS= -w

fsout:
	gcc $(LDFLAGS) app.c  fs.c vfs.c

clean:
	rm -rf $(OBJECTS) $(TARGET_APP)

distclean:
	rm -f `find . -iname 'cscope.*' -o -iname 'tags' -type f`
	rm -rf $(OBJECTS) $(TARGET_APP)
TAGS:
	find . -iname '*.c' -o -iname '*.cpp' -o -iname '*.h' -o -iname '*.hpp' > cscope.files
	cscope -b -i cscope.files -f cscope.out
	ctags -R

