#include "boundary.h"

int main() {
  {
    BoundaryTree source = Ok(
        BoundaryOuter, BoundaryInner,
        Some(BoundaryInner, Some(BoundaryItem, BoundaryItem(1))));
    BoundaryTree copied = copyValue(boundary_borrow(source));
    BoundaryTree moved = moveValue(boundary_mutable_borrow(source));
    if (!isOk(source) || !isOk(copied) || !isOk(moved) ||
        boundary_constructions != 1 || boundary_copies != 1 ||
        boundary_moves != 1 || boundary_live != 3 || boundary_borrows != 2)
      return 1;
    boundary_throw_copy = true;
    try {
      BoundaryTree failed = copyValue(boundary_borrow(source));
      return 2;
    } catch (int code) {
      if (code != 71 || boundary_live != 3 || boundary_destructions)
        return 3;
    }
    boundary_throw_copy = false;
  }
  if (boundary_live || boundary_destructions != 3)
    return 4;
  {
    BoundaryTree failure = Err(
        BoundaryOuter, BoundaryInner, Some(BoundaryItem, BoundaryItem(2)));
    BoundaryTree copied = copyValue(boundary_borrow(failure));
    if (isOk(failure) || isOk(copied) || boundary_live != 2 ||
        boundary_copies != 2)
      return 5;
  }
  return boundary_live || boundary_destructions != 5 ? 6 : 0;
}
