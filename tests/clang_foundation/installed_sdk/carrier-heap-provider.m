#include "carrier-heap-call.h"
#include <objc/runtime.h>
static int weak_reads;
int heap_weak_reads(void) { return weak_reads; }
@implementation StorageBox
@synthesize strong=_strong, weak=_weak, pair=_pair;
+ (id)new { return class_createInstance(self,0); }
- (RecordWeak)weak { ++weak_reads; return _weak; }
- (struct RecordPair)echo:(struct RecordPair)value { ++value.marker; return value; }
- (void)dealloc { object_dispose(self); }
@end
@implementation PropertyFailure
@synthesize value=_value,pair=_pair;
+ (id)new { return class_createInstance(self,0); }
- (void)setValue:(RecordStrong)value { _value=value; @throw record_object(31); }
- (void)setPair:(struct RecordPair)value { _pair=value; @throw record_object(33); }
- (void)dealloc { object_dispose(self); }
@end
StorageState heap_capture_record(struct RecordPair value) {
  return ^int(int unused) { return value.marker==42 && __objc3_has_value(value.first); };
}
StorageState heap_capture_shared(int key) {
  __block RecordStrong value=record_value(key);
  return ^int(int clear) {
    if(clear) value=__objc3_none(id);
    return __objc3_has_value(value);
  };
}
StorageState heap_capture_shared_record(int key) {
  __block struct RecordPair value={42,record_value(key),__objc3_none(id __weak),{}};
  return ^int(int clear) {
    if(clear) value.first=__objc3_none(id);
    return value.marker==42 && __objc3_has_value(value.first);
  };
}

HeapRecordCall heap_record_block(void) {
  return ^struct RecordPair(struct RecordPair value) { ++value.marker; return value; };
}
struct RecordPair heap_invoke_block(HeapRecordCall block, struct RecordPair value) { return block(value); }
