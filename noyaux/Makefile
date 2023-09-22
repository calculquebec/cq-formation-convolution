noyaux= \
  flou_05 \
  flou_15 \
  flou_25 \
  flou_35 \
  flou_45 \
  flou_55 \
  flou_65 \
  flou_75 \

all: $(noyaux)

flou_%: generer.py
	python generer.py ${@:flou_%=%} > $@

clean:
	rm -f $(noyaux)
