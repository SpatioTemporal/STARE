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
extern int cc_IDlevel(long long id);


char name[256];


extern unsigned long long cc_name2ID(const char *name);

int main(int argc, char *argv[])
{

  unsigned long long id;
  unsigned long long targetLevel = 20, addlevel, level;
  if (argc > 1)
    targetLevel = atoi(argv[1]);

  while(1 == SCANID(id)){

/*     PRINTID(lo); */
/*     printf(" "); */
/*     PRINTID(hi); */
/*     printf("\n"); */

    cc_ID2name(name, id);    
    level = strlen(name) - 2;
    addlevel = targetLevel - level;
    if (addlevel < 0) addlevel = 0;
    printf("%s level %llu (%d) \n", name, level, cc_IDlevel(id));

    if(addlevel > 0){
      unsigned long long lo, hi, del;
      int shifts;
      // shifts = addlevel << 1LL;
      shifts = addlevel << 1;
      lo = id << shifts;
      hi = lo + (1LL << shifts) -1;
      printf("   %llu more levels:  ", addlevel);
      PRINTID(lo);
      printf(" ");
      PRINTID(hi);
      del = hi - lo;
      printf("   del = ");
      PRINTID(del);
      printf("\n");
      printf("   (%d %d)\n", cc_IDlevel(lo), cc_IDlevel(hi));
      {
	char name[1024];
	unsigned long long i;
	for(i = lo; i<=hi; i++){
	  cc_ID2name(name, i);    
	  printf("     \"%s\"\n", name);
	}

      }
      addlevel--;
    }
    else {
      PRINTID(id);
      printf(" ");
      PRINTID(id);
      printf("\n");
    }
  }
  return 0;
}
