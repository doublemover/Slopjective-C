// Source-free/module import supplies BorrowedBytes, consume_noescape, and APIs.
BorrowedBytes imported_forward(id owner)
    __attribute__((objc_returns_borrowed(owner_index=0))) {
  consume_noescape(exported_borrow(owner));
  return exported_borrow(owner);
}
#ifdef BORROWED_IMPORT_REJECT
const char *imported_escape(id owner) { return exported_borrow(owner); }
#endif
