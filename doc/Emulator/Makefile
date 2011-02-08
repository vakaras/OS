default: fresh

# Sugeneruoti straipsnį.
aprašymas.pdf:
	xelatex -shell-escape main.tex
	#bibtex main
	xelatex -shell-escape main.tex
	xelatex -shell-escape main.tex
	mv main.pdf aprašymas.pdf

fresh: clear aprašymas.pdf

# Išvalyti šiukšles.
clean:
	rm -f *.aux *.log *.xdv *.out *.toc *.bbl *.blg

# Ištrinti visus sugeneruotus failus.
clear: clean
	rm -f *.pdf
