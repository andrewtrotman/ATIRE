CFLAGS =  /Zi /MTd

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

trec2query.exe : trec2query.c 
	$(CC) $(CFLAGS) /EHsc /Tp trec2query.c ..\obj\stop_word.obj ..\obj\channel_trec.obj ..\obj\channel_file.obj ..\obj\str.obj ..\obj\ctypes.obj ..\obj\memory.obj ..\obj\instream_file.obj ..\obj\instream_file_star.obj ..\obj\instream_pkzip.obj ..\obj\instream_deflate.obj ..\obj\file.obj ..\obj\file_internals.obj ..\obj\directory_iterator_pkzip.obj ..\obj\critical_section.obj ..\obj\directory_iterator.obj ..\obj\disk.obj  ..\obj\directory_iterator_internals.obj  ..\external\unencumbered\zlib\zlib-1.2.5\zlib.lib ..\external\unencumbered\bzip\bzip2-1.0.6\libbz2.lib advapi32.lib /link /FIXED:NO

INEXtopics_to_ANTquery.exe : INEXtopics_to_ANTquery.c
	$(CC) $(CFLAGS) /EHsc /Tp INEXtopics_to_ANTquery.c ..\obj\ctypes.obj

INEXieee_assessments_to_ANT.exe : INEXieee_assessments_to_ANT.c
	$(CC) $(CFLAGS) /EHsc /Tp INEXieee_assessments_to_ANT.c ..\obj\disk.obj ..\obj\file_internals.obj ..\obj\critical_section.obj

INEXassessments_get_best.exe : INEXassessments_get_best.c
	$(CC) $(CFLAGS) /EHsc /Tp INEXassessments_get_best.c ..\obj\disk.obj ..\obj\file_internals.obj ..\obj\critical_section.obj

qrel_orig_alt_compare.exe : qrel_orig_alt_compare.c
	$(CC) $(CFLAGS) /EHsc /Tp qrel_orig_alt_compare.c ..\obj\disk.obj ..\obj\file_internals.obj ..\obj\critical_section.obj /link /FIXED:NO

MAP_fake.exe : MAP_fake.c
	$(CC) $(CFLAGS) /EHsc /Tp MAP_fake.c


clean :
	del *.exe *.obj
