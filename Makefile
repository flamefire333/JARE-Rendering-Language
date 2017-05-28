JakeRenders=$(wildcard *.jare)
Programs=$(subst .jare,,$(JakeRenders))
parser: parser.c Makefile
	gcc parser.c -o parser
renderer: renderer.c Makefile
	gcc -S renderer.c -o renderer.S -lpng -lm -lpthread
tools:
	make parser
	make renderer
all:
	make $(Programs)
clean:
	rm -f *.S
	rm -f render-*.png
	rm -f $(Programs)

$(Programs): % : Makefile renderer.c parser.c %.jare
	make tools
	parser <$*.jare> $*.S
	gcc $*.S renderer.S -o $* -lpng -lm -lpthread
	$*
