THUMBS = \
	gimp-help-browser-thumb.png		\
	gimp-misc-thumb.png			\
	gimp-prefs-color-management-thumb.png

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
