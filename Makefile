UNAME := $(shell uname)

ARTIFACTS		= ${PWD}/artifacts
DIST			= ${PWD}/dist
 
# Library names
XML_LIB 		= saf_xml
XML_CONN 		= xml_conn
SMO_CONN 		= irrsmo00_conn
SMO_LIB 		= irrsmo64

# Directory Paths
IRRSMO00_SRC	= ${PWD}/src/irrsmo00
IRRSEQ00_SRC	= ${PWD}/src/irrseq00
EXTERNALS		= ${PWD}/externals

ifeq ($(UNAME), OS/390)
	AS 			= as
	CC 			= ibm-clang
	CXX 		= ibm-clang++

	ASFLAGS		= -mGOFF -I$(IRRSEQ00_SRC)
	CFLAGS		= -m64 -fzos-le-char-mode=ascii -I $(IRRSEQ00_SRC) -I $(EXTERNALS)
  	CPPFLAGS 	= -std=c++11 -m64 -fzos-le-char-mode=ascii -I $(EXTERNALS)
	LDFLAGS		= -m64 -Wl,-b,edit=no
else
	# the compiler: gcc for C program, define as g++ for C++
	CC 			= gcc
	CXX 		= g++
	# compiler flags:
	#  -g     - this flag adds debugging information to the executable file
	#  -Wall  - this flag is used to turn on most compiler warnings
	CFLAGS  	= -g -Wall -std=c++11
	CPPFLAGS 	= -fpic -shared -std=c++11
endif

RM				= rm -rf

all: clean mkdirs smo

mkdirs:
	mkdir $(ARTIFACTS)
	mkdir $(DIST)
  
  $(XML_CONN): $(XML_LIB)
				$(CXX) $(CPPFLAGS) \
					$(IRRSMO00_SRC)/$(XML_CONN).cpp \
					$(ARTIFACTS)/$(XML_LIB).o \
					-o $(DIST)/$(XML_CONN).so
  
  $(XML_LIB): clean mkdirs
				$(CXX) $(CPPFLAGS) \
					$(IRRSMO00_SRC)/$(XML_LIB).cpp \
					-o $(ARTIFACTS)/$(XML_LIB).o

  UNIT_TEST:				
				$(CC) -c $(IRRSMO00_SRC)/$(SMO_LIB).c -o $(ARTIFACTS)/$(SMO_LIB).o    

  ifeq ($(UNAME), OS/390)
  smo: $(XML_CONN)
				$(CC) -c -D_XOPEN_SOURCE_EXTENDED \
					-std=c99 \
					-m64 \
					$(IRRSMO00_SRC)/$(SMO_CONN).c \
					-o $(ARTIFACTS)/$(SMO_CONN).o
				$(CC) -Wl,-b,edit=no \
					-m64 \
					-o  $(DIST)/$(SMO_CONN).dll \
					$(ARTIFACTS)/$(SMO_CONN).o \
					$(DIST)/$(XML_CONN).so
  else
  smo: $(XML_CONN) UNIT_TEST
				$(CC) -c -DBUILD_DLL \
					$(IRRSMO00_SRC)/$(SMO_CONN).c \
					-o $(ARTIFACTS)/$(SMO_CONN).o
				$(CC) -shared -Wl \
					$(ARTIFACTS)/$(SMO_CONN).o \
					$(ARTIFACTS)/$(SMO_LIB).o \
					$(DIST)/$(XML_CONN).so \
					-o $(DIST)/$(SMO_CONN).dll
  endif

extract: clean mkdirs
	$(AS) $(ASFLAGS) -o $(ARTIFACTS)/irrseq00.o $(IRRSEQ00_SRC)/irrseq00.s
	cd $(ARTIFACTS) && $(CXX) -c $(CFLAGS) $(IRRSEQ00_SRC)/*.cpp
	$(CXX) $(LDFLAGS) -o $(DIST)/extract $(ARTIFACTS)/*.o 

clean:
	$(RM) $(ARTIFACTS) $(DIST) extract.bin
