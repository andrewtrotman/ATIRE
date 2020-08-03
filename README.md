# ABOUT #
This is a somewhat crude dump from the soon to be defunct atire.org.  The Mercurial archive has been converted into a git archive and some of the documentation has been converted to markdown.

### License ###
ATIRE is distributed under the BSD license. However, some components are built on code distributed under the GPL license. As the GPL license is parasitic, we have done what we can to re-license or re-write those components so that they can be distributed under the BSD license. However, this is not always possible (for example, with reference implementations (e.g. lzo compression)).

### Trademark ###
ATIRE is a registered trademark.

# HOW TO BUILD ATIRE #
ATIRE builds with the native compilers on Mac (Xcode), Linux (gcc), and Windows (Visual Studio).  The build process is the same in all cases.

First, get the source code:

> git clone https://github.com/andrewtrotman/ATIRE.git

Second, build

>make

This should leave you with a numner of executable files in the bin directory. one of those tools, index, is the indexer.  Another, atire, is the search engine.

Please contact us if you need help building or running ATIRE.

# HOW TO INDEX #
## Wall Street Journal Collection ##
The WSJ collection is described in the Overview of the TREC-3. The collection is split into two CDs (disk 1 and 2), has a total size of 517MB with 173252 documents.

Use the "-rtrec" option to index the WSJ collection:
> index -sa -N10000 -rtrec wsj.xml

## 2006 Wikipedia Collection ##
The collection contains 22 .tar.gz files. There are a total of 659,388 documents.

Under Linux:

> index -sa -N10000 -rtgz main-*

Windows does not do command line expansion and so the names of the all the files must be explicitly give:

> \atire\bin\index -sa -N10000 -rtgz main-english-part-0.tar.gz main-english-part-120000.tar.gz main-english-part-150000.tar.gz main-english-part-180000.tar.gz main-english-part-210000.tar.gz main-english-part-240000.tar.gz main-english-part-270000.tar.gz main-english-part-30000.tar.gz main-english-part-300000.tar.gz main-english-part-330000.tar.gz main-english-part-360000.tar.gz main-english-part-390000.tar.gz main-english-part-420000.tar.gz main-english-part-450000.tar.gz main-english-part-480000.tar.gz main-english-part-510000.tar.gz main-english-part-540000.tar.gz main-english-part-570000.tar.gz main-english-part-60000.tar.gz main-english-part-600000.tar.gz main-english-part-630000.tar.gz main-english-part-90000.tar.gz

## 2009 Wikipedia Collection ##
The collection contains 4 parts of tar.bz2 files (each has a size of about 1.3GB). The uncompressed size is about 50.7GB. There are a total of 2,666,190 documents.

Two ways to index the collection. First, indexing the tar.bz2 files directly:
> index -sa -N10000 -rtbz2 wikipedia-2009-documents-parts-pages25.tar.bz2 wikipedia-2009-documents-parts-pages26.tar.bz2 wikipedia-2009-documents-parts-pages27.tar.bz2 wikipedia-2009-documents-parts-pages28.tar.bz2

Second, indexing the uncompressed files (assuming all files are stored in "pages" directory): 
> index -sa -N10000 -r './pages/*.xml'

Note the use of ' around the file(s) path: this prevents shell expansion but allows ATIRE to search for the files. As individual threads are created for each set of files, allowing your shell to expand the path may result in too many threads being created (e.g., over 1000).

## WT10G Collection ##
The collection has a total of 2.7GB compressed and 10GB uncompressed. There are 1,692,096 documents.

Use this command to index the collection (assume all files are stored in "data" directory): 
> index -N10000 -rrtrec:clean './data/*.gz'

Note the use of ' around the file(s) path: this prevents shell expansion but allows ATIRE to search for the files. As individual threads are created for each set of files, allowing your shell to expand the path may result in too many threads being created (e.g., over 1000).

## WT100g/VLC2 Collection ##
The collection has a total size of 33GB compressed and 100GB uncompressed. There are 18,571,671 documents.

If your system has enough memory without using swap space, use this command to index the collection (assume all files are stored in "Data" directory): 
> index -N10000 -rrtrec:clean './Data/*.gz'

Note the use of ' around the file(s) path: this prevents shell expansion but allows ATIRE to search for the files. As individual threads are created for each set of files, allowing your shell to expand the path may result in too many threads being created (e.g., over 1000).

In case your system doesn't have enough memory, then you should split the collection into parts and index one part at a time and then merge these indexes together. The size of each part depends on how much memory is available. Assume there are four parts under the directory "parts": 
> index -sa -N10000 -findex index-0.aspt -fdoclist doclist-0.aspt -rrtrec:clean 'parts/part-0/*.gz' 
> index -sa -N10000 -findex index-1.aspt -fdoclist doclist-1.aspt -rrtrec:clean 'parts/part-1/*.gz' 
> index -sa -N10000 -findex index-2.aspt -fdoclist doclist-2.aspt -rrtrec:clean 'parts/part-2/*.gz' 
> index -sa -N10000 -findex index-3.aspt -fdoclist doclist-3.aspt -rrtrec:clean 'parts/part-3/*.gz' 

