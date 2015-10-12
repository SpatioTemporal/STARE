#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
typedef __int64				int64;
typedef unsigned __int64	uint64;
#else
typedef long long			int64;
typedef unsigned long long	uint64;
#endif


#ifndef SpatialWinNT
#   define IDHIGHBIT  0x8000000000000000LL
#   define IDHIGHBIT2 0x4000000000000000LL
#   ifdef SpatialDigitalUnix
#      define PRINTID(x) printf("%lu",(x))
#      define PRINTID_HEX(x) printf("%lX",(x))
#   else
#      define PRINTID(x) printf("%llu",(x))
#      define PRINTID_HEX(x) printf("%llX",(x))
#      define SCANRANGE(x, y) scanf("%llu %llu", &x, &y)
#   endif
#else
#   define PRINTID(x) printf("%I64u",(x))
#   define PRINTID_HEX(x) printf("%I64X",(x))
#endif


int main(int argc, char *argv[])
{
  // run as a filter only
  long long lo, hi;
  long long i;
  int summary = 0;

  if(argc > 1){
    if (argv[1][0] == '-'){
      summary++;
    }
  }
  while(2 == SCANRANGE(lo, hi)){
    if(summary){
      PRINTID(lo);
      printf(" ");
      PRINTID(hi);
      printf("  del = ");
      PRINTID(hi - lo);
      printf("\n");
    } else {
      for(i=lo; i<=hi; i++){
	PRINTID(i);
	printf("\n");
      }
    }
  }
  return 0;
}
