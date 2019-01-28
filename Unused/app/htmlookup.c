#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <math.h>

#define IDSIZE 64

typedef double				float64;

#ifdef _WIN32
typedef __int64				int64;
typedef unsigned __int64	uint64;

typedef __int32 int32;
typedef unsigned __int32 uint32;
#else
typedef long long			int64;
typedef unsigned long long	uint64;

typedef long int32;
typedef unsigned long uint32;
#define IDHIGHBIT  0x8000000000000000LL
#define IDHIGHBIT2 0x4000000000000000LL
#endif

#ifndef SpatialWinNT
#   ifdef SpatialDigitalUnix
#      define PRINTID(x) printf("%lu",(x))
#      define PRINTID_HEX(x) printf("%lX",(x))
#   else
#      define PRINTID(x) printf("%llu",(x))
#      define PRINTID_HEX(x) printf("%llX",(x))
#      define SCANID(x) scanf("%llu", &x)
#   endif
#else
#   define PRINTID(x) printf("%I64u",(x))
#   define PRINTID_HEX(x) printf("%I64X",(x))
#endif

extern uint64 cc_radec2ID(double ra, double dec, int depth);
extern int cc_ID2name(char *name, uint64 htmid);

char name[256];

extern unsigned long long cc_name2ID(const char *name);

int main(int argc, char *argv[])
{
  double ra, dec;
  char htm_name[1024]; // plenty of room
  int depth = 20;
  uint64 htmid;
  while(2 == scanf("%lf %lf", &ra, &dec)){
    htmid = cc_radec2ID(ra, dec, depth);
    PRINTID(htmid);
    cc_ID2name(htm_name, htmid);
    printf(" %s\n", htm_name);
  }
  return 0;
}
