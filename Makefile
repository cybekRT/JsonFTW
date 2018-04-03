CFLAGS		= -std=gnu++1y -Wall -Wextra -Werror
CXXFLAGS	= $(CFLAGS)

libJsonFTW.a: Document.o
	$(AR) cr $@ $<

clean:
	$(RM) *.o *.so

all: libJsonFTW.a
	
.PHONY: clean all