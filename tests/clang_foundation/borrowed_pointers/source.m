// Intended for the coordinated C17/C++20/C++23 ARC/MRC semantic batch.
#include "contracts.h"

// The spelling remains contextual in both declaration and expression grammar.
int borrowed(int value) { return value; }
int identifier_context(void) { return borrowed(3); }

BorrowedBytes forward_owner(id owner)
    __attribute__((objc_returns_borrowed(owner_index=0))) {
  return borrowed_bytes(owner);
}

void local_consumption(id owner, int condition) {
  borrowed const char *pointer = borrowed_bytes(owner);
  const char *alias = pointer + 1;
  void *erased = (void *)alias;
  consume_noescape((const char *)erased);
  consume_borrowed(alias);
  invoke_noescape(^{ consume_borrowed(pointer); });
  (^{ consume_borrowed(pointer); })();
  if (condition)
    alias = 0;
  else
    alias = "owned literal";
  consume_untracked(alias); // Every incoming path kills the borrowed value.
  withLifetime(owner) {
    borrowed const char *bounded = borrowed_bytes(owner);
    consume_borrowed(bounded);
  }
  keepAlive(owner);
  consume_borrowed(borrowed_bytes(owner));
}

int copying_data(id owner) {
  const char *pointer = borrowed_bytes(owner);
  char output[4];
  __builtin_memcpy(output, pointer, sizeof output);
  return output[0] + pointer[0]; // The copied scalar data is owned.
}
