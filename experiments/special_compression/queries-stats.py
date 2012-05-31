#
# 04-Mar-2012
#
# This script reads a dictionary file produced from atire_dictioanry
# and reports simple stats
#
from __future__ import division
import sys
import os

def usage():
	print "./%s --dict-file <dict-file> --queries-file <queries-file> [--queries-has-id]" % os.path.basename(sys.argv[0])
pass # end of def usage()


def bsearch(the_list, key, low, high):
	if high < low:
		return 0, None

	mid = (low + high) // 2
	# in tuple of (term, document_freq)
	term, document_freq = the_list[mid]
	if term == key:
		return 1, the_list[mid]
	elif term > key:
		return bsearch(the_list, key, low, mid-1)
	else:
		return bsearch(the_list, key, mid+1, high)
pass


if __name__ == '__main__':

	total_terms_occurred_once = 0
	total_terms_occurred_twice = 0
	total_unique_terms = 0
	collection_size = 0

	#
	# read dictionary file from command line
	#
	if len(sys.argv) < 5:
		usage()
		sys.exit(2)

	dict_file = None
	queries_file = None
	queries_has_id = 0

	i = 1
	while i < len(sys.argv):
		if '--dict-file' == sys.argv[i]:
			i += 1
			dict_file = open(sys.argv[i], 'r')
			i += 1
		elif '--queries-file' == sys.argv[i]:
			i += 1
			queries_file = open(sys.argv[i], 'r')
			i += 1
		elif '--queries-has-id' == sys.argv[i]:
			i += 1
			queries_has_id = 1
		else:
			print "Argument '%s' is not supported" % sys.argv[i]
			usage()
			sys.exit(2)
	pass

	#
	# parse all the terms in the dictionary
	#
	all_dict_terms = list([])
	#all_dict_terms = [None] * 40650000
	contents = dict_file.read().splitlines()
	counter = 0
	for line in contents:
		items = line.split()
		term = items[0]
		collection_freq = int(items[1])
		document_freq = int(items[2])

		all_dict_terms.append((term, document_freq))
		if counter % 100000 == 0:
			print "counter: %d" % counter
		counter += 1

		if '~' == term[0]:
			if '~length' == term:
				collection_size = document_freq
		else:
			total_unique_terms += 1
			if document_freq == 1:
				total_terms_occurred_once += 1
			elif document_freq == 2:
				total_terms_occurred_twice += 1
		pass # end of if '~' != term[0]
	pass # end of for lin in contents

	#
	# print out the stats
	#
	print 'Dictioanry Stats'
	print '======================================================='
	print 'Collection size: %d' % collection_size
	print 'Total unique terms: %s' % total_unique_terms
	percentage = total_terms_occurred_once / total_unique_terms * 100
	print 'Total terms occurred once: %d (%%%.2f)' % (total_terms_occurred_once, percentage)
	percentage = total_terms_occurred_twice / total_unique_terms * 100
	print 'total terms occurred twice: %d (%%%.2f)' % (total_terms_occurred_twice, percentage)
	percentage = (total_terms_occurred_once + total_terms_occurred_twice) / total_unique_terms * 100;
	print 'total terms occurred either once or twice: %d (%%%.2f)' % (total_terms_occurred_once + total_terms_occurred_twice, percentage)

	dict_file.close()

	#
	# collect stats for the queries
	#
	num_of_queries = 0
	once_queries = 0 # queries containing terms that occurred only once
	twice_queries = 0 # queries containing terms that occurred only twice
	longest_query = 0
	shortest_query = 0
	total_terms = 0
	total_unique_terms = 0
	total_terms_occurred_once = 0
	total_terms_occurred_twice = 0

	all_dict_terms_len = len(all_dict_terms)

	queries_terms = dict({})

	contents = queries_file.read().splitlines()
	for line in contents:

		num_of_queries += 1

		items  = line.split()
		if queries_has_id:
			items = items[1:]

		item_count = len(items)
		total_terms += item_count
		if item_count > longest_query:
			longest_query = item_count
		if not shortest_query:
			shortest_query = item_count
		elif shortest_query > item_count:
			shortest_query = item_count

		once_term_happened = False
		twice_term_happend = False
		for t in items:
			queries_terms[t] = 1
			found, term_tuple = bsearch(all_dict_terms, t, 0, all_dict_terms_len)
			if found:
				term, document_freq = term_tuple
				if document_freq == 1:
					total_terms_occurred_once += 1
					once_term_happened = True
				elif document_freq == 2:
					total_terms_occurred_twice += 1
					twice_term_happend = True
		pass # end of for t in items

		if twice_term_happend:
			twice_queries += 1
		elif once_term_happened:
			once_queries += 1
	pass # end of for line in contents

	total_unique_terms = len(queries_terms)

	print ''
	print 'Queries Stats'
	print '======================================================='
	print "Number of queries: %d" % num_of_queries
	print "Longest queries; %d" % longest_query
	print "Shorest queries: %d" % shortest_query
	print "Total terms: %d" % total_terms
	print "Total unique terms: %d" % total_unique_terms
	print "Total terms occurred once in the dict: %d" % total_terms_occurred_once
	print "Total terms occurred twice in the dict: %d" % total_terms_occurred_twice
	print "Queries containing terms occurred once: %s" % once_queries
	print "Queries containing terms occurred twice: %d" % twice_queries

	queries_file.close()

