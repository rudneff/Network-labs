#
# Простейший Makefile для создания отчёта
#

PDF=report.pdf

# Используемые программы
PDFLATEX = pdflatex -interaction=nonstopmode
D2T=utils/dot2tex -f tikz  --crop --prog=neato
L2D=utils/lab2dot

LAB=../net

TEX=tex
INCLUDES = $(TEX)/includes

.PHONY: clean
all : $(PDF)
report : $(PDF)

# Создание из исходного кода тех-файлов.
# $<  -- исходный файл, $@ --файл цели.

# Файл на языке Graphviz, который создается по лабе.
graphs/network.gv: $(LAB)/lab.conf $(LAB)/*/etc/network/interfaces
	$(L2D) $(LAB) $@

# Картинка с графом, которая создаётся из файла Graphviz.
$(INCLUDES)/network_gv.tex  : graphs/network.gv
	$(D2T) --preproc $< | $(D2T) > $@

$(INCLUDES)/%.pdf : $(INCLUDES)/%.tex
	$(PDFLATEX) -output-directory=$(INCLUDES) $<

# Основная цель сборки --- отчет
$(PDF) : $(TEX)/report.tex $(TEX)/header.tex $(INCLUDES)/network_gv.pdf
	cd $(TEX) && $(PDFLATEX) report.tex && $(PDFLATEX) report.tex && cp $(PDF) ..

# Удаление ВСЕХ временных файлов
clean:
	find $(TEX)/ -regextype posix-egrep -type f ! -regex ".*\.(sty|tex|clo|cls|bib|bst|gitignore)" -exec $(RM) {} \; ;

