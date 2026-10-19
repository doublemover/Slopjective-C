#include "private.h"
#ifdef __OBJC__
@import fixture.Leaf;
#endif
int right(void) {
#ifdef __OBJC__
  return @fixture.Leaf.leaf() + 2;
#else
  return leaf() + 2;
#endif
}
