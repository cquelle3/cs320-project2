P = cache-sim
CFLAGS = -Wall -Werror -g

all: $(P)

$(P): $(P).o 
	g++ $(CFLAGS) -std=c++14 $(P).o -o $(P)
$(P).o: $(P).cpp
	g++ $(CFLAGS) -std=c++14 -c $(P).cpp -o $(P).o

run: all
	./$(P)
memcheck: all
	valgrind ./$(P)
clean:
	rm *.o $(P)
