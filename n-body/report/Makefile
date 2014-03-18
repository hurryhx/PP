TARGET=report
TEX=xelatex -shell-escape
BIBTEX=bibtex
READER=evince

all: view

output/$(TARGET).pdf: *.tex *.bib output
	cd output && rm -f *.tex *.bib && ln -s ../*.tex ../*.bib .
	cd output && $(TEX) $(TARGET).tex && $(BIBTEX) $(TARGET).aux && \
		$(TEX) $(TARGET).tex
	make

rebuild:
	make clean
	make

output:
	mkdir output
	cd output && rm -f data src res && ln -s ../data ../src ../res .

view: output/$(TARGET).pdf
	$(READER) output/$(TARGET).pdf &

clean:
	rm -rf output

dist: output/$(TARGET).pdf
	rm -rf paper
	mkdir paper
	cp output/$(TARGET).pdf paper/
	7z a -tzip paper.zip paper
	rm -rf paper

.PHONY: all view clean rebuild dist
