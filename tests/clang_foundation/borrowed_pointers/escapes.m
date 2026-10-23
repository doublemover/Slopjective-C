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
#endif
