#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "dwarf_eh.h"

static void signed_value(unsigned char *bytes, size_t size, int64_t expected) {
  unsigned char *cursor=bytes;
  assert(read_sleb128(&cursor)==expected);
  assert(cursor==bytes+size);
}

static void fixed_width_value(char encoding, const void *value, size_t size,
                              uint64_t expected) {
  _Alignas(uint64_t) unsigned char bytes[sizeof(uint64_t) + 1];
  unsigned char *cursor = bytes + 1;
  memcpy(cursor, value, size);
  assert(read_value(encoding, &cursor) == expected);
  assert(cursor == bytes + 1 + size);
}

static void unaligned_values(void) {
  uint16_t u16 = UINT16_MAX;
  uint32_t u32 = UINT32_MAX;
  uint64_t u64 = UINT64_MAX;
  int16_t s16 = INT16_MIN;
  int32_t s32 = INT32_MIN;
  int64_t s64 = INT64_MIN;
  uintptr_t address = (uintptr_t)&u64;
  fixed_width_value(DW_EH_PE_udata2, &u16, sizeof(u16), u16);
  fixed_width_value(DW_EH_PE_udata4, &u32, sizeof(u32), u32);
  fixed_width_value(DW_EH_PE_udata8, &u64, sizeof(u64), u64);
  fixed_width_value(DW_EH_PE_sdata2, &s16, sizeof(s16), (uint64_t)s16);
  fixed_width_value(DW_EH_PE_sdata4, &s32, sizeof(s32), (uint64_t)s32);
  fixed_width_value(DW_EH_PE_sdata8, &s64, sizeof(s64), (uint64_t)s64);
  fixed_width_value(DW_EH_PE_absptr, &address, sizeof(address), address);

  _Alignas(uintptr_t) unsigned char slot[sizeof(address) + 1];
  memcpy(slot + 1, &address, sizeof(address));
  assert(resolve_indirect_value(NULL, DW_EH_PE_absptr | DW_EH_PE_indirect,
                               (uintptr_t)(slot + 1), NULL) == address);
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
  unaligned_values();
  return 0;
}
