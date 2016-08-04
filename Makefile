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

# Use 'make V=1' to see the full commands
# Use 'make help' to see Usage
ifeq ($(V), 1)
	S=
else
	S=@
endif

.PHONY: clean distclean

TARGET= FileSystem

OBJDIR= Objects/
OBJ_NAMES= app.o fs.o vfs.o
VPATH=Source/  Objects/ Include/

OBJECTS=$(addprefix $(OBJDIR),$(OBJ_NAMES))


CFLAGS= -Wall
INCLUDE = -I Include/



all: $(TARGET)



$(TARGET):$(OBJECTS)
	$(S) $(CC)  $^ -o $(TARGET)
	$(S) echo "Compiled successfully"


#For this kind of .o.c rule VPATH need to be there
${OBJDIR}%.o:%.c  
	 $(S) $(CC)  $(INCLUDE) $(CFLAGS) -c $< -o ${OBJDIR}$*.o
	 $(S) echo "GEN   $(PWD)/$@"

TAGS:
	find . -iname '*.c' -o -iname '*.cpp' -o -iname '*.h' -o -iname '*.hpp' > cscope.files
	cscope -b -i cscope.files -f cscope.out

#	ctags -R


clean:
	rm -rf $(OBJECTS) $(TARGET)

distclean:
	rm -f `find . -iname 'cscope.*' -o -iname 'tags' -type f`
	rm -rf $(OBJECTS) $(TARGET)

