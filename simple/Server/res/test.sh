rm *.data
rm *.log
rm *.aux
rm *.pdf
pdflatex test.tex && evince test.pdf
