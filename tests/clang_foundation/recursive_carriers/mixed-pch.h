#include "mixed-boundary.h"

template <class Carrier> void mixed_pch_copy(const Carrier &source) {
  Carrier copied = copyValue(source);
}
inline void mixed_pch_move(MixedTree &source) {
  MixedTree moved = moveValue(source);
}
