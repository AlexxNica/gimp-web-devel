THUMBS = \
	gimp-various-thumb.png

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
