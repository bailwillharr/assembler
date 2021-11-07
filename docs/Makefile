assembler.pdf: assembler.tex
	pdflatex $^

clean:
	rm -rf assembler.pdf assembler.log assembler.aux

test: assembler.pdf
	zathura assembler.pdf

.PHONY: clean test
