UNAME := $(shell uname)

ARTIFACTS		= ${PWD}/artifacts
DIST			= ${PWD}/dist
 
# Library names
SMO_CONN 		= irrsmo64_conn
SMO_LIB 		= irrsmo64

# Directory Paths
SRC				= ${PWD}/src
IRRSMO00_SRC	= ${PWD}/src/irrsmo00
IRRSEQ00_SRC	= ${PWD}/src/irrseq00
KEY_MAP			= ${PWD}/src/key_map
IRRSMO64_TST	= ${PWD}/tests/irrsmo64
EXTERNALS		= ${PWD}/externals

ifeq ($(UNAME), OS/390)
	AS 			= as
	CC 			= ibm-clang
	CXX 		= ibm-clang++

	ASFLAGS		= -mGOFF -I$(IRRSEQ00_SRC)
	CFLAGS		= \
				-m64 -fzos-le-char-mode=ascii \
				-I $(SRC) \
				-I $(IRRSMO00_SRC) \
				-I $(IRRSEQ00_SRC) \
				-I $(KEY_MAP) \
				-I $(EXTERNALS)
	LDFLAGS		= -m64 -Wl,-b,edit=no
	CKFLGS		= \
				--clang=ibm-clang++64 \

	REQTEST		=
else
	CC 			= clang
	CXX 		= clang++

	CFLAGS  	= -g -Wall -std=c++11
	CPPFLAGS 	= -fpic -c -D_XOPEN_SOURCE_EXTENDED -std=c++11 -m64
	LDFLAGS		= -shared -Wl -m64
	CKFLGS		= \
				--suppress='missingIncludeSystem'

	REQTEST		= SMO64_TEST
endif

RM				= rm -rf

all: racfu

mkdirs:
	mkdir $(ARTIFACTS)
	mkdir $(DIST)

racfu: clean mkdirs
	$(AS) $(ASFLAGS) -o $(ARTIFACTS)/irrseq00.o $(IRRSEQ00_SRC)/irrseq00.s
	cd $(ARTIFACTS) && $(CXX) -c $(CFLAGS) \
		$(SRC)/*.cpp \
		$(IRRSMO00_SRC)/*.cpp \
		$(IRRSEQ00_SRC)/*.cpp \
		$(KEY_MAP)/*.cpp
	cd $(DIST) && $(CXX) $(LDFLAGS) $(ARTIFACTS)/*.o -o racfu.so

dbg:
	cd $(ARTIFACTS) && $(CC) -m64 -std=c99 -fzos-le-char-mode=ascii \
		-o $(DIST)/debug \
		${PWD}/debug/debug.c

check:
	cppcheck \
		--language=c++ \
		--std=c++11 \
		--enable=all \
		--suppress='*:*/externals/*' \
		--output-file=cppcheck/output.xml \
		--checkers-report=cppcheck/checkers_report.txt \
		--cppcheck-build-dir=cppcheck \
		--xml --xml-version=2 \
		--force --verbose --check-level=exhaustive \
		$(CKFLGS) \
		-I $(SRC) \
		-I $(IRRSMO00_SRC) \
		-I $(IRRSEQ00_SRC) \
		-I $(KEY_MAP) \
		-I $(EXTERNALS) \
		$(SRC)/*.cpp \
		$(IRRSMO00_SRC)/*.cpp \
		$(IRRSEQ00_SRC)/*.cpp \
		$(KEY_MAP)/*.cpp 

SMO64_TEST:	
	$(CXX) -c $(IRRSMO64_TST)/$(SMO_LIB).cpp -o $(ARTIFACTS)/$(SMO_LIB).o    

clean:
	$(RM) $(ARTIFACTS) $(DIST)
