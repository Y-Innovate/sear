UNAME := $(shell uname)

ARTIFACTS		= ${PWD}/artifacts
DIST			= ${PWD}/dist

# Directory Paths
SRC				= ${PWD}/racfu
IRRSMO00_SRC	= ${PWD}/racfu/irrsmo00
IRRSEQ00_SRC	= ${PWD}/racfu/irrseq00
KEY_MAP			= ${PWD}/racfu/key_map
LOGGER			= ${PWD}/racfu/logger
VALIDATION      = ${PWD}/racfu/validation
EXTERNALS		= ${PWD}/externals
TESTS			= ${PWD}/tests
ZOSLIB			= $(TESTS)/zoslib

ifeq ($(UNAME), OS/390)
	AS 			= as
	CC 			= ibm-clang64
	CXX 		= ibm-clang++64

	SRCZOSLIB	=

	ASFLAGS		= -mGOFF -I$(IRRSEQ00_SRC)
	CFLAGS		= \
				-m64 -fzos-le-char-mode=ascii \
				-I $(SRC) \
				-I $(IRRSMO00_SRC) \
				-I $(IRRSEQ00_SRC) \
				-I $(KEY_MAP) \
				-I $(VALIDATION) \
				-I $(EXTERNALS) \
				-I $(LOGGER)
	TFLAGS		= \
				-DUNIT_TEST -DUNITY_OUTPUT_COLOR \
				-I ${PWD} \
				-I $(TESTS)/mock
	LDFLAGS		= -m64 -Wl,-b,edit=no
else
	CC 			= clang
	CXX 		= clang++

	SRCZOSLIB	= $(ZOSLIB)/*.c

	CFLAGS		= \
				-std=c++11 -D__ptr32= \
				-I $(SRC) \
				-I $(IRRSMO00_SRC) \
				-I $(IRRSEQ00_SRC) \
				-I $(KEY_MAP) \
				-I $(VALIDATION) \
				-I $(EXTERNALS) \
				-I $(LOGGER)
	TFLAGS		= \
				-DUNIT_TEST -DUNITY_OUTPUT_COLOR \
				-I ${PWD} \
				-I $(TESTS)/mock \
				-I $(ZOSLIB)
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
		$(KEY_MAP)/*.cpp \
		$(LOGGER)/*.cpp \
		$(VALIDATION)/*.cpp
	cd $(DIST) && $(CXX) $(LDFLAGS) $(ARTIFACTS)/*.o -o racfu.so

test: clean mkdirs
	cd $(ARTIFACTS) \
		&& $(CXX) -c $(CFLAGS) $(TFLAGS) \
			$(TESTS)/unity/unity.c \
			$(TESTS)/mock/*.cpp \
			$(SRCZOSLIB) \
			$(SRC)/*.cpp \
			$(IRRSMO00_SRC)/*.cpp \
			$(IRRSEQ00_SRC)/*.cpp \
			$(KEY_MAP)/*.cpp \
			$(LOGGER)/*.cpp \
			$(VALIDATION)/*.cpp \
			$(TESTS)/*.cpp \
			$(TESTS)/irrsmo00/*.cpp \
			$(TESTS)/irrseq00/*.cpp \
			$(TESTS)/validation/*.cpp \
		&& $(CXX) $(LDFLAGS) *.o -o $(DIST)/test_runner
	$(DIST)/test_runner

fvt: 
	python3 $(TESTS)/fvt/fvt.py

dbg:
	cd $(ARTIFACTS) && $(CC) -m64 -std=c99 -fzos-le-char-mode=ascii \
		-o $(DIST)/debug \
		${PWD}/debug/debug.c

check:
	cppcheck \
		--suppress='missingIncludeSystem' \
		--suppress='useStlAlgorithm' \
		--inline-suppr \
		--language=c++ \
		--std=c++11 \
		--enable=all \
		--force \
		--check-level=exhaustive \
		--inconclusive \
		--error-exitcode=1 \
		-U __TOS_390__ -D __ptr32= \
		-I $(SRC) \
		-I $(IRRSMO00_SRC) \
		-I $(IRRSEQ00_SRC) \
		-I $(KEY_MAP) \
		-I $(VALIDATION) \
		-I $(LOGGER) \
		-I $(ZOSLIB) \
		$(SRC)/

clean:
	$(RM) $(ARTIFACTS) $(DIST)
