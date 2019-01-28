#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <math.h>

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


extern long long cc_vector2ID(double x, double y, double z, int depth);
extern int cc_name2Triangle(char *name, double *v0, double *v1, double *v2);
extern int cc_ID2name(char *name, long long id);
extern unsigned long long cc_name2ID(const char *name);

char name[256];


extern unsigned long long cc_name2ID(const char *name);

int main(int argc, char *argv[])
{
  int addlevel = 0;
  unsigned long long id;
  
  if(argc > 1){
    addlevel = atoi(argv[1]);
  }
  while(1 == scanf("%s", name)){

/*     PRINTID(lo); */
/*     printf(" "); */
/*     PRINTID(hi); */
/*     printf("\n"); */

    id = cc_name2ID(name);
    printf("%s = ", name);
    PRINTID(id);
    printf("is at level %d\n", strlen(name) - 2);
    if(addlevel > 0){
      unsigned long long lo, hi;
      int shifts;
      shifts = addlevel << 1;
      lo = id << shifts;
      hi = lo + (1 << shifts) -1;
      printf("   %d more levels:  ", addlevel);
      PRINTID(lo);
      printf(" - ");
      PRINTID(hi);
      printf("\n");
    }
  }
  return 0;
}
