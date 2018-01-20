#
# Простейший Makefile для создания отчёта
#

PDF=report.pdf

# Используемые программы
PDFLATEX = pdflatex -interaction=nonstopmode

LAB=../net

TEX=tex
INCLUDES = $(TEX)/includes

.PHONY: clean
all : $(PDF)
report : $(PDF)

$(INCLUDES)/%.pdf : $(INCLUDES)/%.tex
	$(PDFLATEX) -output-directory=$(INCLUDES) $<

# Основная цель сборки --- отчет
$(PDF) : $(TEX)/report.tex $(TEX)/header.tex
	cd $(TEX) && $(PDFLATEX) report.tex && $(PDFLATEX) report.tex && cp $(PDF) ..

# Удаление ВСЕХ временных файлов
clean:
	find $(TEX)/ -regextype posix-egrep -type f ! -regex ".*\.(sty|tex|clo|cls|bib|bst|gitignore)" -exec $(RM) {} \; ;

