#include "records.h"

#if defined(REJECT_RECORD_ASSIGNMENT)
void rejected(CarrierRecord &left, const CarrierRecord &right) { left = right; }
#elif defined(REJECT_RECORD_DEFAULT)
void rejected() { CarrierAggregate value; }
#elif defined(REJECT_RECORD_OMITTED)
void rejected() { CarrierAggregate value{None(RecordLeaf)}; }
#elif defined(REJECT_RECORD_EMPTY)
struct Rejected {
  RecordInner field;
  Rejected() : field() {}
};
void rejected() { Rejected value; }
#elif defined(REJECT_RECORD_PACKED)
struct __attribute__((packed)) Rejected {
  RecordInner field;
  Rejected() : field(Some(RecordLeaf, RecordLeaf(1))) {}
};
void rejected() { Rejected value; }
#elif defined(REJECT_RECORD_VOLATILE)
struct Rejected {
  volatile RecordInner field;
  Rejected() : field(Some(RecordLeaf, RecordLeaf(1))) {}
};
void rejected() { Rejected value; }
#elif defined(REJECT_RECORD_UNION)
union Rejected {
  RecordInner field;
  Rejected() : field(Some(RecordLeaf, RecordLeaf(1))) {}
  ~Rejected() {}
};
void rejected() { Rejected value; }
#elif defined(REJECT_RECORD_ARRAY)
struct Rejected {
  RecordInner fields[2];
  Rejected() : fields{None(RecordLeaf), None(RecordLeaf)} {}
};
void rejected() { Rejected value; }
#elif defined(REJECT_RECORD_BLOCK_ABI)
CarrierRecord (^rejected)(CarrierRecord) = ^(CarrierRecord value) { return value; };
#elif defined(REJECT_RECORD_OBJC_ABI)
__attribute__((objc_root_class)) @interface Rejected
- (CarrierRecord)exchange:(CarrierRecord)value;
@end
@implementation Rejected
- (CarrierRecord)exchange:(CarrierRecord)value { return value; }
@end
#elif defined(REJECT_RECORD_MIXED)
struct Rejected {
  RecordInner field;
  id object;
  Rejected() : field(None(RecordLeaf)), object(0) {}
};
void rejected() { Rejected value; }
#endif
