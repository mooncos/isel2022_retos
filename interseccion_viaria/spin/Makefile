spin:
	spin -a model.pml
	gcc -o pan pan.c
	./pan -a -f -N peatonEvtCrucePrincipal
	./pan -a -f -N peatonEvtCruceSecundario
	./pan -a -f -N cochesEspiraPasarSecundario
	./pan -a -f -N nuncaDosSemaforosVerde

clean:
	rm -f pan* _spin_nvr.tmp model.pml.trail