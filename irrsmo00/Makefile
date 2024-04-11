UNAME := $(shell uname)

  ifeq ($(UNAME), OS/390)
  CC = xlclang
  CXX = xlclang++

  CFLAGS = 
  CPPFLAGS = -qascii -qlanglvl=extended0x
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
				$(CXX) $(CPPFLAGS) $(CPP_PATH)$(XML_CONN).cpp $(LIB_PATH)$(XML_LIB).so -o $(LIB_PATH)$(XML_CONN).so
  
  $(XML_LIB):
				$(CXX) $(CPPFLAGS) $(CPP_PATH)$(XML_LIB).cpp -o $(LIB_PATH)$(XML_LIB).so

  $(SMO_LIB):				
				$(CC) -c $(C_PATH)$(SMO_LIB).c -o $(LIB_PATH)$(SMO_LIB).o    

  ifeq ($(UNAME), OS/390)
  dll: $(XML_CONN)
				$(CC) -c -DBUILD_DLL $(C_PATH)$(SMO_CONN).c  -I../../safCommon -I libIRRSMO64.so $(LIB_PATH)$(XML_CONN).so -o $(LIB_PATH)$(SMO_CONN).o
				$(CC) -v -s-Wl,"DLL,LP64,XPLINK" $(LIB_PATH)$(SMO_CONN).o  -I../../safCommon -I libIRRSMO64.so $(LIB_PATH)$(XML_CONN).so -o $(LIB_PATH)$(SMO_CONN).dll
  else
  dll: $(XML_CONN) $(SMO_LIB)
				$(CC) -c -DBUILD_DLL $(C_PATH)$(SMO_CONN).c  $(LIB_PATH)$(XML_CONN).so -o $(LIB_PATH)$(SMO_CONN).o
				$(CC) -v -shared -Wl $(LIB_PATH)$(SMO_CONN).o $(LIB_PATH)$(SMO_LIB).o $(LIB_PATH)$(XML_CONN).so -o $(LIB_PATH)$(SMO_CONN).dll
  endif
  clean:
				$(RM) $(LIB_PATH)*.o $(LIB_PATH)*.dll $(LIB_PATH)*.so