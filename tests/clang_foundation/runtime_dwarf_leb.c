#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "dwarf_eh.h"

static void signed_value(unsigned char *bytes, size_t size, int64_t expected) {
  unsigned char *cursor=bytes;
  assert(read_sleb128(&cursor)==expected);
  assert(cursor==bytes+size);
}
int main(void) {
  unsigned char zero[]={0};
  unsigned char negative_one[]={0x7f};
  unsigned char positive[]={0xe5,0x8e,0x26};
  unsigned char negative[]={0x9b,0xf1,0x59};
  unsigned char maximum[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0};
  unsigned char minimum[]={0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x7f};
  unsigned char full_width[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,1};
  unsigned char *cursor=full_width;
  int bits=0;
  signed_value(zero,sizeof(zero),0);
  signed_value(negative_one,sizeof(negative_one),-1);
  signed_value(positive,sizeof(positive),624485);
  signed_value(negative,sizeof(negative),-624485);
  signed_value(maximum,sizeof(maximum),INT64_MAX);
  signed_value(minimum,sizeof(minimum),INT64_MIN);
  assert(read_leb128(&cursor,&bits)==UINT64_MAX);
  assert(cursor==full_width+sizeof(full_width) && bits==70);
  return 0;
}
