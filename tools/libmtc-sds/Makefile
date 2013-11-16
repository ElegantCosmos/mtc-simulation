CORELIB_NAME=libmtc-sds.so
CORELIB_FILES=libmtc-sds.cpp
CORELIB_OBJECTS	= $(CORELIB_FILES:%.cpp=%.o)

CORELIB_COMPILE_FLAGS= -Wall -fPIC -O2 `root-config --cflags --glibs`
CORELIB_LIBS=  `root-config --cflags --glibs`
CORELIB_LINK_FLAGS= -shared -Wl,-soname,$(CORELIB_NAME) $(CORELIB_LIBS) -o $(CORELIB_NAME)
CPP=g++
MAKE=make

all: slib

%.o: %.cpp
	$(CPP) $(CORELIB_COMPILE_FLAGS) -c -o $@  $< $(CXXFLAGS)

slib: $(CORELIB_OBJECTS)
	$(CPP) $(CORELIB_LINK_FLAGS) $(CORELIB_OBJECTS)

test:  $(CORELIB_OBJECTS) main.cpp
	g++ main.cpp -Wl,--rpath,. $(CORELIB_OBJECTS) $(CORELIB_LIBS) -o testbench

install: slib
	mkdir -p /usr/local/include/mtc
	cp libmtc-sds.hpp /usr/local/include/mtc
	cp sds-structures.hpp /usr/local/include/mtc
	cp $(CORELIB_NAME) /usr/local/lib/

.PHONY	: clean 

clean:
	rm -rf $(CORELIB_OBJECTS) $(CORELIB_NAME) testbench libmtc-sdc.tar.gz test.root

dist:
	tar -pczf libmtc-sdc.tar.gz *.cpp *.hpp Makefile

