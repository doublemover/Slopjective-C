#if defined(REJECT_PRIVATE)
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
