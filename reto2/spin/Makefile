#  ISEL 2022

#  Reto 2: Makefile Spin
#  Cristina Conforto Lopez
#  Marcos Gomez Bracamonte
#  Miguel Medina Anton


MODELS = alarm code light
alarm_SPEC = alarmActiveAndPresenceSounds alarmNoActiveNoSounds alarmNoActiveAndButtonActive alarmActiveAndButtonNoActive
code_SPEC = codeKOWhileInputNotCode
light_SPEC = siPresenciaLuz sifinPresenciaLuz
model_PANARGS = -m100000

all: $(patsubst %, %.verify, $(MODELS))


%.pan: %.pml
	@echo "----------- GENERANDO VERIFICADOR $(*F) ---------------"
	spin -a $< && gcc -o $(*F).pan pan.c
	rm -f *.trail

%.verify: %.pan
	@echo "----------- VERIFICANDO $(*F) -------------------------"
	for f in $($(*F)_SPEC); do ./$< -a -f $($(*F)_PANARGS) -N $$f ; \
	  if [ -f *.trail ]; then \
	    spin -t -r -s -l -g $(*F).pml ; \
	  fi ; \
	done

clean:
	rm -f *.trail *.pan pan.* *.tmp *.tcl pan