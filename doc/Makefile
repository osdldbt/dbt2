
.SUFFIXES: .tex .dvi .aux .eps .fig .dia .ps .pdf .bib .bbl

FIGFILES:=$(wildcard *.fig)
EPSFILES:=$(wildcard *.eps)
EPSFILES+=$(FIGFILES:.fig=.eps)
PDFFILES=$(EPSFILES:.eps=.pdf)

.fig.eps:
	fig2dev -L eps $< >$@

.fig.pdf:
	fig2dev -L pdf $< >$@

.eps.pdf:
	epstopdf $<

all: dbt2-architecture.ps dbt2-architecture.pdf dbt2-tpc.ps dbt2-tpc.pdf dbt2-user.ps dbt2-user.pdf

%.ps: %.dvi
	dvips -o $@ $<

%.dvi: %.tex $(EPSFILES)
	latex $< || true
	bibtex $< || true
	latex $< || true
	latex $<

%.pdf: %.tex $(PDFFILES)
	pdflatex $< || true
	bibtex $< || true
	pdflatex $< || true
	pdflatex $<

clean:
	rm -f *.aux *.dvi *.log 
	rm -f *.ps *.pdf *.bbl *.blg
