--
-- PROTOCOL STRING
-- ---------------
--
PROTOCOL string
  CASE
    line; INT32::[]BYTE
    eof
:

DATA TYPE term.t
  RECORD
    INT32 docid:
    [PARSER.MAX.TOKEN.LEN]BYTE term:
:

PROTOCOL index.term
  CASE
    term; term.t
    eof
:
