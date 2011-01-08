CXX					= g++
MEX					= /Applications/MATLAB_R2010b.app/bin/mex
CXXFLAGS			= -g -arch x86_64
LIBS				= -lyajl
MATLAB_INCLUDE_DIR	= -I/Applications/MATLAB_R2010b.app/extern/include
MATLAB_LIB_DIR		= -L/Applications/MATLAB_R2010b.app/bin/maci64
YAJL_INCLUDE_DIR	= -I/usr/local/include
YAJL_LIB_DIR		= -L/usr/local/lib
MEXFLAGS			= ${YAJL_INCLUDE_DIR} ${YAJL_LIB_DIR} ${LIBS} -v -g
MEXSUFFIX           = maci64

all: dta2json import_dta.mex${MEXSUFFIX}

dta2json: dta_parse_skeleton.o dta_to_json.o main.o
	${CXX} ${CXXFLAGS} main.o dta_to_json.o dta_parse_skeleton.o ${LIBS} -o dta2json

import_dta.mex${MEXSUFFIX}: import_dta.cpp dta_to_matlab.o
	${MEX} ${MEXFLAGS} dta_parse_skeleton.o dta_to_json.o dta_to_matlab.o import_dta.cpp -output import_dta

main.o: main.cpp
	${CXX} -c ${CXXFLAGS} main.cpp

dta_to_json.o: dta_to_json.cpp dta_to_json.h
	${CXX} -c ${CXXFLAGS} dta_to_json.cpp

dta_parse_skeleton.o: dta_parse_skeleton.cpp dta_parse_skeleton.h
	${CXX} -c ${CXXFLAGS} dta_parse_skeleton.cpp

dta_to_matlab.o: dta_to_matlab.cpp dta_to_matlab.h dta_parse_skeleton.o dta_to_json.o
	${CXX} -c ${CXXFLAGS} ${MATLAB_INCLUDE_DIR} dta_to_matlab.cpp

clean:
	rm -f *.o
	rm -f dta2json
	rm -f *.mex${MEXSUFFIX}