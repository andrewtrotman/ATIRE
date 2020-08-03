# ABOUT #
This is a somewhat crude dump from the soon to be defunct atire.org.  The Mercurial archive has been converted into a git archive and some of the documentation has been converted to markdown.

### License ###
ATIRE is distributed under the BSD license. However, some components are built on code distributed under the GPL license. As the GPL license is parasitic, we have done what we can to re-license or re-write those components so that they can be distributed under the BSD license. However, this is not always possible (for example, with reference implementations (e.g. lzo compression)).

### Trademark ###
ATIRE is a registered trademark.

# How to build ATIRE #
ATIRE builds with the native compilers on Mac (Xcode), Linux (gcc), and Windows (Visual Studio).  The build process is the same in all cases.

First, get the source code:

> git clone https://github.com/andrewtrotman/ATIRE.git

Second, build

>make

This should leave you with a numner of executable files in the bin directory. one of those tools, index, is the indexer.  Another, atire, is the search engine.

Please contact us if you need help building or running ATIRE.

# How to index #
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
