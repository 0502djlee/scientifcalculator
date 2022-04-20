TARGET = calc

OBJECTS =  \
        Expression.o \
	ExpressionParser.o

OBJECTS_MAIN =  \
	ExpressionMain.o

SRCS = Expression.cpp ExpressionParser.cpp
SRC_MAIN = ExpressionMain.cpp

INCFLAGS = -I.
CFLAGS =

LIBS	= -lm

EXTRA_LFLAGS =

all: $(TARGET) 

.cpp.o:
	g++ -g -c $(INCFLAGS) -o $@ $<

#
# Target Build Rules.  Here you need to specify how to build each target.
#

calc: $(OBJECTS) $(OBJECTS_MAIN)
	g++ -g -o calc $(OBJECTS) $(OBJECTS_MAIN) $(EXTRA_LFLAGS) $(LIBS)


# More boilerplate stuff.
clean::
	rm -rf $(OBJECTS) $(OBJECTS_MAIN) $(TARGET)
#include *.d
