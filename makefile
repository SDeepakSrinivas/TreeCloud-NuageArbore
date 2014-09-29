
CC=g++
END=-O3


OBJS = Unitex_Traitement.o Error.o unicode_flex.o fonts.o path.o cooccurrences.o read_write.o arbredico.o Phylo.o SVG.o ArbreArbore.o

PROG = ArbreArbore

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -g -o $(PROG) $(OBJS) -Wl,-whole-archive ../uni31static2/libunitex.a -lpthread -Wl,--no-whole-archive ../uni31static2/libtre.a
	
Unitex_Traitement.o:
	$(CC) -g -c Unitex_Traitement.cpp -I ../uni31static2/Unitex-C++ -I ../uni31static2/Unitex-C++/build/tre-0.8.0/lib -o Unitex_Traitement.o $(END)
Error.o:
	$(CC) -g -c Error.cpp -o Error.o $(END)
unicode_flex.o:
	$(CC) -g -c unicode_flex.cpp -o unicode_flex.o $(END)
Phylo.o:
	$(CC) -g -c Phylo.cpp -o Phylo.o $(END)
SVG.o:
	$(CC) -g -c SVG.cpp -o SVG.o $(END)
arbredico.o:
	$(CC) -g -c arbredico.cpp -o arbredico.o $(END)
read_write.o:
	$(CC) -g -c read_write.cpp -o read_write.o $(END)
cooccurrences.o:
	$(CC) -g -c cooccurrences.cpp -o cooccurrences.o $(END)
path.o:
	$(CC) -g -c path.cpp -o path.o $(END)
fonts.o:
	$(CC) -g -c fonts.cpp -o fonts.o $(END)
ArbreArbore.o:
	$(CC) -g -c ArbreArbore.cpp -o ArbreArbore.o

clean:
	mv ArbreArbore ../../cgi-bin/ArbreArbore_04
	rm *.o	