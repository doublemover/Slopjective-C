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

// Mutable local address aliases expose current pointee facts, including kills.
void local_address_alias(id owner) {
  const char *pointer = borrowed_bytes(owner);
  const char **alias;
  alias = &pointer;
  consume_borrowed(*alias);
  *alias = 0;
  consume_untracked(pointer);
}

// A pin created after acquisition extends that same owner's existing loans.
void late_lifetime_pin(id owner, id replacement) {
  const char *pointer = borrowed_bytes(owner);
  keepAlive(owner);
  owner = replacement;
  consume_borrowed(pointer);
}

void ended_pin_unchanged_owner(id owner) {
  const char *pointer = borrowed_bytes(owner);
  withLifetime(owner) { consume_borrowed(pointer); }
  consume_borrowed(pointer);
}

void nullable_branch(id owner) {
  const char *pointer = borrowed_bytes(owner);
  if (!pointer)
    consume_untracked(pointer);
  else
    consume_borrowed(pointer);
}

void moved_capture(id owner) {
  const char *pointer = borrowed_bytes(owner);
  invoke_noescape(^[move pointer] { consume_borrowed(pointer); });
  consume_untracked(pointer); // move initialized the source pointer to null.
}

void nested_payload(id owner) {
  Optional(BorrowedBytes) payload = Some(BorrowedBytes, borrowed_bytes(owner));
  invoke_noescape(^{ (void)payload; });
}
