
SUBDIRS=screenshots

PROC=xsltproc
STYLEDIR=xsl
SCRIPTDIR=scripts
STYLESHEET=$(STYLEDIR)/mine.xsl

all: subdirs website

include depends.tabular

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

autolayout.xml: layout.xml
	$(PROC) $(STYLEDIR)/autolayout.xsl $< > $@
	$(MAKE) depends

%.html: autolayout.xml
	$(PROC) $(STYLESHEET) $(filter-out autolayout.xml,$^) $(TIDY) > $@

depends: autolayout.xml
	$(PROC) $(STYLEDIR)/makefile-dep.xsl $< > depends.tabular

depends.tabular: layout.xml
	touch $@
	$(MAKE) depends

.PHONY: clean subdirs $(SUBDIRS)
