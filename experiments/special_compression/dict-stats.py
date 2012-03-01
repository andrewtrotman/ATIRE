#
# 01-Mar-2012
#
# This script reads a dictionary file produced from atire_dictioanry
# and reports simple stats
#
from __future__ import division
import sys

if __name__ == '__main__':

	dict_file = ''
	total_terms_occurred_once = 0
	total_terms_occurred_twice = 0
	total_unique_terms = 0

	#
	# read dictionary file from command line
	#
	if len(sys.argv) < 2:
		print 'Please specify the dictioanry file produced by atire_dictionary'
	dict_file = sys.argv[1]
	file = open(dict_file, 'r')
	contents = file.read().splitlines()

	#
	# parse all the terms in the dictionary
	#
	for line in contents:
		# in format of ['term', 'collection_freq', 'document_freq']
		items = line.split()
		term = items[0]
		collection_freq = int(items[1])
		document_freq = int(items[2])
		#print term, collection_freq, document_freq
		if term[0] == '~':
			if term == '~length':
				total_unique_terms = document_freq
		else:
			if document_freq == 1:
				total_terms_occurred_once += 1
			elif document_freq == 2:
				total_terms_occurred_twice += 1
		pass # end of if term[0] == '~'
	pass # end of for lin in contents

	#
	# print out the stats
	#
	print 'Total unique terms: %s' % total_unique_terms
	percentage = total_terms_occurred_once / total_unique_terms * 100
	print 'Total terms occurred once: %d (%%%.2f)' % (total_terms_occurred_once, percentage)
	percentage = total_terms_occurred_twice / total_unique_terms * 100
	print 'total terms occurred twice: %d (%%%.2f)' % (total_terms_occurred_twice, percentage)
	percentage = (total_terms_occurred_once + total_terms_occurred_twice) / total_unique_terms * 100;
	print 'total terms occurred either once or twice: %d (%%%.2f)' % (total_terms_occurred_once + total_terms_occurred_twice, percentage)

	file.close()
