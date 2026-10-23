// The coordinated batch selects one rejection at a time using CASE_* macros.
#include "contracts.h"
const char *global_pointer;
void (^global_block)(void);
struct Holder { const char *pointer; };

#if defined(CASE_GLOBAL)
void rejected(id owner) { global_pointer = borrowed_bytes(owner); }
#elif defined(CASE_STATIC)
void rejected(id owner) { static const char *pointer; pointer = borrowed_bytes(owner); }
#elif defined(CASE_TLS)
void rejected(id owner) { static __thread const char *pointer; pointer = borrowed_bytes(owner); }
#elif defined(CASE_HEAP)
void rejected(id owner, struct Holder *holder) { holder->pointer = borrowed_bytes(owner); }
#elif defined(CASE_ARGUMENT)
void rejected(id owner) { consume_untracked(borrowed_bytes(owner)); }
#elif defined(CASE_BLOCK_ARGUMENT)
void rejected(id owner) {
  const char *pointer = borrowed_bytes(owner);
  invoke_escaping(^{ consume_borrowed(pointer); });
}
#elif defined(CASE_BLOCK_STORE)
void rejected(id owner) {
  const char *pointer = borrowed_bytes(owner);
  global_block = ^{ consume_borrowed(pointer); };
}
#elif defined(CASE_RETURN)
const char *rejected(id owner) { return borrowed_bytes(owner); }
#elif defined(CASE_WRONG_OWNER)
BorrowedBytes rejected(id wrong, id actual)
    __attribute__((objc_returns_borrowed(owner_index=0))) {
  return borrowed_bytes(actual);
}
#elif defined(CASE_ERASURE)
unsigned long rejected(id owner) { return (unsigned long)borrowed_bytes(owner); }
#elif defined(CASE_UNSTABLE)
void rejected(void) { consume_borrowed(borrowed_bytes(make_owner())); }
#elif defined(CASE_OWNER_REPLACED)
void rejected(id owner, id replacement) {
  const char *pointer = borrowed_bytes(owner);
  owner = replacement;
  consume_borrowed(pointer);
}
#elif defined(CASE_OWNER_ENDED)
void rejected(void) {
  const char *pointer;
  { id owner = make_owner(); pointer = borrowed_bytes(owner); }
  consume_borrowed(pointer);
}
#elif defined(CASE_ADDRESS_ALIAS)
void rejected(id owner) {
  const char *pointer = borrowed_bytes(owner);
  const char **alias = &pointer;
  invoke_noescape(^{ consume_untracked(*alias); });
}
#elif defined(CASE_NOESCAPE_DEFINITION)
void rejected(const char *pointer __attribute__((noescape))) {
  global_pointer = pointer;
}
#elif defined(CASE_CARRIER_CAPTURE)
void rejected(id owner) {
  Optional(BorrowedBytes) payload = Some(BorrowedBytes, borrowed_bytes(owner));
  invoke_escaping(^{ (void)payload; });
}
#elif defined(CASE_MOVED_CAPTURE)
void rejected(id owner) {
  const char *pointer = borrowed_bytes(owner);
  invoke_escaping(^[move pointer] { consume_borrowed(pointer); });
}
#elif defined(CASE_NESTED_FIELDS)
void rejected(id owner) {
  struct Pair { struct Holder first, second; };
  struct Pair value = {{borrowed_bytes(owner)}, {0}};
  value.second.pointer = 0;
  consume_untracked(value.first.pointer);
}
#elif defined(CASE_AGGREGATE_REPRESENTATION)
void rejected(id owner, void *heap) {
  struct Holder value = {borrowed_bytes(owner)};
  __builtin_memcpy(heap, &value, sizeof value);
}
#elif defined(CASE_FUTURE_ALIAS_LOAN)
void rejected(id owner, int choose) {
  const char *first = 0, *second = 0;
  const char **alias;
  if (choose) alias = &first; else alias = &second;
  first = borrowed_bytes(owner);
  invoke_noescape(^{ consume_untracked(*alias); });
}
#elif defined(CASE_BOUND_PAYLOAD)
void rejected(id owner) {
  Optional(BorrowedBytes) source = Some(BorrowedBytes, borrowed_bytes(owner));
  guard let pointer = source else { return; }
  consume_untracked(pointer);
}
#elif defined(CASE_NOESCAPE_CONSTRUCTOR) && defined(__cplusplus)
struct Saved {
  const char *pointer;
  Saved(const char *source __attribute__((noescape))) : pointer(source) {}
};
#elif defined(CASE_DYNAMIC_GLOBAL) && defined(__cplusplus)
extern id owner;
const char *rejected = borrowed_bytes(owner);
#endif
