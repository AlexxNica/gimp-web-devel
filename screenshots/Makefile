THUMBS = \
	gimp-brush-preview-thumb.png 	\
	gimp-docks-thumb.png		\
	gimp-file-open-thumb.png	\
	gimp-grid-thumb.png		\
	gimp-osx-thumb.png		\
	gimp-prefs-thumb.png		\
	gimp-svg-import-thumb.png	\
	gimp-templates-thumb.png	\
	gimp-text-thumb.png		\
	gimp-tips-thumb.png		\
	gimp-unicode-thumb.png		\
	gimp-winxp-thumb.png

all: thumbs

.PHONY: thumbs
thumbs: $(THUMBS)

%-thumb.png: %.png
	sh ../scripts/thumb.sh $<

%-thumb.png: %.jpeg
	sh ../scripts/thumb.sh $<

.PHONY: clean
clean:
	-rm -rf *-thumb.png
