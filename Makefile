
SUBDIRS=screenshots

MKDIRS=gimpcon gimpcon/2003

PROC=xsltproc
STYLEDIR=xsl
SCRIPTDIR=scripts
STYLESHEET=$(STYLEDIR)/mine.xsl

all: mkdir subdirs website

include depends.tabular

mkdir: $(MKDIRS)

$(MKDIRS):
	mkdir -p $@

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

.PHONY: clean subdirs $(SUBDIRS) mkdir
