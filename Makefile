

termbmp: termbmp.o
	g++ -o termbmp termbmp.o

termbmp.o: termbmp.cpp
	g++ -c termbmp.cpp
