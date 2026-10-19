#include "carrier-record-call.h"
#include <objc/runtime.h>
static int destroyed[128], order[128], next_order;
__attribute__((objc_root_class))
@interface RecordProbe <Error> { Class isa; @public int key; }
+ (id)newWithKey:(int)value;
@end
@implementation RecordProbe
+ (id)newWithKey:(int)value {
  RecordProbe *object = class_createInstance(self, 0);
  object->key = value;
  return object;
}
- (void)dealloc {
  ++destroyed[key]; order[next_order++] = key; object_dispose(self);
}
@end
id record_object(int key) { return [RecordProbe newWithKey:key]; }
int record_destroyed(int key) { return destroyed[key]; }
int record_destroyed_order(int index) { return order[index]; }
