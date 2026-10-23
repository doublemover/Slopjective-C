// Compile with contracts.h as a native PCH, without a textual include.
BorrowedBytes forwarded_from_pch(id owner)
    __attribute__((objc_returns_borrowed(owner_index=0))) {
  consume_noescape(borrowed_bytes(owner));
  return borrowed_bytes(owner);
}
#ifdef BORROWED_PCH_REJECT
const char *rejected_from_pch(id owner) { return borrowed_bytes(owner); }
#endif
