DIR := ${CURDIR}

HDR = ./interface/
SRC = ./src/
PLG = ./plugins/
PRG = ./main/
OBJ = ./lib/
LIB = ./lib/
BIN = ./bin/

HDRSuf = .h
SRCSuf = .cc
PRGSuf = .cpp
OBJSuf = .o
LIBSuf = .so

HDRS     =  $(wildcard $(HDR)*$(HDRSuf))
SRCS     =  $(wildcard $(SRC)*$(SRCSuf))
PLGS     =  $(wildcard $(PLG)*$(SRCSuf))
_OBJS    =  $(patsubst %$(SRCSuf), %$(OBJSuf), $(SRCS))
OBJS     =  $(patsubst $(SRC)%, $(OBJ)%, $(_OBJS))
_PLGOBJS =  $(patsubst %$(SRCSuf), %$(OBJSuf), $(PLGS))
PLGOBJS  =  $(patsubst $(PLG)%, $(OBJ)%, $(_PLGOBJS))
_LIBS    =  $(patsubst %$(SRCSuf), %$(LIBSuf), $(PLGS))
LIBS     =  $(patsubst $(PLG)%, $(LIB)lib%, $(_LIBS))
PRGS     =  $(wildcard $(PRG)*$(PRGSuf))
_BINS    =  $(wildcard $(PRG)*$(PRGSuf))
__BINS   =  $(_BINS:$(PRGSuf)=$(BINSuf))
___BINS  =  $(notdir $(__BINS))
BINS     =  $(addprefix $(BIN),${___BINS})

LINKDEF   =  $(wildcard ${HDR}*LinkDef.h)
DICTHDRS  =  $(patsubst $(LINKDEF),,$(HDRS)) $(LINKDEF)


ARCH  =  $(shell root-config --arch)

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTGLIBS     = $(shell root-config --glibs)



CXX  =  g++
CXXFLAGS  = -Wall -Wno-sign-compare -Wno-overloaded-virtual -O2 -fPIC -I$(DIR) -I/cvmfs/cms.cern.ch/slc6_amd64_gcc700/external/boost/1.63.0-omkpbe4/include $(ROOTCFLAGS) 

CPP  =  g++
CPPFLAGS  = -Wall -Wno-sign-compare -Wno-overloaded-virtual -I$(DIR) -I/cvmfs/cms.cern.ch/slc6_amd64_gcc700/external/boost/1.63.0-omkpbe4/include $(ROOTCFLAGS)

LD       =  g++
LDFLAGS  =  -rdynamic -shared -O2
SONAME	 =  libMakePlots.so
SOFLAGS  =  -Wl,-soname,

GLIBS   =  -lm -Wl,-rpath=lib/ $(ROOTGLIBS)



.PHONY: all clean test


all: $(LIB)$(SONAME) $(LIBS) $(BINS)

test:
	@echo "HDRS = $(HDRS)"
	@echo "DICTHDRS = $(DICTHDRS)"
	@echo "SRCS = $(SRCS)"
	@echo "PLGS = $(PLGS)"
	@echo "PRGS = $(PRGS)"
	@echo "OBJS = $(OBJS)"
	@echo "PLGOBJS = $(PLGOBJS)"
	@echo "LIBS = $(LIBS)"
	@echo "BINS = $(BINS)"

$(BIN)%: $(PRG)%$(PRGSuf) $(HDRS) $(LIB)$(SONAME) Makefile
	@echo " CXX $<"
	$(CPP) $(CPPFLAGS) $(GLIBS) -L$(LIB) -lMakePlots -o $@ $<

$(OBJ)%$(OBJSuf): $(SRC)%$(SRCSuf) Makefile
	@echo " CXX $<"
	@$ $(CXX) -c $(CXXFLAGS) -o $@ $< 

$(LIB)mydict.cc: $(DICTHDRS)
	@echo "Generating dictionary..."
	@$ rootcling -f $(LIB)mydict.cc -c -p ${CXXFLAGS} $(DICTHDRS)

$(LIB)mydict.o: $(LIB)mydict.cc
	@echo " CXX $<"	
	@$ $(CXX) -c $(CXXFLAGS) -o $@ $<

$(LIB)$(SONAME): $(OBJS) $(LIB)mydict.o
	@echo "Linking $(SONAME):"
	@$ $(LD) $(LDFLAGS) $(OBJS) $(LIB)mydict.o -o $(LIB)$(SONAME) $(SOFLAGS)$(SONAME)

$(LIB)lib%$(LIBSuf): $(PLG)%$(SRCSuf) $(PLG)%$(HDRSuf) $(OBJS)
	@echo "Creating plugin library " $@
	@echo " CXX $<"	
	@$ $(LD) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(SOFLAGS)lib$*.so $(GLIBS)

clean:
	@echo "cleaning..."
	rm -f lib/* $(OBJ)*$(OBJSuf) $(LIB)*$(LIBSuf) $(LIB)mydict* $(BIN)*$(BINSuf)
