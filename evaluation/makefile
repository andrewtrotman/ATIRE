all : 	between.exe 						\
		mkqrel.exe 							\
		trec2query.exe 						\
		INEXtopics_to_ANTquery.exe 			\
		INEXieee_assessments_to_ANT.exe 	\
		INEXassessments_get_best.exe 		\
		qrel_orig_alt_compare.exe			\
		MAP_fake.exe

between.exe : between.c

mkqrel.exe : mkqrel.c

trec2query.exe : trec2query.c stop_word.c
	$(CC) $(CFLAGS) /EHsc /TP trec2query.c stop_word.c

INEXtopics_to_ANTquery.exe : INEXtopics_to_ANTquery.c
	$(CC) $(CFLAGS) /EHsc /Tp INEXtopics_to_ANTquery.c ..\bin\ctypes.obj

INEXieee_assessments_to_ANT.exe : INEXieee_assessments_to_ANT.c
	$(CC) $(CFLAGS) /EHsc /Tp INEXieee_assessments_to_ANT.c ..\bin\disk.obj ..\bin\file_internals.obj

INEXassessments_get_best.exe : INEXassessments_get_best.c
	$(CC) $(CFLAGS) /EHsc /Tp INEXassessments_get_best.c ..\bin\disk.obj ..\bin\file_internals.obj

qrel_orig_alt_compare.exe : qrel_orig_alt_compare.c
	$(CC) $(CFLAGS) /EHsc /Tp qrel_orig_alt_compare.c ..\bin\disk.obj ..\bin\file_internals.obj /link /FIXED:NO

MAP_fake.exe : MAP_fake.c
	$(CC) $(CFLAGS) /EHsc /Tp MAP_fake.c


clean :
	del *.exe *.obj
