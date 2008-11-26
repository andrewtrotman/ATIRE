all : between.exe mkqrel.exe trec2query.exe

between.exe : between.c

mkqrel.exe : mkqrel.c

trec2query.exe : trec2query.c stop_word.c
	$(CC) $(CFLAGS) /EHsc /TP trec2query.c stop_word.c



clean :
	del *.exe *.obj
