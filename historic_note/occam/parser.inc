VAL MEGABYTE IS 1024 * 1024:
VAL PARSER.MAX.TOKEN.LEN IS 80:

DATA TYPE parser.object
  RECORD
    [128 * MEGABYTE]BYTE document:
    INT token.start:
:

