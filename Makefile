include ./Makefile.inc

COMMON_OBJS = common/dta_common.o
YAJL_OBJS = \
	yajl_build/yajl_alloc.o \
	yajl_build/yajl_buf.o \
	yajl_build/yajl_encode.o \
	yajl_build/yajl_gen.o \
	yajl_build/yajl_lex.o \
	yajl_build/yajl_parser.o \
	yajl_build/yajl.o

all: json/dta2json matlab/import_dta_private.mex${MEXSUFFIX}

json/dta2json: common_obj_lib.a yajl_obj_lib.a json/main.o json/dta_to_json.o
	${CXX} ${CXXFLAGS} $^ -o $@

matlab/import_dta_private.mex${MEXSUFFIX}: common_obj_lib.a matlab/dta_to_matlab.o matlab/import_dta_private.cpp
	${MEX} ${MEXFLAGS} ${COMMON_INCLUDE_DIR} ${MATLAB_INCLUDE_DIR} $^ -output matlab/import_dta_private

matlab/dta_to_matlab.o: matlab/dta_to_matlab.cpp
	${CXX} ${CXXFLAGS} ${COMMON_INCLUDE_DIR} ${MATLAB_INCLUDE_DIR} -c $< -o $@

common_obj_lib.a: ${COMMON_OBJS}
	${AR} ${ARFLAGS} rv common_obj_lib.a $^
	${RANLIB} common_obj_lib.a

yajl_obj_lib.a: ${YAJL_OBJS}
	${AR} ${ARFLAGS} rv yajl_obj_lib.a $^
	${RANLIB} yajl_obj_lib.a

yajl_build/%.o: yajl_build/%.c
	${CC} ${CFLAGS} -c $< -o $@

common/%.o: common/%.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@

json/%.o: json/%.cpp
	${CXX} ${CXXFLAGS} ${COMMON_INCLUDE_DIR} ${YAJL_INCLUDE_DIR} -c $< -o $@

clean:
	rm -rf *.o
	rm -rf common/*.o
	rm -rf yajl_build/*.o
	rm -rf json/*.o
	rm -rf matlab/*.o
	rm -rf *.a
	rm -f json/dta2json
	rm -f matlab/*.mex${MEXSUFFIX}