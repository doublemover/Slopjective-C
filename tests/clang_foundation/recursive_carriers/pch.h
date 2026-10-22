#include "boundary.h"

template <class Carrier> void pch_copy_recursive(const Carrier &source) {
  Carrier copied = copyValue(source);
  if (!isOk(copied))
    __builtin_trap();
}

inline void pch_move_recursive(BoundaryTree &source) {
  BoundaryTree moved = moveValue(source);
  if (!isOk(moved))
    __builtin_trap();
}
