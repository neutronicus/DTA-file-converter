CXX= g++
CXXFLAGS= -g
LIBS= -lyajl

all: dta2json

dta2json: dta_parse_skeleton.o dta_to_json.o main.o
	${CXX} ${CXXFLAGS} main.o dta_to_json.o dta_parse_skeleton.o ${LIBS} -o dta2json

main.o: main.cpp
	${CXX} -c ${CXXFLAGS} main.cpp

dta_to_json.o: dta_to_json.cpp dta_to_json.h
	${CXX} -c ${CXXFLAGS} dta_to_json.cpp

dta_parse_skelelton.o: dta_parse_skeleton.cpp dta_parse_skeleton.h
	${CXX} -c ${CXXFLAGS} dta_parse_skelelton.cpp

clean:
	rm -f *.o
	rm -f dta2json