all : between.exe mkqrel.exe trec2query.exe INEXtopics_to_ANTquery.exe

between.exe : between.c

mkqrel.exe : mkqrel.c

trec2query.exe : trec2query.c stop_word.c
	$(CC) $(CFLAGS) /EHsc /TP trec2query.c stop_word.c

INEXtopics_to_ANTquery.exe : INEXtopics_to_ANTquery.c
	$(CC) $(CFLAGS) /EHsc /Tp INEXtopics_to_ANTquery.c ..\bin\ctypes.obj



clean :
	del *.exe *.obj
