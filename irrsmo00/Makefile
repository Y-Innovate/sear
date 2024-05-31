UNAME := $(shell uname)

  ifeq ($(UNAME), OS/390)
  CC = xlc
  CXX = ibm-clang++

  CFLAGS = 
  CPPFLAGS = -std=c++11 -m64 -fzos-le-char-mode=ascii
  else
  # the compiler: gcc for C program, define as g++ for C++
  CC = gcc
  CXX = g++
    # compiler flags:
  #  -g     - this flag adds debugging information to the executable file
  #  -Wall  - this flag is used to turn on most compiler warnings
  CFLAGS  = -g -Wall -std=c++11
  CPPFLAGS = -fpic -shared -std=c++11
  endif
 
  # Library names
  XML_LIB = saf_xml
  XML_CONN = xml_conn
  SMO_CONN = irrsmo00_conn
  SMO_LIB = irrsmo64

  # Directory Paths
  PARENT_PATH = openRACFCore/corelib/
  C_PATH = $(PARENT_PATH)
  CPP_PATH = $(PARENT_PATH)
  LIB_PATH = $(PARENT_PATH)lib/

  all: dll
  
  $(XML_CONN): $(XML_LIB)
				$(CXX) $(CPPFLAGS) $(CPP_PATH)$(XML_CONN).cpp $(LIB_PATH)$(XML_LIB).o -o $(LIB_PATH)$(XML_CONN).so
				$(RM) $(XML_CONN).o
  
  $(XML_LIB):
				$(CXX) $(CPPFLAGS) $(CPP_PATH)$(XML_LIB).cpp -o $(LIB_PATH)$(XML_LIB).o
				$(RM) $(XML_LIB).o

  UNIT_TEST:				
				$(CC) -c $(C_PATH)$(SMO_LIB).c -o $(LIB_PATH)$(SMO_LIB).o    

  ifeq ($(UNAME), OS/390)
  dll: $(XML_CONN)
				$(CC) -c -D_XOPEN_SOURCE_EXTENDED -qlanglvl=extc99 -Wc,lp64,STACKPROTECT\(ALL\) -I../../safCommon -I libIRRSMO64.so $(C_PATH)$(SMO_CONN).c $(LIB_PATH)$(XML_CONN).so -o $(LIB_PATH)$(SMO_CONN).o
				$(CC) -Wl,"DLL,LP64,XPLINK" -o  $(LIB_PATH)$(SMO_CONN).dll $(LIB_PATH)$(SMO_CONN).o $(LIB_PATH)$(XML_CONN).so
				$(RM) $(SMO_CONN).x
  else
  dll: $(XML_CONN) UNIT_TEST
				$(CC) -c -DBUILD_DLL $(C_PATH)$(SMO_CONN).c -o $(LIB_PATH)$(SMO_CONN).o
				$(CC) -v -shared -Wl $(LIB_PATH)$(SMO_CONN).o $(LIB_PATH)$(SMO_LIB).o $(LIB_PATH)$(XML_CONN).so -o $(LIB_PATH)$(SMO_CONN).dll
  endif
  clean:
				$(RM) $(LIB_PATH)*.o $(LIB_PATH)*.dll $(LIB_PATH)*.so *.o *.x