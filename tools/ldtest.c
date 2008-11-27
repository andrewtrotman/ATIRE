#include <stdio.h>
#include <basetsd.h>/* required typedef __int64 for gcc */

typedef char TCHAR;

#define TEXT(x) x
unsigned __int64 atou64(const TCHAR *s) ;
int main()
{
  unsigned __int64 value;
  TCHAR *test = "1234567890123456789";
  printf("in main(): test = <%s>\n", test);
  printf("sizeof(__int64) = %d\n", sizeof(__int64));
  value = 1234567890123456789;
  printf("value using I64d: %I64d\n", value); /* for Borland BCC or Microsoft VC++ */
  printf("value using Ld:   %Ld\n", value); /* for Borland BCC */
  printf("value using lld:  %lld\n", value); /* for gcc */
  printf("value using I64x: %016I64x(hex)\n", value);/* for Borland BCC or Microsoft VC++ */
  printf("value using Lx:   %016Lx(hex)\n", value);/* for Borland BCC */
  printf("value using llx:  %016llx(hex)\n", value); /* for gcc */
  return 0;
}
