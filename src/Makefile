
all:
	echo "\nWe all like quick & dirty Makefiles...or not :)\n\n"
	gcc -c -o tldextractor.o -W -Wall tldextractor.c
	gcc -c -o tldlist.o      -W -Wall tldlist.c
	gcc -Wall main.c tldextractor.o tldlist.o -o main
	make clean
clean:
	rm -f *.o
mrproper: clean
	rm -f main

