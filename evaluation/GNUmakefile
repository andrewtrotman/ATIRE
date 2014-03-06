CC := g++

TARGETS := 	between 						\
		mkqrel 							\
		trec2query 						\
		INEXtopics_to_ANTquery 			\
		INEXieee_assessments_to_ANT 	\
		INEXassessments_get_best 		\
		qrel_orig_alt_compare			\
		MAP_fake

all : $(TARGETS)

between : between.o

mkqrel : mkqrel.o

trec2query : trec2query.o
	$(CC) trec2query.o ../obj/str.o ../obj/stop_word.o ../obj/channel_file.o ../obj/channel_trec.o ../obj/ctypes.o ../obj/memory.o ../obj/file.o ../obj/file_internals.o ../obj/instream_deflate.o ../obj/instream_file.o ../obj/instream_file_star.o ../obj/instream_pkzip.o ../obj/directory_iterator.o ../obj/directory_iterator_pkzip.o ../obj/directory_iterator_internals.o ../obj/disk.o ../external/unencumbered/zlib/libz.a ../external/unencumbered/bzip/libbz2.a -o trec2query

INEXtopics_to_ANTquery : INEXtopics_to_ANTquery.o
	$(CC) INEXtopics_to_ANTquery.o ../obj/ctypes.o -o INEXtopics_to_ANTquery

INEXieee_assessments_to_ANT : INEXieee_assessments_to_ANT.o
	$(CC) INEXieee_assessments_to_ANT.o ../obj/disk.o ../obj/file_internals.o -o INEXieee_assessments_to_ANT

INEXassessments_get_best : INEXassessments_get_best.o
	$(CC) INEXassessments_get_best.o ../obj/disk.o ../obj/file_internals.o -o INEXassessments_get_best

qrel_orig_alt_compare : qrel_orig_alt_compare.o
	$(CC) qrel_orig_alt_compare.o ../obj/disk.o ../obj/file_internals.o -o qrel_orig_alt_compare

MAP_fake : MAP_fake.o
	$(CC) MAP_fake.o -o MAP_fake

%.o : %.c
	$(CC) -g -x c++ -c $<

clean :
	\rm -f *.o $(TARGETS)
