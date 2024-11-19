UNAME := $(shell uname)

ARTIFACTS		= ${PWD}/artifacts
DIST			= ${PWD}/dist

# Directory Paths
SRC				= ${PWD}/racfu
IRRSMO00_SRC	= ${PWD}/racfu/irrsmo00
IRRSEQ00_SRC	= ${PWD}/racfu/irrseq00
KEY_MAP			= ${PWD}/racfu/key_map
VALIDATION      = ${PWD}/racfu/validation
EXTERNALS		= ${PWD}/externals
TESTS			= ${PWD}/tests

ifeq ($(UNAME), OS/390)
	AS 			= as
	CC 			= ibm-clang
	CXX 		= ibm-clang++

	ZOSLIB		= 

	ASFLAGS		= -mGOFF -I$(IRRSEQ00_SRC)
	CFLAGS		= \
				-m64 -fzos-le-char-mode=ascii \
				-I $(SRC) \
				-I $(IRRSMO00_SRC) \
				-I $(IRRSEQ00_SRC) \
				-I $(KEY_MAP) \
				-I $(VALIDATION) \
				-I $(EXTERNALS)
	TFLAGS		= \
				-DUNIT_TEST -DUNITY_OUTPUT_COLOR\
				-I ${PWD} \
				-I $(TESTS)/mock \
				-I $(TESTS)/zoslib
	LDFLAGS		= -m64 -Wl,-b,edit=no
	CKFLGS		= --clang=ibm-clang++64 
else
	CC 			= clang
	CXX 		= clang++

	ZOSLIB		= $(TESTS)/zoslib/*.c

	CFLAGS		= \
				-std=c++11 -D__ptr32= \
				-I $(SRC) \
				-I $(IRRSMO00_SRC) \
				-I $(IRRSEQ00_SRC) \
				-I $(KEY_MAP) \
				-I $(VALIDATION) \
				-I $(EXTERNALS)
	TFLAGS		= \
				-DUNIT_TEST -DUNITY_OUTPUT_COLOR \
				-I ${PWD} \
				-I $(TESTS)/mock \
				-I $(TESTS)/zoslib
	CKFLGS		= --suppress='missingIncludeSystem'
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
		$(VALIDATION)/*.cpp
	cd $(DIST) && $(CXX) $(LDFLAGS) $(ARTIFACTS)/*.o -o racfu.so

test: clean mkdirs
	cd $(ARTIFACTS) \
		&& $(CXX) -c $(CFLAGS) $(TFLAGS) \
			$(TESTS)/unity/unity.c \
			$(TESTS)/mock/*.cpp \
			$(ZOSLIB) \
			$(SRC)/*.cpp \
			$(IRRSMO00_SRC)/*.cpp \
			$(IRRSEQ00_SRC)/*.cpp \
			$(KEY_MAP)/*.cpp \
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

check: export CLANG_CONFIG_PATH = ${PWD}/clang.cfg
check:
	mkdir -p artifacts/cppcheck
	cppcheck \
		--language=c++ \
		--std=c++11 \
		--enable=all \
		--suppress='*:*/externals/*' \
		--suppress='*:*openxl\*' \
		--output-file=artifacts/cppcheck/output.xml \
		--checkers-report=artifacts/cppcheck/checkers_report.txt \
		--cppcheck-build-dir=artifacts/cppcheck \
		--xml --xml-version=2 \
		--force \
		--verbose \
		--check-level=exhaustive \
		--inconclusive \
		$(CKFLGS) \
		-I $(SRC) \
		-I $(IRRSMO00_SRC) \
		-I $(IRRSEQ00_SRC) \
		-I $(KEY_MAP) \
		-I $(VALIDATION) \
		-I $(EXTERNALS) \
		$(SRC)/*.cpp \
		$(IRRSMO00_SRC)/*.cpp \
		$(IRRSEQ00_SRC)/*.cpp \
		$(KEY_MAP)/*.cpp 

clean:
	$(RM) $(ARTIFACTS) $(DIST)
