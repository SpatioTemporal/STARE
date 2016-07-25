#include <HtmRangeMultiLevelIterator.h>

Key HtmRangeMultiLevelIterator::next()
{
  Key key = this->nextval;
  getNext();
  return (Key) key;
}

void HtmRangeMultiLevelIterator::getNext()
{
  if (currange[0] <= 0){
    nextval = -1;
    return;
  }
  nextval++;
  if (nextval > currange[1]){
    range->getNext(&currange[0], &currange[1]);
    if (currange[0] <= 0){
      nextval = -1;
      return;
    }
    nextval = currange[0];
  }
  return;
}

char * HtmRangeMultiLevelIterator::nextSymbolic(char *buffer)
{
  Key key = this->next();
  if (key <= 0) {
    buffer[0] = '\0';
  } else {
	  strcpy(buffer,range->encoding->nameById(key));
  }
  return buffer;
}
bool HtmRangeMultiLevelIterator::hasNext()
{
  return (nextval > 0);
}


// HtmRangeMultiLevelIterator::
