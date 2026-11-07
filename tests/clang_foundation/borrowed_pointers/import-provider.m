#include "contracts.h"
BorrowedBytes exported_borrow(id owner)
    __attribute__((objc_returns_borrowed(owner_index=0))) {
  return borrowed_bytes(owner);
}
