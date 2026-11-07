#ifndef BORROWED_POINTER_FIXTURE_CONTRACTS_H
#define BORROWED_POINTER_FIXTURE_CONTRACTS_H
typedef borrowed const char *BorrowedBytes;
BorrowedBytes borrowed_bytes(id owner)
    __attribute__((objc_returns_borrowed(owner_index=0)));
BorrowedBytes borrowed_second(id ignored, id owner)
    __attribute__((objc_returns_borrowed(owner_index=1)));
void consume_borrowed(borrowed const char *pointer);
void consume_noescape(const char *pointer __attribute__((noescape)));
void invoke_noescape(void (^work)(void) __attribute__((noescape)));
void invoke_escaping(void (^work)(void));
void consume_untracked(const char *pointer);
id make_owner(void);
#endif
