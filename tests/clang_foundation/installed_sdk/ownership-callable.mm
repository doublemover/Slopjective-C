#ifndef IR_OWNERSHIP_PCH
#include "ownership-callable.h"
#endif
#include "carrier-record-call.h"

static int read_borrowed(Loan value) { return value[0]; }
static int read_plain(const char *value) { return value[1]; }

__attribute__((objc_returns_borrowed(owner_index=0)))
static Loan read_owner(id owner) {
  static const char value[] = "OK";
  (void)owner;
  return value;
}

#pragma pack(push, 1)
struct PackedStrongLeaf {
  char marker;
  RecordStrong object;
};
#pragma pack(pop)
static_assert(__builtin_offsetof(PackedStrongLeaf, object) == 1);

__attribute__((noinline))
static PackedStrongLeaf copy_packed(const PackedStrongLeaf &source) {
  return source;
}

__attribute__((noinline))
static PackedStrongLeaf move_packed(PackedStrongLeaf &source) {
  return static_cast<PackedStrongLeaf &&>(source);
}

static int borrowed_calls() {
  char bytes[] = {'O', 'K', 0};
  BorrowingReader borrowed_reader = read_borrowed;
  PlainReader plain_reader = read_plain;
  static_assert(!__is_same(BorrowingReader, PlainReader));
  if (count_calls(borrowed_reader) != 1 || count_calls(plain_reader) != 1 ||
      count_calls<BorrowingReader>(borrowed_reader) != 2)
    return 1;
  BorrowingBlock block = ^int(Loan input) { return input[1]; };
  if (observe(borrowed_reader, bytes) != 'O' ||
      observe<BorrowingBlock>(block, bytes) != 'K')
    return 2;
  OwnerReader getter = read_owner;
  __attribute__((objc_precise_lifetime)) id owner = record_object(2);
  Loan result = forward_owner(getter, owner);
  return result[0] == 'O' && result[1] == 'K' ? 0 : 3;
}

static int packed_cpp_lifetime() {
  PackedStrongLeaf empty;
  if (hasValue(empty.object))
    return 4;
  {
    PackedStrongLeaf source{42, record_value(1)};
    PackedStrongLeaf copied = copy_packed(source);
    source.object = None(id);
    PackedStrongLeaf moved = move_packed(copied);
    if (moved.marker != 42 || !hasValue(moved.object) || record_destroyed(1))
      return 5;
  }
  return record_destroyed(1) == 1 ? 0 : 6;
}

int main() {
  int result = borrowed_calls();
  if (result)
    return result;
  if (record_destroyed(2) != 1)
    return 7;
  return packed_cpp_lifetime();
}
