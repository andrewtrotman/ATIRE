EXE = ltw_eval.exe teara_run_to_trec.exe 
all : $(EXE)

ltw_eval.exe : ltw_eval.c
	cl /W2 /Zi /Ox /Tp ltw_eval.c

teara_run_to_trec.exe : teara_run_to_trec.c
	cl /W2 /Zi /Tp teara_run_to_trec.c

clean :
	del *.bak *.obj $(EXE)
