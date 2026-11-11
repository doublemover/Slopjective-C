#include "mixed-boundary.h"

int main() {
  mixed_reset();
  __weak id observer = 0;
  {
    id object = [MixedProbe new];
    observer = object;
    MixedTree source = Ok(MixedManaged, MixedNative, Some(id, object));
    object = 0;
    MixedTree copied = copyValue(mixed_borrow(source));
    MixedTree moved = moveValue(mixed_mutable_borrow(source));
    if (!isOk(copied) || !isOk(moved) || mixed_borrows != 2 || !observer)
      return 1;
  }
  if (observer || mixed_objects_destroyed != 1)
    return 2;
  {
    MixedTree source = Err(MixedManaged, MixedNative, Some(MixedItem, MixedItem(6)));
    MixedTree copied = copyValue(mixed_borrow(source));
    MixedTree moved = moveValue(mixed_mutable_borrow(source));
    if (isOk(source) || isOk(copied) || isOk(moved) || mixed_borrows != 4 ||
        mixed_live != 3 || mixed_copies != 1 || mixed_moves != 1)
      return 3;
  }
  return mixed_live || mixed_destructions != 3 ? 4 : 0;
}
