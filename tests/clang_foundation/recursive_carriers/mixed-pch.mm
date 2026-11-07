#include "mixed-boundary.h"

int main() {
  mixed_reset();
  __weak id observer = 0;
  {
    id object = [MixedProbe new];
    observer = object;
    MixedTree source = Ok(MixedManaged, MixedNative, Some(id, object));
    object = 0;
    mixed_pch_copy(source);
    if (!observer || mixed_objects_destroyed) return 1;
    mixed_pch_move(source);
    if (observer || mixed_objects_destroyed != 1 || !isOk(source)) return 2;
  }
  {
    MixedTree source = Err(MixedManaged, MixedNative, Some(MixedItem, MixedItem(7)));
    mixed_pch_copy(source);
    mixed_pch_move(source);
    if (mixed_live != 1 || mixed_destructions != 2 ||
        mixed_copies != 1 || mixed_moves != 1) return 3;
  }
  return mixed_live || mixed_destructions != 3 ? 4 : 0;
}
