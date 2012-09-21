#
# This script is used to map page rank values to doclist
#
# (1) call link_extract_inex_wikipedia to generate the links
# (2) call link_ayalysis_inex_wikipedia to generate page rank results
# (3) call index to genreate the doclist and the index file
# (4) call this script to map the page rank results to the doclist
# (5) call generate_pregen to generate a pregen file
#         ./genereate_pregen doclist.aspt mapped-doclist.aspt pagerank
# (6) calll atire to run the search engine with the pregen
#         ./atire -pregen pagerank -Rpregen:pagerank
#

import re
import sys
import os
import math

def usage(prog_name):
    print "Usage: %s <doclist file> <page rank file> [output file]" % os.path.basename(prog_name)
    sys.exit()

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

#
# Read the page rank results generated from the command link_ayalysis_inex_wikipedia
#
def read_page_ranks(pg_filename, all_pagerank_values):
    f = open(pg_filename, 'r')
    contents = f.readlines()
    value = None
    node = None
    maximum_value = 0.0
    minimum_value = sys.maxint
    for line in contents:
        result = PAGE_RANK_PATTERN.search(line)
        if result != None:
            value = float(result.group(1))
            if value > maximum_value:
                maximum_value = value
            if minimum_value > value:
                minimum_value = value
            node = int(result.group(2))
            all_pagerank_values.append((node, value))
    f.close()
    return (minimum_value, maximum_value)

#
# scale the page rank
#
def scale_page_rank(value, linear_scale):
    return int(math.ceil(math.log(value*linear_scale)))

#
# map the page rank results to the doclist
#
def map_page_ranks(doclist_filename, all_pagerank_values, output_filename, linear_scale):
    doclists = open(doclist_filename, 'r').read().splitlines()
    doclists.sort()
    count = len(all_pagerank_values)-1
    f = open(output_filename, 'w')
    i = 0
    for line in doclists:
        if i % 100000 == 0:
            print i
        # doclist is in format of "pages/130/2323130.xml"
        # line.rfind('/')+1 is used to find the beginning of the page number
        # -4 is used to remove the ".xml" extension
        node = int(line[line.rfind('/')+1:-4])
        found, page_rank_value = bsearch(all_pagerank_values, node, 0, count)
        if found:
            #print '%s %d %d' % (line, node, scale_page_rank(page_rank_value[1], linear_scale))
            f.write('%s %s\n' % (line, 1+scale_page_rank(page_rank_value[1], linear_scale)))
        else:
            #print '%s %d 0' % (line, node)
            f.write('%s 1\n' % line)
        i = i+1
    f.close()


#
# the main
#
if __name__ == '__main__':
    PAGE_RANK_PATTERN = re.compile(r'\s*PageRank\s*:\s*([0-9.e-]*)\s*Node:\s*([0-9]*)', re.IGNORECASE)
    pg_filename = None
    output_filename = 'mapped-doctlist.aspt'

    if len(sys.argv) < 3:
        print usage(sys.argv[0])

    doclist_filename = sys.argv[1]
    pg_filename = sys.argv[2]

    if len(sys.argv) == 4:
        output_filename = sys.argv[3]

    all_pagerank_values = list([])
    print "start reading page ranks"
    minimum_page_rank, maximum_page_rank = read_page_ranks(pg_filename, all_pagerank_values)
    print 'minimum_page_rank: ', minimum_page_rank, 'maximum_page_rank: ', maximum_page_rank
    all_pagerank_values.sort()
    print "finished reading page ranks"

    linear_scale = 1
    min = minimum_page_rank
    while min < 1.0:
        min = min * 10
        linear_scale = linear_scale * 10

    print 'linear scale: ', linear_scale

    print "start mapping page ranks"
    map_page_ranks(doclist_filename, all_pagerank_values, output_filename, linear_scale)
    print "finished mapping page ranks"




