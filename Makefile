PROC=xsltproc
STYLEDIR=xsl
SCRIPTDIR=scripts
STYLESHEET=$(STYLEDIR)/mine.xsl

.PHONY : clean

all:
	make -C screenshots all
	make website

include depends.tabular

autolayout.xml: layout.xml
	$(PROC) $(STYLEDIR)/autolayout.xsl $< > $@
	make depends

%.html: autolayout.xml
	$(PROC) $(STYLESHEET) $(filter-out autolayout.xml,$^) $(TIDY) > $@

depends: autolayout.xml
	$(PROC) $(STYLEDIR)/makefile-dep.xsl $< > depends.tabular

depends.tabular:
	touch $@
	make depends
