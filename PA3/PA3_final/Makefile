all:
	gcc -pthread -g -o abba abba.c
	gcc -pthread -g -o abba_mod abba_mod.c
	gcc -pthread -g -o gateabba gateabba.c
	gcc -pthread -g -o dinning dinning_deadlock.c
	gcc -pthread -o DPredictor DPredictor.c
	gcc -pthread -shared -fPIC -o DDetector.so DDetector.c -ldl
	gcc -pthread -shared -fPIC -o DMonitor.so DMonitor.c -ldl
