CC                  = gcc
CXX					= g++
MEX					= /Applications/MATLAB_R2010b.app/bin/mex
CXXFLAGS			= -g -arch x86_64
CFLAGS              = ${CXXFLAGS}
#LIBS				= -lyajl
MATLAB_INCLUDE_DIR	= -I/Applications/MATLAB_R2010b.app/extern/include
MATLAB_LIB_DIR		= -L/Applications/MATLAB_R2010b.app/bin/maci64
YAJL_INCLUDE_DIR	= -I/usr/local/include
YAJL_LIB_DIR		= -L/usr/local/lib
MEXFLAGS			= ${YAJL_INCLUDE_DIR} ${YAJL_LIB_DIR} ${LIBS} -v -g
MEXSUFFIX           = maci64

all: dta2json import_dta.mex${MEXSUFFIX} import_dta_private.mex${MEXSUFFIX}

dta2json: dta_parse_skeleton.o dta_to_json.o yajl_alloc.o yajl_buf.o yajl_encode.o yajl_gen.o yajl_lex.o yajl_parser.o yajl.o main.o
	${CXX} ${CXXFLAGS} main.o dta_to_json.o dta_parse_skeleton.o yajl_alloc.o yajl_buf.o yajl_encode.o yajl_gen.o yajl_lex.o yajl_parser.o yajl.o ${LIBS} -o dta2json

import_dta_private.mex${MEXSUFFIX}: import_dta_private.cpp dta_to_matlab.o
	${MEX} ${MEXFLAGS} dta_parse_skeleton.o dta_to_json.o dta_to_matlab.o yajl_alloc.o yajl_buf.o yajl_encode.o yajl_gen.o yajl_lex.o yajl_parser.o yajl.o import_dta_private.cpp -output import_dta_private

main.o: main.cpp
	${CXX} -c ${CXXFLAGS} main.cpp

dta_to_json.o: dta_to_json.cpp dta_to_json.h
	${CXX} -c ${CXXFLAGS} dta_to_json.cpp

dta_parse_skeleton.o: dta_parse_skeleton.cpp dta_parse_skeleton.h
	${CXX} -c ${CXXFLAGS} dta_parse_skeleton.cpp

dta_to_matlab.o: dta_to_matlab.cpp dta_to_matlab.h dta_parse_skeleton.o dta_to_json.o
	${CXX} -c ${CXXFLAGS} ${MATLAB_INCLUDE_DIR} dta_to_matlab.cpp

yajl_alloc.o: yajl_alloc.c
	${CC} ${CFLAGS} -c yajl_alloc.c

yajl_buf.o: yajl_buf.c
	${CC} ${CFLAGS} -c yajl_buf.c

yajl_encode.o: yajl_encode.c
	${CC} ${CFLAGS} -c yajl_encode.c

yajl_gen.o: yajl_gen.c
	${CC} ${CFLAGS} -c yajl_gen.c

yajl_lex.o: yajl_lex.c
	${CC} ${CFLAGS} -c yajl_lex.c

yajl_parser.o: yajl_parser.c
	${CC} ${CFLAGS} -c yajl_parser.c

yajl.o: yajl.c
	${CC} ${CFLAGS} -c yajl.c

clean:
	rm -f *.o
	rm -f dta2json
	rm -f *.mex${MEXSUFFIX}