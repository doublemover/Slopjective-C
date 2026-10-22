#if defined(REJECT_CXX_NEW) || defined(REJECT_CXX_PLACEMENT) || defined(REJECT_CXX_DELETE)
#include "boundary.h"
void *operator new(__SIZE_TYPE__, void *) noexcept;
#if defined(REJECT_CXX_NEW)
BoundaryTree *new_cpp_carrier() { return new BoundaryTree{}; }
#elif defined(REJECT_CXX_PLACEMENT)
void replace_cpp_carrier(BoundaryTree &source) { new (&source) BoundaryTree{}; }
#else
void delete_cpp_carrier(BoundaryTree *source) { delete source; }
#endif
#elif defined(REJECT_MIXED_NEW) || defined(REJECT_MIXED_NEW_VALUE) || defined(REJECT_MIXED_NEW_ARRAY) || defined(REJECT_MIXED_PLACEMENT) || defined(REJECT_MIXED_DELETE)
#include "mixed-boundary.h"
void *operator new(__SIZE_TYPE__, void *) noexcept;
#if defined(REJECT_MIXED_NEW)
MixedTree *new_mixed_carrier() { return new MixedTree; }
#elif defined(REJECT_MIXED_NEW_VALUE)
MixedTree *new_mixed_value() { return new MixedTree{}; }
#elif defined(REJECT_MIXED_NEW_ARRAY)
MixedTree *new_mixed_array() { return new MixedTree[2]{}; }
#elif defined(REJECT_MIXED_PLACEMENT)
void replace_mixed_carrier(MixedTree &source) { new (&source) MixedTree{}; }
#else
void delete_mixed_carrier(MixedTree *source) { delete source; }
#endif
#elif defined(REJECT_MIXED_CAPTURE)
#include "mixed-boundary.h"
void mixed_capture() {
  MixedTree source = Err(MixedManaged, MixedNative, None(MixedItem));
  void (^capture)(void) = ^{ (void)source; };
  capture();
}
#elif defined(REJECT_MIXED_RECORD)
#include "mixed-boundary.h"
struct MixedContainer {
  MixedTree value;
  MixedContainer() : value(Err(MixedManaged, MixedNative, None(MixedItem))) {}
};
void mixed_record() { MixedContainer object; }
#elif defined(REJECT_MIXED_LAMBDA)
#include "mixed-boundary.h"
void mixed_lambda() {
  auto capture = [value = Err(MixedManaged, MixedNative, None(MixedItem))] {};
  capture();
}
#elif defined(REJECT_MIXED_ASSIGNMENT)
#include "mixed-boundary.h"
void mixed_assignment(MixedTree &source) {
  source = Err(MixedManaged, MixedNative, Some(MixedItem, MixedItem(1)));
}
#elif defined(REJECT_MIXED_CALL_ABI)
#include "mixed-boundary.h"
MixedTree mixed_by_value(MixedTree source) { return copyValue(source); }
#elif defined(REJECT_MIXED_ARRAY)
#include "mixed-boundary.h"
void mixed_array() {
  MixedTree values[2] = {
      Ok(MixedManaged, MixedNative, None(id)),
      Err(MixedManaged, MixedNative, None(MixedItem))};
}
#elif defined(REJECT_MIXED_VOLATILE)
#include "mixed-boundary.h"
void mixed_volatile(volatile MixedTree &source) {
  MixedTree copied = copyValue(source);
}
#elif defined(REJECT_MIXED_MRC)
#include "mixed-boundary.h"
void mixed_mrc() {
  MixedTree value = Err(MixedManaged, MixedNative, None(MixedItem));
}
#elif defined(REJECT_PRIVATE)
class PrivateDestructor { ~PrivateDestructor(); };
using Inner = Optional(PrivateDestructor);
void private_nested_destructor() {
  Optional(Inner) value = None(Inner);
}
#elif defined(REJECT_DELETED_COPY)
struct MoveOnly {
  MoveOnly(const MoveOnly &) = delete;
  MoveOnly(MoveOnly &&);
  ~MoveOnly();
};
using Inner = Optional(MoveOnly);
using Outer = Result(int, Inner);
void deleted_nested_copy(Outer &source) {
  Outer value = copyValue(source);
}
#elif defined(REJECT_ARC_RECORD)
struct Managed { id object; };
using Inner = Optional(Managed);
void managed_record_composition() {
  Optional(Inner) value = None(Inner);
}
#elif defined(REJECT_CALL_ABI)
#include "boundary.h"
BoundaryTree by_value(BoundaryTree source) {
  return copyValue(source);
}
#elif defined(REJECT_VOLATILE)
#include "boundary.h"
using VolatileInner = volatile BoundaryInner;
void nested_volatile(Optional(VolatileInner) &source) {
  auto value = copyValue(source);
}
#else
#error A specific recursive carrier rejection must be selected
#endif
