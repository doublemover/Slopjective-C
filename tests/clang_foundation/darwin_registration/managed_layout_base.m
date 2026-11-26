#include "managed_layout_api.h"

static int events[16];
static unsigned count;
static __weak DarwinManagedChild *owner_observer;

void compound_managed_event(int value) {
  if (count < sizeof(events) / sizeof(events[0]))
    events[count++] = value;
  if (value == 2 && owner_observer)
    events[0] = -2;
}

void managed_observe_owner(DarwinManagedChild *owner) {
  owner_observer = owner;
}

unsigned managed_event_count(void) {
  return count;
}

int managed_event_at(unsigned index) {
  return events[index];
}

@implementation DarwinManagedRoot
- (void)dealloc {
  compound_managed_event(4);
}
@end

@implementation DarwinManagedParent {
  id _inherited;
  __weak id _observed;
}
- (void)setInherited:(id)value {
  _inherited = value;
  _observed = value;
}
- (id)inherited {
  return _observed == _inherited ? _inherited : (id)0;
}
- (void)dealloc {
  compound_managed_event(_inherited ? 3 : -3);
}
@end

@implementation DarwinManagedValue {
  int _tag;
}
- (void)setTag:(int)tag {
  _tag = tag;
}
- (void)dealloc {
  compound_managed_event(_tag);
}
@end
