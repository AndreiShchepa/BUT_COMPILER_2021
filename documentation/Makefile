SHELL = /usr/bin/env bash
NAME = dokumentace



all:
	pdflatex $(NAME).tex
	pdflatex $(NAME).tex
	@$(MAKE) -s clean

clean:
	rm -f $(NAME).{aux,dvi,log,ps,out,toc}
