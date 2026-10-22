#include "records.h"

template <class T> T record_pch_copy(const T &value) { return T(value); }
template <class T> T record_pch_move(T &value) {
  return T(static_cast<T &&>(value));
}
