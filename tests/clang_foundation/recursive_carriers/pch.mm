#include "boundary.h"

int main() {
  {
    BoundaryTree source = Ok(
        BoundaryOuter, BoundaryInner,
        Some(BoundaryInner, Some(BoundaryItem, BoundaryItem(3))));
    pch_copy_recursive(source);
    pch_move_recursive(source);
    if (!isOk(source) || boundary_live != 1 || boundary_copies != 1 ||
        boundary_moves != 1 || boundary_destructions != 2)
      return 1;
  }
  return boundary_live || boundary_destructions != 3 ? 2 : 0;
}
