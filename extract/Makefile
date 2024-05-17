ARTIFACTS	= ./artifacts
DIST		= ./dist

AS 			= as
ASFLAGS		= -mGOFF -I.

CXX			= ibm-clang++
CFLAGS		= -fzos-le-char-mode=ascii -I ./

RM			= rm -rf


all: extract

mkdirs:
	mkdir $(ARTIFACTS)
	mkdir $(DIST)

extract: clean mkdirs
	$(AS) $(ASFLAGS) -o irrseq00.o irrseq00.s
	$(CXX) -c $(CFLAGS) *.cpp && mv *.o $(ARTIFACTS)
	$(CXX) -o $(DIST)/extract $(ARTIFACTS)/*.o 

clean:
	$(RM) $(ARTIFACTS) $(DIST) extract.bin
