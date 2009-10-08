VAL MEM.INDEX.HASH.TABLE.SIZE IS 65536:
VAL MEM.INDEX.MAX.POSTINGS IS 10000000:
VAL MEM.INDEX.MAX.TERMS IS 1000000:
VAL MEM.INDEX.MAX.COMPRESSED.SIZE IS 1000000:
VAL MEM.INDEX.TERMS.PER.NODE IS 1000:
VAL MEM.INDEX.HEADER.SIZE IS 1024:

DATA TYPE mem.index.term.ptr IS INT:
DATA TYPE mem.index.term
  RECORD
    [PARSER.MAX.TOKEN.LEN]BYTE term:
    INT first.posting:
    INT last.posting:
    INT32 disk.location:
    BOOL root.node:
    mem.index.term.ptr left:
    mem.index.term.ptr right:
:

DATA TYPE mem.index.postings.ptr IS INT:
DATA TYPE mem.index.posting
  RECORD
    INT32 docid:
    INT occurences:
    mem.index.postings.ptr next.posting:
:

DATA TYPE mem.index
  RECORD
    [MEM.INDEX.MAX.POSTINGS]mem.index.posting postings:
    mem.index.postings.ptr last.posting:

    [MEM.INDEX.MAX.TERMS]mem.index.term terms:
    mem.index.term.ptr last.term:

    [MEM.INDEX.HASH.TABLE.SIZE]mem.index.term.ptr hash.table:
:
