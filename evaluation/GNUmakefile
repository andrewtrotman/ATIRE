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

between : between.c

mkqrel : mkqrel.c

trec2query : trec2query.c stop_word.c
	$(CC) $(CFLAGS) trec2query.c stop_word.c -o trec2query

INEXtopics_to_ANTquery : INEXtopics_to_ANTquery.c
	$(CC) $(CFLAGS) INEXtopics_to_ANTquery.c ../obj/ctypes.o -o INEXtopics_to_ANTquery

INEXieee_assessments_to_ANT : INEXieee_assessments_to_ANT.c
	$(CC) $(CFLAGS) INEXieee_assessments_to_ANT.c ../obj/disk.o ../obj/file_internals.o -o INEXieee_assessments_to_ANT

INEXassessments_get_best : INEXassessments_get_best.c
	$(CC) $(CFLAGS) INEXassessments_get_best.c ../obj/disk.o ../obj/file_internals.o -o INEXassessments_get_best

qrel_orig_alt_compare : qrel_orig_alt_compare.c
	$(CC) $(CFLAGS) qrel_orig_alt_compare.c ../obj/disk.o ../obj/file_internals.o -o qrel_orig_alt_compare

MAP_fake : MAP_fake.c
	$(CC) $(CFLAGS) MAP_fake.c -o MAP_fake

clean :
	\rm -f *.o $(TARGETS)
