#include "carrier-heap-call.h"
#include <objc/runtime.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"heap storage %d: %s\n",__LINE__,#x); return __LINE__; } } while(0)
static int heap_ivars(void) {
  @autoreleasepool {
    StorageBox *box=[StorageBox new];
    CHECK(!__objc3_has_value(box.strong));
    box.strong=record_value(0);
    RecordStrong copy=box.strong;
    box.strong=record_value(1); CHECK(!record_destroyed(0));
    copy=__objc3_none(id); CHECK(record_destroyed(0)==1);
    id object=record_object(2); box.weak=__objc3_some(id __weak,object);
    object=(id)0;
    __objc3_match_stmt(box.weak) {
      case __objc3_case_some(let value): { CHECK(value==(id)0); }
      case __objc3_case_none: { CHECK(0); }
    }
    struct RecordPair source={42,record_value(3),__objc3_none(id __weak),{record_value(4)}};
    box.pair=source;
    source.first=__objc3_none(id);source.values[0]=__objc3_none(id);
    struct RecordPair other=box.pair;
    box->_array[0]=record_value(5);
    box=(StorageBox*)0;
    CHECK(record_destroyed(1)==1 && record_destroyed(5)==1 && !record_destroyed(3));
    other.first=__objc3_none(id); other.values[0]=__objc3_none(id);
    CHECK(record_destroyed(3)==1 && record_destroyed(4)==1);
  }
  CHECK(record_destroyed(2)==1 && heap_weak_reads()==1);
  StorageBox *missing=(StorageBox*)0;
  missing.strong=record_value(20); CHECK(record_destroyed(20)==1);
  CHECK(!__objc3_has_value(missing.strong));
  struct RecordPair input={42,record_value(21),__objc3_none(id __weak),{}};
  missing.pair=input; input.first=__objc3_none(id); CHECK(record_destroyed(21)==1);
  struct RecordPair empty=missing.pair;
  CHECK(!__objc3_has_value(empty.first) && !__objc3_has_value(empty.values[0]));
  return 0;
}
static int heap_captures(void) {
  StorageState state;
  {
    struct RecordPair value={42,record_value(10),__objc3_none(id __weak),{record_value(11)}};
    state=heap_capture_record(value);
  }
  CHECK(state(0) && !record_destroyed(10));
  state=(StorageState)0; CHECK(record_destroyed(10)==1 && record_destroyed(11)==1);
  state=heap_capture_shared(12); CHECK(state(0) && !record_destroyed(12));
  CHECK(!state(1) && record_destroyed(12)==1); state=(StorageState)0;
  state=heap_capture_shared_record(13); CHECK(state(0) && !record_destroyed(13));
  CHECK(!state(1) && record_destroyed(13)==1);state=(StorageState)0;
  StorageState first,second,weak_state;
  {
    __block RecordStrong shared=record_value(22);
    first=^int(int clear) { if(clear) shared=__objc3_none(id); return __objc3_has_value(shared); };
    second=^int(int clear) { if(clear) shared=__objc3_none(id); return __objc3_has_value(shared); };
  }
  first=(StorageState)0; CHECK(second(0) && !record_destroyed(22));
  CHECK(!second(1) && record_destroyed(22)==1);second=(StorageState)0;
  {
    id object=record_object(23);
    __block RecordWeak weak=__objc3_some(id __weak,object);
    weak_state=^int(int unused) {
      __objc3_match_stmt(weak) {
        case __objc3_case_some(let payload): { return payload==(id)0; }
        case __objc3_case_none: { return -1; }
      }
      return -2;
    };
    CHECK(weak_state(0)==0);
  }
  CHECK(record_destroyed(23)==1 && weak_state(0)==1);weak_state=(StorageState)0;
  return 0;
}
static int property_failure(void) {
  @autoreleasepool {
    PropertyFailure *box=[PropertyFailure new];
    @try { box.value=record_value(30); CHECK(0); }
    @catch(id error) { CHECK(error && !record_destroyed(30)); }
    @try { box.pair=(struct RecordPair){42,record_value(32),__objc3_none(id __weak),{}}; CHECK(0); }
    @catch(id error) { CHECK(error && !record_destroyed(32)); }
    box=(PropertyFailure*)0;
  }
  CHECK(record_destroyed(30)==1 && record_destroyed(31)==1 && record_destroyed(32)==1 && record_destroyed(33)==1);
  return 0;
}
#ifdef __cplusplus
struct NestedFieldCtor {
  RecordPair value;
  NestedFieldCtor(int key) :value{42,record_value(key),__objc3_none(id __weak),{record_value(key+1)}} {}
};
static int nested_constructor(void) {
  { NestedFieldCtor value(34); CHECK(__objc3_has_value(value.value.first)); }
  CHECK(record_destroyed(34)==1 && record_destroyed(35)==1);
  return 0;
}
#endif
static int runtime_records(void) {
  StorageBox *box=[StorageBox new], *missing=(StorageBox*)0;
  {
    id object=record_object(37);
    struct RecordPair value={42,record_value(36),__objc3_some(id __weak,object),{}};
    struct RecordPair echoed=[box echo:value]; CHECK(echoed.marker==43);
    HeapRecordCall block=heap_record_block();
    struct RecordPair mapped=heap_invoke_block(block,value); CHECK(mapped.marker==43);
    struct RecordPair absent=[missing echo:value]; CHECK(!__objc3_has_value(absent.first));
    value.first=__objc3_none(id); echoed.first=__objc3_none(id); CHECK(!record_destroyed(36));
    mapped.first=__objc3_none(id); CHECK(record_destroyed(36)==1);
  }
  CHECK(record_destroyed(37)==1);
  return 0;
}
int main(void) { CHECK(heap_ivars()==0);CHECK(heap_captures()==0);CHECK(property_failure()==0);CHECK(runtime_records()==0);
#ifdef __cplusplus
CHECK(nested_constructor()==0);
#endif
return 0; }
