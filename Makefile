PROC=xsltproc
STYLEDIR=xsl
SCRIPTDIR=scripts
STYLESHEET=$(STYLEDIR)/mine.xsl
#TIDY= | tidy -xml -quiet -indent
GIMP_CHANGELOG=/home/neo/gnomecvs/gimp/ChangeLog

.PHONY : clean

all:
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

gimp-cvs.rdf: $(GIMP_CHANGELOG)
	$(SCRIPTDIR)/cl2rdf $< > $@
