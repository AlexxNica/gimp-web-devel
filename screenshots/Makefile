THUMBS = \
	gimp-open-dialog-thumb.png	\
	gimp-preview-cartoon-thumb.png	\
	gimp-preview-neon-thumb.png	\
	gimp-preview-rotate-thumb.png	\
	gimp-various-thumb.png		\
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
