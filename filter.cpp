#include <iostream> // cout
#include <fstream> // ifsteram
#include <iomanip>  // setw()
#include <stdlib.h>   // rand(), drand48() but on the mac osX there
                      // is no drand48 yet

#include <HtmRange.h>

#define SCANRANGE(x, y) scanf("%llu %llu", &x, &y)

/*
 * For examples of exercising Range and Skiplists's methods, see a, b, c
 * (a.cpp, etc)
 */

void skipCRLF(ifstream &in)
{
  char c = in.peek();
  while (c == 10 || c == 13) {
    in.ignore();
    c = in.peek();
  }
}


int main(int argc, char *argv[])
{
  // run as a filter only
  uint64 lo, hi;
  HtmRange ran;
  int desire;
  Key gapsize;
  int lines = 0;

  desire = 100;
  if (argc > 1){
    desire = atoi(argv[1]);
  }

  while(2 == SCANRANGE(lo, hi)){
      ran.mergeRange(lo, hi);
      lines++;
  }

  //cerr << "Lines read: " << lines << endl;
  //cerr << ran;

  gapsize = ran.bestgap(desire);
  // cerr <<  "Gapsize = " << gapsize << endl;
  ran.defrag(gapsize);
  cout << ran;
  return 0;
}