Then merge the indexes together:
> atire_merge -N100000 `ls index-*` 
or alternatively for a small number of index files to merge (like the example above):
> atire_merge -N100000 index-0.aspt index-1.aspt index-2.aspt index-3.aspt

And finally if the indexes don't include the docids (merge should warn if they don't) then combine the doclists together (note: the doclists have to be combined in the same relative order as the indexes were given):
> cat `ls doclist-*` > merged_doclist.aspt 
or
> cat doclist-0.aspt doclist-1.aspt doclist-2.aspt doclist-3.aspt > merged_doclist.aspt

## .GOV2 Collection ##
The collection has a total size of 81GB compressed and 400GB uncompressed. There are 25,205,179 documents.

The instructions for indexing .GOV2 collection is the same as the instructions for WT100g. You might need to split the collection into more parts depending on how much memory your system has.

# INDEX STRUCTURE #
Each index file begins with the string "ANT Search Engine Index File\n\0\0". Performing head -n1 index.aspt will show this string on all valid ATIRE index files.

In general the index file contains the postings followed by the dictionary and then the header, although the dictionary second level and postings may be intertwined.


## Header ##
Despite the name, the header for each index file is at the end of the document. The header contains the following data in the given order (with the file signature occupying the last 4 bytes of the file):

| Variable | Number of Bytes | Notes |
|-----------|--------------------|---------|
| Position in the file of the first dictionary level | 8 | |	
| Length of the longest term | 4 | |	
| Number of unique terms | 4 | This only exists in version 0.4 of the index file (IMPACT_HEADER defined) |
| Length of the longest compressed postings list in bytes | 4 | |	
| Highest document frequency | 8 | |	
| Reserved | 8 | Possible future use: checksum |
| Reserved | 8 | Possible future use: collection name, currently set to 0x494e444558000000, or "INDEX" in Intel byte order |
| Version of the index | 4 | Used to make sure search engine will be able to process index |
| File signature | 4 | Used to test the validity of the index |

## Dictionary ##
There are two levels to the dictionary in ATIRE. On disk the second level is stored first, then the top level. The top level is always loaded into memory on start-up.

### Top Level ###
There are 8 bytes dedicated to storing the number of nodes that are contained in the top level of the tree.

Then for each node, the first B_TREE_PREFIX_SIZE bytes (currently set to 4), or the whole term if smaller, are stored followed by the NUL terminator. Then 8 bytes for the position on disk of the second level for terms that begin with the prefix in this node (for instance, shee -> sheen, sheep, sheepish, sheet etc.)

### Second Level ###
Each section of the second level begins with 4 bytes containing the number of terms in this section. Then for each term in the node:

Variable	Number of Bytes	Notes
Collection frequency	4	
Document frequency	4	
Position of postings on disk	8	DOCID_1 << 32 | IMPACT_1
Length of postings	4	DOCID_2
Length of postings in bytes	4	IMPACT_2
Term local max impact	1	Only if TERM_LOCAL_MAX_IMPACT is defined.
Position of string in string block	4	
Then the strings for the tails of the terms in this node, NUL terminated (following the example above, n, p, pish, t etc.

In the case where SPECIAL_COMPRESSION is defined, then the postings position and postings length variables get replaced by the respective docid and impact values, as shown in the above table.

## Postings ##
The structure of the postings depends largely on whether or not IMPACT_HEADER is defined, and whether the term is special (begins with "~")

### ~ ###
Terms that begin with ~ denote variables to be used within the search engine and are not directly searchable.

These terms are neither difference encoded, or impacted ordered. The search engine allows these variables to be get and set with {get,set}_variable methods, which allows storage of arbitrary 64-bit values.

See known issues for some known issues with regards to ~ variables.

### Impact Header ###
Each postings list is preceded by an impact header:

Variable	Number of Bytes	Notes
Postings chain	8	For chaining postings lists together, currently unused
Chain length	8	As above
Quantum count	4	
Beginning of the postings	4	Pointer to the beginning of the postings on disk
Following this data is a compressed block of 3 * quantum_count values the contain the impact values, document counts and postings start.

The impact values are the tf values, or the option specified by -Q when indexing. The document counts contain the number of documents for each impact value. The postings start is a location on disk for the postings for that impact value. That is, there are document_counts[i] documents with impact impact_values[i] and the docids start at postings_start[i]. The docids are difference encoded for each impact. Each docid list for each impact value is compressed separately.

### Non Impact Header ###
Postings lists are impact ordered, and stored in decreasing order of impact, with 0 identifying the end of the docid list for a given impact. For example: impact, docid, 0, impact, docid, docid, 0. Docid's are difference encoded for each impact, and the whole impact ordering is compressed.

