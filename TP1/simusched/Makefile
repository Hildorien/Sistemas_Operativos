export LC_ALL=C
CXXFLAGS= -Wall -pedantic -g -ggdb
LDFLAGS= -lpthread

OBJS=main.o simu.o basesched.o basetask.o tasks.o sched_fcfs.o sched_rr.o sched_psjf.o sched_nm.o
MAIN=simusched

.PHONY: all clean new ejercicio1 ejercicio2 ejercicio3 ejercicio4 ejercicio5 ejercicio6 ejercicio7
all: $(MAIN)

ejercicio1: simusched
	./simusched lote_ej1.tsk 1 0 5 SchedFCFS | ./graphsched.py > Imagenes_Ej1/imagenej1.png

ejercicio2: simusched
	./simusched lote_ej2.tsk 1 2 5 SchedFCFS | ./graphsched.py > Imagenes_Ej2/imagenej2n1.png
	./simusched lote_ej2.tsk 2 2 2 SchedFCFS | ./graphsched.py > Imagenes_Ej2/imagenej2n2.png
	./simusched lote_ej2.tsk 3 2 2 SchedFCFS | ./graphsched.py > Imagenes_Ej2/imagenej2n3.png

ejercicio3: simusched
	./simusched lote_ej3.tsk 2 2 2 SchedFCFS | ./graphsched.py > Imagenes_Ej3/imagenej3n2.png
	./simusched lote_ej3.tsk 3 2 2 SchedFCFS | ./graphsched.py > Imagenes_Ej3/imagenej3n3.png

ejercicio4: simusched
	./simusched lote_ej3.tsk 2 0 0 SchedRR 2 3 | ./graphsched.py > Imagenes_Ej4/imagenej4n2.png
	./simusched lote_ej2.tsk 2 0 0 SchedRR 2 3 | ./graphsched.py > Imagenes_Ej4/imagenej4n2b.png

ejercicio5: simusched
	./simusched lote.tsk 1 0 5 SchedNoMistery | ./graphsched.py > Imagenes_Ej5/imagenej5mn1.png
	./simusched lote.tsk 2 0 5 SchedNoMistery | ./graphsched.py > Imagenes_Ej5/imagenej5mn2.png
ejercicio6: simusched
	./simusched lote_ej6.tsk 1 0 5 SchedPSJF | ./graphsched.py > Imagenes_Ej6/imagenej6n1.png
	./simusched lote_ej6.tsk 2 0 5 SchedPSJF | ./graphsched.py > Imagenes_Ej6/imagenej6n2.png

ejercicio7: simusched
	./simusched lote_ej7_t1.tsk 1 1 0 SchedRR 5 | ./graphsched.py > Imagenes_Ej7/RR_test1_n1.png
	./simusched lote_ej7_t1.tsk 2 1 0 SchedRR 5 | ./graphsched.py > Imagenes_Ej7/RR_test1_n2.png
	./simusched lote_ej7_t2.tsk 1 1 0 SchedRR 5 | ./graphsched.py > Imagenes_Ej7/RR_test2_n1.png
	./simusched lote_ej7_t2.tsk 2 1 0 SchedRR 5 | ./graphsched.py > Imagenes_Ej7/RR_test2_n2.png
	./simusched lote_ej7_t1_prior.tsk 1 1 0 SchedPSJF | ./graphsched.py > Imagenes_Ej7/PSJF_test1_n1.png
	./simusched lote_ej7_t1_prior.tsk 2 1 0 SchedPSJF | ./graphsched.py > Imagenes_Ej7/PSJF_test1_n2.png
	./simusched lote_ej7_t2_prior.tsk 1 1 0 SchedPSJF | ./graphsched.py > Imagenes_Ej7/PSJF_test2_n1.png
	./simusched lote_ej7_t2_prior.tsk 2 1 0 SchedPSJF | ./graphsched.py > Imagenes_Ej7/PSJF_test2_n2.png
	./simusched lote_ej7_t1.tsk 1 1 0 SchedNoMistery | ./graphsched.py > Imagenes_Ej7/NM_test1_n1.png
	./simusched lote_ej7_t1.tsk 2 1 0 SchedNoMistery | ./graphsched.py > Imagenes_Ej7/NM_test1_n2.png
	./simusched lote_ej7_t2.tsk 1 1 0 SchedNoMistery | ./graphsched.py > Imagenes_Ej7/NM_test2_n1.png
	./simusched lote_ej7_t2.tsk 2 1 0 SchedNoMistery | ./graphsched.py > Imagenes_Ej7/NM_test2_n2.png

$(MAIN): $(OBJS) sched_mistery.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp %.h basesched.h
basesched.o: basesched.cpp basesched.h
basetask.o: basetask.cpp basetask.h
main.o: main.cpp simu.h basetask.h basesched.h tasks.h sched_fcfs.h sched_rr.h sched_psjf.h sched_nm.h
simu.o: simu.cpp simu.h basetask.h basesched.h
simusched.o: simusched.cpp basetask.h basesched.h tasks.h
tasks.o: tasks.cpp tasks.h basetask.h

clean:
	rm -f $(OBJS) $(MAIN)

new: clean all

