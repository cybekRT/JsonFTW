CFLAGS		= -std=gnu++1y -Wall -Wextra 
#-Werror
CXXFLAGS	= $(CFLAGS)

libJsonFTW.a: Document.o
	$(AR) cr $@ $<
	
Document.o: Document.cpp Document.hpp Exception.hpp JsonFTW.hpp Value.hpp
	g++ -c -o $@ $< $(CFLAGS)

clean:
	$(RM) *.o *.so lib*.a

all: libJsonFTW.a
	
.PHONY: clean all