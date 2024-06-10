UNAME := $(shell uname)

ARTIFACTS		= ${PWD}/artifacts
DIST			= ${PWD}/dist
 
# Library names
XML_LIB 		= saf_xml
SMO_CONN 		= irrsmo00_conn
SMO_LIB 		= irrsmo64

# Directory Paths
IRRSMO00_SRC	= ${PWD}/src/irrsmo00
IRRSEQ00_SRC	= ${PWD}/src/irrseq00
IRRSMO64_TST	= ${PWD}/tests/irrsmo64
EXTERNALS		= ${PWD}/externals

ifeq ($(UNAME), OS/390)
	AS 			= as
	CC 			= ibm-clang
	CXX 		= ibm-clang++

	ASFLAGS		= -mGOFF -I$(IRRSEQ00_SRC)
	CFLAGS		= -m64 -fzos-le-char-mode=ascii -I $(IRRSEQ00_SRC) -I $(EXTERNALS)
  	CPPFLAGS 	= -std=c++11 -m64 -fzos-le-char-mode=ascii -I $(EXTERNALS)
	LDFLAGS		= -m64 -Wl,-b,edit=no

	REQTEST		=
else
	CC 			= gcc
	CXX 		= g++

	CFLAGS  	= -g -Wall -std=c++11
	CPPFLAGS 	= -fpic -c -D_XOPEN_SOURCE_EXTENDED -std=c++11 -m64
	LDFLAGS		= -shared -Wl -m64

	REQTEST		= SMO64_TEST
endif

RM				= rm -rf

all: clean mkdirs smo

mkdirs:
	mkdir $(ARTIFACTS)
	mkdir $(DIST)

SMO64_TEST:	
	$(CXX) -c $(IRRSMO64_TST)/$(SMO_LIB).cpp -o $(ARTIFACTS)/$(SMO_LIB).o    

smo: clean mkdirs $(REQTEST)
	cd $(ARTIFACTS) \
		&& $(CXX) $(CPPFLAGS) $(IRRSMO00_SRC)/*.cpp
	$(CXX) $(LDFLAGS) $(ARTIFACTS)/*.o -o $(DIST)/$(SMO_CONN).dll

extract: clean mkdirs
	$(AS) $(ASFLAGS) -o $(ARTIFACTS)/irrseq00.o $(IRRSEQ00_SRC)/irrseq00.s
	cd $(ARTIFACTS) && $(CXX) -c $(CFLAGS) $(IRRSEQ00_SRC)/*.cpp
	$(CXX) $(LDFLAGS) -o $(DIST)/extract $(ARTIFACTS)/*.o 

clean:
	$(RM) $(ARTIFACTS) $(DIST) extract.bin
