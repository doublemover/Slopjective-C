#pragma once

typedef borrowed const char *Loan;
using BorrowingReader = int (*)(Loan);
using PlainReader = int (*)(const char *);
using BorrowingBlock = int (^)(Loan);
typedef Loan (*OwnerReader)(id)
    __attribute__((objc_returns_borrowed(owner_index=0)));

template<class Reader>
__attribute__((noinline))
int observe(Reader reader, Loan value) {
  return reader(value);
}

template<class Reader>
__attribute__((noinline))
int count_calls(Reader) {
  static int count;
  return ++count;
}

template<class Reader>
__attribute__((noinline, objc_returns_borrowed(owner_index=1)))
Loan forward_owner(Reader reader, id owner) {
  return reader(owner);
}
