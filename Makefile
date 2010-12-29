all: dta2json

dta2json: dta_parse_skeleton.o dta_to_json.o main.o
	g++ main.o dta_to_json.o dta_parse_skeleton.o -lyajl -o dta2json

main.o: main.cpp
	g++ -c main.cpp

dta_to_json.o: dta_to_json.cpp
	g++ -c dta_to_json.cpp

dta_parse_skelelton.o: dta_parse_skeleton.cpp
	g++ -c dta_parse_skelelton.cpp

clean:
	rm -f *.o
	rm -f dta2json