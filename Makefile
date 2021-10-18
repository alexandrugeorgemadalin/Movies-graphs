build:
	gcc -g -Wall -o movies movies.c graph.c graph.h Util.h Util.c -I.

run : 
	./movies

clean : 
	rm movies
